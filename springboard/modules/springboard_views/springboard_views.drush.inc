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
  $output = '';
  $current_page = 0;
  $total_results = 0;
  $default_items_per_page = 50000; // If the view returns all results
  $max_page = 100; // Primarily for testing; secondarily to avoid an infinite loop.

  // Export each page of results into its own CSV file:
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
    $drush_options = (array) json_decode(drush_get_option('json'));
    $order = 'ASC';
    unset($sort);
    foreach ($drush_options as $option_name => $option_value) {
      // Collect sorting data:
      if ($option_name == 'sort') {
        $sort = $option_value;
        continue;
      }
      // Collect ordering data:
      if ($option_name == 'order') {
        $order = strtoupper($option_value);
        continue;
      }
      // Collect the complete message email:
      if ($option_name == 'export_email') {
        $export_email = $option_value;
        continue;
      }
      // Apply filters:
      $filter = $view->get_item($display_id, 'filter', $option_name);
      $filter['value']['value'] = $option_value;
      $view->set_item($display_id, 'filter', $option_name, $filter);
    }

    // Apply sort and order, if specified:
    if (isset($sort)) {
      // how ???
    }

    $view->pre_execute();
    $view->execute();

    // Save the output to a file, if results were returned; otherwise stop:
    if (isset($view->result) && count($view->result) > 0) {
      $output = $view->render();
      $temp_name = drupal_tempnam('temporary://', date('Y-m-d--H-i-s') . '-donations-export-page-' . ($current_page + 1) . '--');
      if (file_put_contents($temp_name . '.csv', $output)) {
        $file_paths[] = $temp_name;
        $total_results += count($view->result); 
      }
      else {
        echo json_encode(array('error' => 'Export failed for file: "' . $temp_name . '"; aborting.'));
        return;
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
  echo "\n\n"; print_r($export_summary);echo "\n\n";

  echo json_encode($export_summary);
}
