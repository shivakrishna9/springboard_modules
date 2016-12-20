<?php

/**
 * Implements hook_drush_command().
 *
 * Adds a drush command which saves a CSV file containing advocacy data from the Drupal DB.
 */
function springboard_export_drush_command() {
  $drupal_dependencies = array(
    'views',
    'commerce',
    'commerce_order',
    'webform',
    'fundraiser_webform',
    'fundraiser_commerce',
    'fundraiser',
    'springboard_views',
    'springboard_advocacy',
  );
  return array(
    'sb-export-donations-csv' => array(
      'aliases' => array('sb-edc'),
      'description' => t('Exports a CSV file containing donation data from the Drupal DB.'),
      'callback' => 'springboard_export_drush_export_donations_csv',
      'options' => array(
        'json' => t('JSON any key/value search filters, sort (the field on which to sort), and order (asc or desc).'),
      ),
      'drupal dependencies' => $drupal_dependencies,
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
    'sb-export-action-submissions-csv' => array(
      'aliases' => array('sb-easc'),
      'description' => t('Exports a CSV file containing advocacy action submission data from the Drupal DB.'),
      'callback' => 'springboard_export_drush_export_action_submissions_csv',
      'options' => array(
        'json' => t('JSON any key/value search filters, sort (the field on which to sort), and order (asc or desc).'),
      ),
      'drupal dependencies' => $drupal_dependencies,
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
  );
}

/**
 * Callback for drush sb-export-donations-csv (alias: sb-edc)
 */
function springboard_export_drush_export_donations_csv() {
  $drush_field_map = _springboard_export_donation_drush_to_drupal_field_map();
  _springboard_export_drush_export_view_data_csv(
    'sbv_donations',
    'views_data_export_1',
    'donations',
    $drush_field_map
  );
}

/**
 * Callback for drush sb-export-action-submissions-cs (alias: sb-easc)
 */
function springboard_export_drush_export_action_submissions_csv() {
  $drush_field_map = _springboard_export_action_drush_to_drupal_field_map();
  _springboard_export_drush_export_view_data_csv(
    'sba_action_submissions',
    'views_data_export_1',
    'action-subsmissions',
    $drush_field_map
  );
}

/**
 * Fetch and generate Views Data Export-based CSV output.
 *
 * @param $view_id
 *   View from which data is queried.
 * @param $export_display_id
 *   Denotes the views data export display to be used for fetching and rendering csv file content.
 * @param $filename_descr
 *   Used as part of the filename for this export.
 * @param $drush_field_map
 *   Defines how to alter the query for this view based on drush filtering and sorting.
 */
function _springboard_export_drush_export_view_data_csv($view_id, $export_display_id, $filename_descr, $drush_field_map) {
  // Ensure a minimum of 1.5GB RAM is available, which should be more than enough:
  ini_set('memory_limit', '1536M');

  // Grant full access so the view can be executed:
  $start = time();
  global $user;
  drupal_save_session(FALSE);
  $user = user_load(1);
  
  //$output_path = variable_get('springboard_dw_reports_export_path', 'temporary://'); // Default output path if it is not overwritten.
  $output_path = 'temporary://';
  $current_page = 0;
  $total_results = 0;
  $file_paths = array();

  // $items_per_page = variable_get('springboard_dw_reports_batch_count', 125000); // This determines the number of results per file.
  $items_per_page = 125000; // Multiple pages are output to a single file to greatly reduce max memory used.
  $max_page = 100; // To prevent an infinite loop.
  $debug = FALSE; // Set this to true to prevent CSV export file generation.
  // Apply filters, sorting; run the data export view:
  $drush_options = (array) json_decode(drush_get_option('json'));
  while ($current_page < $max_page) {
    $view = null;

    // Prepare the data export view for execution:
    $view = views_get_view($view_id);
    if (!isset($view) || $view == FALSE || !is_object($view)) {
      echo json_encode(array('error' => 'View ' . $view_id . ' could not be loaded; aborting.'));
      return;
    }
    $view->set_display($export_display_id);
    $view->init_handlers();    
    $view->set_items_per_page($items_per_page);
    $view->set_offset($current_page * $items_per_page);

    // Tell the query alter this query might need altering and provide the field map:
    $view->is_sb_export = TRUE;
    $view->drush_field_map = $drush_field_map;

    // Apply filtering and sorting:
    $view->drush_filters = array();
    $sort = 'asc';
    unset($order);
    foreach ($drush_options as $option_name => $option_value) {
      switch ($option_name) {
        case 'sort':
          $sort = $option_value;
          break;
        case 'order':
          $order = $option_value;
          break;
/*        case 'transaction_date':
          unset($min_date);
          unset($max_date);
          $expected_date_format = 'L, F j, Y';
          if (isset($option_value->min) && isset($option_value->max)) {
            $min_date = strtotime($option_value->min);
            $max_date = strtotime($option_value->max);
          }
          elseif (isset($option_value->min)) {
            $min_date = strtotime($option_value->min);
            $max_date = date("m/d/Y", time() + (60 * 60 * 24));
          }
          elseif (isset($option_value->max)) {
            $min_date = strtotime('1/1/1980');
            $max_date = strtotime($option_value->max);
          }
          if (isset($min_date) && isset($max_date)) {
            $view->exposed_input['date_filter']['min']['date'] = date($expected_date_format, $min_date);
            $view->exposed_input['date_filter']['max']['date'] = date($expected_date_format, $max_date);
          }
          break; */
        case 'debug':
          if ($option_value != 0) {
            $debug = TRUE;
          }
          break;
        default:
          if (!empty($option_value)) {
            if (isset($drush_field_map[$option_name])) {
              $view->drush_filters[$option_name] = $option_value;
            }
            else {
              echo json_encode(array('error' => 'Views Data Export failed due to unsupported filter: \'' . $option_name . '\'; aborting.'));
              return;
            }
          }
          break;
      }
    }

    // Apply the order and sort:
    if (isset($order)) {
      $view->drush_sort = $sort;
      $view->drush_order = $order;
    }

    // Fetch the data:
    $view->pre_execute();
    $view->execute();

    // Render the output:
    $output = _springboard_export_render_csv_page($view);

    // Output this page's results to a file:
    if (isset($view->result) && count($view->result) > 0) {
      $filename = $output_path . '/' . date('Y-m-d--H-i-s', $start) . '-' . $filename_descr . '-export-page-' . ($current_page + 1) . '.csv';
      $file_paths[] = $filename;
      $total_results += count($view->result);
      if (!$debug) {
        if (!file_put_contents($filename, $output)) {
          echo json_encode(array('error' => 'Export failed for file: \'' . $filename . '\'; aborting.'));
          return;
        }
      } 
      else {
        // Simulate rendering the output if in debug mode:
        //$view->render();
      }
    }
    else {
      break;
    }
    $view->destroy();
    echo "\n" . gc_collect_cycles() . " garbage cycles collected.\n\n";
    $current_page++;
  }

  // Upload each file to S3 and return the pre-signed S3 URL.
  $s3_pre_signed_urls = array();
  // $s3_pre_signed_urls = _springboard_views_drush_export_aws_files_upload(array($filename));

  $duration = (time() - $start) . 's';
  $export_summary = array(
    'total_results_exported' => $total_results,
    'view_id' => $view_id,
    'view_display' => $export_display_id,
    'start' => date('Y-m-d - H:i:s', $start),
    'end' => date('Y-m-d - H:i:s'),
    'duration' => $duration,
    'internal_file_pathis' => $file_paths,
    'export_file_count' => count($s3_pre_signed_urls),
    'export_file_paths' => $s3_pre_signed_urls,
  );

  if ($debug) {
    $export_summary['file_gen_disabled'] = 1;
    echo "\n\n";
    print_r($export_summary);
    echo "\n\n";
  }

  // Return the job summary.
  //return json_encode($export_summary);
  echo json_encode($export_summary);

  if ($debug) {
    echo "\n\n";
  }
}


/**
 * Render the CSV output for a given page:
 */
function _springboard_export_render_csv_page($view) {
  $output = '"unique_id","form_id","group_id","form_type","name","internal_name","url","created_at","updated_at","extra_props"' . "\n";
  foreach ($view->result as $result) {
    $node = $result->_field_data['node_fundraiser_donation_nid']['entity'];

    // Fetch the extra props:
    $extra_props = array();
    if (isset($result->_field_data['node_fundraiser_donation_nid']['entity']->webform['components'])) {
      $components = $result->_field_data['node_fundraiser_donation_nid']['entity']->webform['components'];
      foreach ($components as $component) {
        if (isset($component['value']) && !empty($component['value']) && isset($component['form_key'])) {
          $extra_props[$component['form_key']] = $component['value'];
        }
      }
    }

    $row_data = array(
      $result->commerce_order_fundraiser_donation_created . '-' . $node->nid . '-' . $result->did, // unique_id
      $node->nid, // form_id
      '', // group_id
      $node->type, // form_type
      $result->node_fundraiser_donation_title, // name
      $result->field_field_fundraiser_internal_name[0]['raw']['value'], // internal_name
      drupal_get_path_alias('node/' . $node->nid), // url
      date('n/j/Y H:i:s', $result->commerce_order_fundraiser_donation_created), // created_at
      date('n/j/Y H:i:s', $result->commerce_order_fundraiser_donation_changed), // updated_at
      json_encode($extra_props),  // extra_props
    );
    // Escape double quotes:
    foreach ($row_data as $key => $value) {
      $row_data[$key] = str_replace('"', '""', $value);
    } 
    $output .= '"' . implode('", "', $row_data) . "\n";
  }
  return $output;
}
