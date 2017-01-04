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
  // Drush may have a low default memory_limit; change that to 1.5GB:
  ini_set('memory_limit', '1536M');

  // Grant full access so the view can be executed:
  $start = time();
  global $user;
  drupal_save_session(FALSE);
  $user = user_load(1);

  // Gather data needed to run the view:
  $output_path = variable_get('springboard_dw_reports_export_path', 'temporary://'); // Default output path if it is not overwritten.
  $current_page = 0;
  $total_results = 0;
  $file_paths = array();
  $items_per_page = variable_get('springboard_dw_reports_batch_count', 125000); // This determines the number of results per file.
  $max_page = 100; // To prevent an infinite loop.
  $debug = FALSE; // Set this to true to prevent CSV export file generation.

  // Apply filters, sorting; run the data export view:
  $drush_options = (array) json_decode(drush_get_option('json'));
  $dw_drupal_key_map = _springboard_export_drush_donations_export_dw_key_map();
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
    //$view->drush_filters = array();
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
        case 'recurs':
          if ($option_value == 'yes') {
            $view->exposed_input['master_did_op'] = 'not empty';
          }
          elseif ($option_value == 'no') {
            $view->exposed_input['master_did_op'] = 'empty';
          }
          break;
        case 'transaction_date':
          if (isset($option_value->min)) {
            $view->exposed_input['date_filter']['min']['date'] = date('Y-m-d', strtotime($option_value->min));
          }
          if (isset($option_value->max)) {
            $view->exposed_input['date_filter']['max']['date'] = date('Y-m-d', strtotime($option_value->max));
          }
          break;
        case 'next_charge_date':
          if (isset($option_value->min)) {
            $view->exposed_input['next_charge']['min']['date'] = date('Y-m-d', strtotime($option_value->min));
          }
          if (isset($option_value->max)) {
            $view->exposed_input['next_charge']['max']['date'] = date('Y-m-d', strtotime($option_value->max));
          }
          break;
        case 'debug':
          if ($option_value != 0) {
            $debug = TRUE;
          }
          break;
        default:
          if (!empty($option_value)) {
            if (isset($dw_drupal_key_map[$option_name])) {
              //echo '"' . $option_value . '"';
              $filter_key = $dw_drupal_key_map[$option_name];
              $view->exposed_input[$filter_key] = $option_value;
            }
            else {
              echo json_encode(array('error' => 'Bad filter \'' . $option_name . '\'; aborting'));
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
    }
    else {
      break;
    }
    $view->destroy();
    $current_page++;
  }

  // Upload each file to S3 and return the pre-signed S3 URL.
  $s3_pre_signed_urls = array();
  if (class_exists('Aws\S3\S3Client')) {
    $s3_pre_signed_urls = _springboard_views_drush_export_aws_files_upload($file_paths);
  }

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

    // Populate each column's value if available; otherwise, provide an empty value:
    $created_at = isset($result->commerce_order_fundraiser_donation_created) ? $result->commerce_order_fundraiser_donation_created : 0;
    $updated_at = isset($result->commerce_order_fundraiser_donation_changed) ? $result->commerce_order_fundraiser_donation_created : 0;
    $nid = isset($node->nid) ? $node->nid : '';
    $did = isset($result->did) ? $result->did : '';
    $form_type = isset($node->type) ? $node->type : '';
    $name = isset($result->node_fundraiser_donation_title) ? $result->node_fundraiser_donation_title : '';
    $internal_name = isset($result->field_field_fundraiser_internal_name[0]['raw']['value']) ?
      $result->field_field_fundraiser_internal_name[0]['raw']['value'] : '';

    $row_data = array(
      $created_at . '-' . $nid . '-' . $did, // unique_id
      $nid, // form_id
      '', // group_id
      $form_type, // form_type
      $name, // name
      $internal_name, // internal_name
      drupal_get_path_alias('node/' . $node->nid), // url
      $created_at > 0 ? date('n/j/Y H:i:s', $created_at) : '', // created_at
      $updated_at > 0 ? date('n/j/Y H:i:s', $updated_at) : '', // updated_at
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

/**
 * Helper function which returns an array that maps data warehouse field keys 
 * to the export donation view's fields used for filtering and sorting:
 */
function _springboard_export_drush_donations_export_dw_key_map() {
  return array(
    'status' => 'status',
    'internal_name' => 'field_fundraiser_internal_name_value',
    'transaction_date' => 'date_filter',
    'next_charge_date' => 'next_charge',
  );
}
