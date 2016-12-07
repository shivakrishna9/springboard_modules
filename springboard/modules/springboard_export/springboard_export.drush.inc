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
 * Callback for drush sb-easc
 */
function springboard_export_drush_export_action_submissions_csv() {
  _springboard_export_drush_export_action_data_csv('views_data_export_1', 'sba_action_submissions');
}

/**
 * Fetch and generate advocacy CSV output.
 *
 * @param $export_display_id
 *   Denotes the views data export display to be used for fetching and rendering csv file content.
 *
 * @param $view_id
 *   View from which data is queried.
 */
function _springboard_export_drush_export_action_data_csv($export_display_id, $view_id) {
  // Grant full access so the view can be executed:
  $file_paths = array();
  $start = time();
  global $user;
  drupal_save_session(FALSE);
  $user = user_load(1);
  
  //$output_path = variable_get('springboard_dw_reports_export_path', 'temporary://'); // Default output path if it is not overwritten.
  $output_path = 'temporary://';
  $current_page = 0;
  $total_results = 0;
  $items_per_page = variable_get('springboard_dw_reports_batch_count', 125000); // This determines the number of results per file.
  $max_page = 100; // Primarily for testing; secondarily to avoid an infinite loop.
  $debug = FALSE; // Set this to true to prevent CSV export file generation.

  // Apply filters, sorting; run the data export view:
  $dw_field_map = _springboard_export_action_dw_to_drupal_field_map();
  $drush_options = (array) json_decode(drush_get_option('json'));
  while ($current_page < $max_page) {
    $view = views_get_view($view_id);
    if (!isset($view) || $view == FALSE || !is_object($view)) {
      echo json_encode(array('error' => 'View ' . $view_id . ' could not be loaded; aborting.'));
      return;
    }
    $view->set_display($export_display_id);
    $view->init_handlers();    

    // Set the appropriate offset; if items per page is missing then assume 25K/page:
    $view->set_items_per_page($items_per_page);
    $view->set_offset($current_page * $items_per_page);

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
            if (isset($dw_field_map[$option_name])) {
              $view->drush_filters[$option_name] = $option_value;
            }
            else {
              echo json_encode(array('error' => 'Export failed due to unsupported filter: \'' . $option_name . '\'; aborting.'));
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

    // Output this page's results to a file:
    if (isset($view->result) && count($view->result) > 0) {
      $output = $view->render();
      $filename = $output_path . '/' . date('Y-m-d--H-i-s') . '-actions-submissions-export-page-' . ($current_page + 1) . '.csv';
      $file_paths[] = $filename;
      $total_results += count($view->result);
      if (!$debug) {
        if (!file_put_contents($filename, $output)) {
          echo json_encode(array('error' => 'Export failed for file: \'' . $filename . '\'; aborting.'));
          return;
        }
      }
    }
    else {
      break;
    }

    $current_page++;
  }

  // Upload each file to S3 and return the pre-signed S3 URL.
  $s3_pre_signed_urls = array();
  // $s3_pre_signed_urls = _springboard_views_drush_export_aws_files_upload($file_paths)

  $duration = (time() - $start) . 's';
  $export_summary = array(
    'start' => date('Y-m-d - H:i:s', $start),
    'end' => date('Y-m-d - H:i:s'),
    'duration' => $duration,
    'total_results_exported' => $total_results,
    'internal_file_paths' => $file_paths,
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
