<?php

/**
 * Implements hook_drush_command().
 *
 * Adds a drush command which saves a CSV file containing donation data from the Drupal DB.
 */
function springboard_views_drush_command() {
  return array(
    'sb-export-donations-csv' => array(
      'aliases' => array('sb-edc'),
      'description' => t('Exports a CSV file containing donation data from the Drupal DB.'),
      'callback' => '_springboard_views_drush_export_donations_csv',
      'options' => array(
        'json' => t('JSON any key/value search filters, sort (the field on which to sort), and order (asc or desc).'),
      ),
      'drupal dependencies' => array('views', 'commerce', 'commerce_order', 'webform', 'fundraiser_webform', 'fundraiser_commerce', 'fundraiser', 'springboard_views'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
  );
}

/**
 * Callback for sb-export-donations-csv.
 */
function _springboard_views_drush_export_donations_csv() {
  // Grant full access so the view can be executed:
  $file_paths = array();
  $start = time();
  global $user; 
  drupal_save_session(FALSE); 
  $user = user_load(1);
  $output_path = 'temporary://'; // Default output path if it is not overwritten.
  $current_page = 0;
  $total_results = 0;
  $default_items_per_page = 250000; // If the view returns all results
  $max_page = 100; // Primarily for testing; secondarily to avoid an infinite loop.
  $debug = FALSE; // Set this to true to prevent CSV export file generation.

  // Export each page of results into its own CSV file:
  $drush_options = (array) json_decode(drush_get_option('json'));
  $dw_drupal_key_map = _springboard_views_drush_export_donations_dw_key_map();
  echo "\n\n" . print_r($drush_options, 1) . "\n\n";
  while ($current_page < $max_page) {
    // Load the view.
    $view = views_get_view('sbv_donations');
    if (!isset($view) || $view == FALSE || !is_object($view)) {
      echo json_encode(array('error' => 'View sbv_donations could not be loaded; aborting.'));
      return;
    }
    $display_id = 'views_data_export_1';
    $view->set_display($display_id);
    $view->init_handlers();

    // Set the appropriate offset; if items per page is missing then assume 25K/page:
    $pager = $view->display_handler->get_option('pager');
    if (isset($pager['options']['items_per_page']) && $pager['options']['items_per_page'] > 0) {
      $items_per_page = $pager['options']['items_per_page'];
    }
    else {
      $items_per_page = $default_items_per_page;
      $view->set_items_per_page($items_per_page);
    }
    $view->set_offset($current_page * $items_per_page);




    // Apply filtering, sorting, and ordering:  
    $order = 'ASC';
    unset($sort);
    foreach ($drush_options as $option_name => $option_value) {
      // Collect sorting data:
      if ($option_name == 'sort') {
        $sort = $option_value;
        continue;
      }
      // Collect ordering data:
      elseif ($option_name == 'order') {
        $order = strtoupper($option_value);
        continue;
      }
      // Collect the complete message email:
      elseif ($option_name == 'export_email') {
        $export_email = $option_value;
        continue;
      }
      // Check for debug mode where files don't get created:
      elseif ($option_name == 'debug') {
        $debug = TRUE;
        continue; 
      }
      // Check for a target output path:
      elseif ($option_name == 'output_path') {
        $output_path = $option_value;
        continue;
      }

      // Support an array value if transaction date filtering is done:
      if ($option_name == 'transaction_date') {
        unset($min_date); unset($max_date);
        $expected_date_format = 'L, F j, Y';
        if (isset($option_value->min) && isset($option_value->max)) {
          $min_date = strtotime($option_value->min);
          $max_date = strtotime($option_value->max);
        }
        elseif (isset($option_value->min)) {
          $min_date = strtotime($option_value->min);
          $max_date = date("m/d/Y", time() + (60*60*24));
        }
        elseif (isset($option_value->max)) {
          $min_date = strtotime('1/1/1980');
          $max_date = strtotime($option_value->max);
        }
        if (isset($min_date) && isset($max_date)) {
          $view->exposed_input['date_filter']['min']['date'] = date($expected_date_format, $min_date);
          $view->exposed_input['date_filter']['max']['date'] = date($expected_date_format, $max_date);
        }
        continue;
      }
    
      // Apply filters:
      if (isset($dw_drupal_key_map[$option_name])) {
        echo '"' . $option_value . '"';
        $filter_key = $dw_drupal_key_map[$option_name];
        $view->exposed_input[$filter_key] = $option_value;
      }
      else {
        echo json_encode(array('error' => 'Bad filter \'' . $option_name . '\'; aborting'));
        return;
      }
    }

    // Apply sort and order, if specified:
    if (isset($sort)) {
      // how ???
    }
    echo "\n\n" . print_r($view->exposed_input, 1) . "\n\n";
    $view->pre_execute();
    $view->execute();

    // Save the output to a file, if results were returned; otherwise stop:
    if (isset($view->result) && count($view->result) > 0) {
      $output = $view->render();
      $filename = $output_path . '/' . date('Y-m-d--H-i-s') . '-donations-export-page-' . ($current_page + 1) . '.csv';
      $file_paths[] = $filename;
      $total_results += count($view->result); 
      if (!$debug) {
        if (!file_put_contents($filename, $output)) {
          echo json_encode(array('error' => 'Export failed for file: "' . $temp_name . '"; aborting.'));
          return;
        }
      }
    }
    else {  
      break; 
    } 

    // Increment the page number: 
    $current_page++;
  }
  $duration = (time() - $start) . 's';
  $export_summary = array(
    'start' => date('Y-m-d - H:i:s', $start),
    'end' => date('Y-m-d - H:i:s'),
    'duration' => $duration,
    'total_results_exported' => $total_results,
    'export_file_count' => count($file_paths),
    'export_file_paths' => $file_paths,
  );
  if ($debug) {
    $export_summary['file_gen_disabled'] = 1;
  }
  echo "\n\n"; print_r($export_summary);echo "\n\n";

  echo json_encode($export_summary);

  echo "\n\n"; 
}

/**
 * Helper function which returns an array that maps data warehouse field keys 
 * to the export donation view's fields used for filtering and sorting:
 */
function _springboard_views_drush_export_donations_dw_key_map() {
  return array(
    'amount' => 'amount',
    'currency' => 'currency',
    'created_at' => 'created',
    'email' => 'mail',
    'form_id' => 'nid',
    'gateway' => 'gateway',
    'internal_name' => 'field_fundraiser_internal_name_value',
    'status' => 'status',
    'transaction_id' => 'txn_id',
    'transaction_date' => 'date_filter',
    'updated_at' => 'changed',
  );
}
