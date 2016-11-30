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
      'callback' => 'springboard_views_drush_export_donations_csv',
      'options' => array(
        'json' => t('JSON any key/value search filters, sort (the field on which to sort), and order (asc or desc).'),
      ),
      'drupal dependencies' => array(
        'views',
        'commerce',
        'commerce_order',
        'webform',
        'fundraiser_webform',
        'fundraiser_commerce',
        'fundraiser',
        'springboard_views'
      ),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    ),
  );
}

/**
 * Callback for sb-export-donations-csv.
 */
function springboard_views_drush_export_donations_csv() {
  // Grant full access so the view can be executed:
  $file_paths = array();
  $start = time();
  global $user;
  drupal_save_session(FALSE);
  $user = user_load(1);
  $output_path = variable_get('springboard_dw_reports_export_path', 'temporary://'); // Default output path if it is not overwritten.
  $current_page = 0;
  $total_results = 0;
  $items_per_page = variable_get('springboard_dw_reports_batch_count', 125000); // This determines the number of results per file.
  $max_page = 100; // Primarily for testing; secondarily to avoid an infinite loop.
  $debug = FALSE; // Set this to true to prevent CSV export file generation.

  // Export each page of results into its own CSV file:
  $drush_options = (array) json_decode(drush_get_option('json'));
  $dw_drupal_key_map = _springboard_views_drush_export_donations_dw_key_map();
  $dw_order_key_map = _springboard_views_drush_export_donations_dw_query_key_map();
  if ($debug) {
    echo "\n\n" . print_r($drush_options, 1) . "\n\n";
  }
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
    $view->set_items_per_page($items_per_page);

    $view->set_offset($current_page * $items_per_page);

    // Apply filtering, sorting, and ordering:
    $view->drush_filters = array();
    $sort = 'asc';
    unset($order_by);
    foreach ($drush_options as $option_name => $option_value) {
      // Collect sorting data:
      if ($option_name == 'order') {
        if (isset($dw_order_key_map[$option_value])) {
          $order_by = $dw_order_key_map[$option_value];
        }
        else {
          echo json_encode(array('error' => 'Bad order \'' . $option_value . '\'; aborting'));
          return;
        }
        continue;
      }
      // Collect ordering data:
      elseif ($option_name == 'sort') {
        $sort = strtolower($option_value);
        if ($sort != 'desc' && $sort != 'asc') {
          $sort = 'asc';
        }
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
      elseif ($option_name == 'transaction_date') {
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
        continue;
      }

      // Apply filters:
      if (isset($dw_drupal_key_map[$option_name])) {
        if ($debug) {
          echo '"' . $option_value . '"';
        }
        $filter_key = $dw_drupal_key_map[$option_name];
        $view->drush_filters[$filter_key] = $option_value;
      }
      else {
        echo json_encode(array('error' => 'Bad filter \'' . $option_name . '\'; aborting'));
        return;
      }
    }

    // Apply sort and order, if specified; these are applied via query alter:
    if (isset($order_by)) {
      $view->drush_sort = $sort;
      $view->drush_order = $order_by;
    }

    if ($debug) {
     // echo "\n\n" . print_r($view->exposed_input, 1) . "\n\n";
    }
    $view->pre_execute();
    $view->execute();

    // Save the output to a file, if results were returned; otherwise stop:

    $output_path = '/tmp';

    if (isset($view->result) && count($view->result) > 0) {
      $output = $view->render();
      $filename = $output_path . '/' . date('Y-m-d--H-i-s') . '-donations-export-page-' . ($current_page + 1) . '.csv';
      $file_paths[] = $filename;
      $total_results += count($view->result);
      if (!$debug) {
        if (!file_put_contents($filename, $output)) {
          echo json_encode(array('error' => 'Export failed for file: "' . $filename . '"; aborting.'));
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

/**
 * Helper function which returns an array that maps data warehouse field keys
 * to the export donation view's fields used for filtering and sorting:
 */
function _springboard_views_drush_export_donations_dw_key_map() {
  return array(
    'address'          => 'sbp_address_value',
    'address_line_2'   => 'sbp_address_line_2_value',
    'amount'           => 'amount',
    'city'             => 'sbp_city_value',
    'contact_id'       => 'sbp_salesforce_contact_id_value',
    'country'          => 'sbp_country_value',
    'currency'         => 'currency',
    'created_at'       => 'created',
    'device_browser'   => 'sbp_device_browser_value',
    'device_name'      => 'sbp_device_name_value',
    'device_os'        => 'sbp_device_os_value',
    'device_type'      => 'sbp_device_type_value',
    'email'            => 'mail',
    'first_name'       => 'sbp_first_name_value',
    'last_name'        => 'sbp_last_name_value',
    'form_id'          => 'nid',
    'gateway'          => 'gateway',
    'internal_name'    => 'field_fundraiser_internal_name_value',
    'ip_address'       => 'hostname',
    'order_id'         => 'order_id',
    'referrer'         => 'sbp_referrer_value',
    'initial_referrer' => 'sbp_initial_referrer_value',
    'search_engine'    => 'sbp_search_engine_value',
    'search_string'    => 'sbp_search_string_value',
    'state'            => 'sbp_state_value',
    'status'           => 'status',
    'submission_id'    => 'sid',
    'transaction_id'   => 'txn_id',
    'transaction_date' => 'transaction_date',
    'updated_at'       => 'changed',
    'user_agent'       => 'sbp_user_agent_value',
    'zip'              => 'sbp_zip_value',
  );
}

/**
 * Helper function which returns an array that maps data warehouse field keys
 * to the export donation view's view-specific field using for query-based filtering/sorting:
 */
function _springboard_views_drush_export_donations_dw_query_key_map() {
  return array(
    'address'          => 'users_fundraiser_donation__field_data_sbp_address.sbp_address_value',
    'address_line_2'   => 'users_fundraiser_donation__field_data_sbp_address_line_2.sbp_address_line_2_value',
    'amount'           => 'fundraiser_donation.amount',
    'city'             => 'users_fundraiser_donation__field_data_sbp_city.sbp_city_value',
    'contact_id'       => 'sbp_salesforce_contact_id_value',
    'country'          => 'users_fundraiser_donation__field_data_sbp_country.sbp_country_value',
    'currency'         => 'fundraiser_donation.currency',
    'created_at'       => 'fundraiser_donation.created',
    'device_browser'   => 'users_fundraiser_donation__field_data_sbp_device_browser.sbp_device_browser_value',
    'device_name'      => 'users_fundraiser_donation__field_data_sbp_device_name.sbp_device_name_value',
    'device_os'        => 'users_fundraiser_donation__field_data_sbp_device_os.sbp_device_os_value',
    'device_type'      => 'users_fundraiser_donation__field_data_sbp_device_type.sbp_device_type_value',
    'email'            => 'commerce_order_fundraiser_donation.mail',
    'first_name'       => 'users_fundraiser_donation__field_data_sbp_first_name.sbp_first_name_value',
    'last_name'        => 'users_fundraiser_donation__field_data_sbp_last_name.sbp_last_name_value',
    'form_id'          => 'fundraiser_donation.nid',
    'gateway'          => 'fundraiser_donation.gateway',
    'internal_name'    => 'node_fundraiser_donation__field_data_field_fundraiser_internal_name.field_fundraiser_internal_name_value',
    'ip_address'       => 'commerce_order_fundraiser_donation.hostname',
    'order_id'         => 'commerce_order_fundraiser_donation.order_id',
    'referrer'         => 'users_fundraiser_donation__field_data_sbp_referrer.sbp_referrer_value',
    'initial_referrer' => 'users_fundraiser_donation__field_data_sbp_initial_referrer.sbp_initial_referrer_value',
    'search_engine'    => 'users_fundraiser_donation__field_data_sbp_search_engine.sbp_search_engine_value',
    'search_string'    => 'users_fundraiser_donation__field_data_sbp_search_string.sbp_search_string_value',
    'state'            => 'users_fundraiser_donation__field_data_sbp_state.sbp_state_value',
    'status'           => 'fundraiser_donation.status',
    'submission_id'    => 'webform_submissions_fundraiser_donation.sid',
    'transaction_id'   => 'fundraiser_donation.txn_id',
    'transaction_date' => 'fundraiser_donation.created',
    'updated_at'       => 'fundraiser_donation.changed',
    'user_agent'       => 'users_fundraiser_donation__field_data_sbp_user_agent.sbp_user_agent_value',
    'zip'              => 'users_fundraiser_donation__field_data_sbp_zip.sbp_zip_value',
  );
}


/**
 * AWS code:
 */
function _springboard_views_drush_export_aws_files_upload($file_paths) {
  // Composer Vendor autoload.
  if (function_exists('drush_autoload')) {
    drush_autoload("sites/all/libraries/springboard_composer/vendor/autoload.php");
  }
  else {
    include_once "sites/all/libraries/springboard_composer/vendor/autoload.php";
  }

  // Enable Garbage Collection.
  gc_enable();

  // Setup the S3 config.
  $s3_config = array(
    'region'      => variable_get
    ('springboard_dw_reports_export_S3_region', NULL),
    'version'     => '2006-03-01',
    'credentials' => array(
      'key'    => variable_get
      ('springboard_dw_reports_export_S3_key', NULL),
      'secret' => variable_get
      ('springboard_dw_reports_export_S3_secret', NULL),
    )
  );

  // Create an S3 client instance.
  $s3_client = new \Aws\S3\S3Client($s3_config);

  // Register the stream wrapper.
  $s3_client->registerStreamWrapper();

  // Create new array shells to track the S3 uploads.
  $s3_uploaded_files = array();
  $s3_failed_files = array();
  $s3_pre_signed_urls = array();

  // Upload each file to S3 and return the pre-signed S3 URL.
  foreach ($file_paths as $key => $path) {
    $local_file_to_upload = $path;
    $new_s3_file_key = end(explode('/', $path));

    // Try the S3 upload, if it fails track the failure.
    try {
      // Instantiate a multi-part uploader.
      $uploader = new \Aws\S3\MultipartUploader($s3_client, $local_file_to_upload, [
        'bucket' => variable_get('springboard_dw_reports_export_S3_bucket', NULL),
        'key'    => $new_s3_file_key,
      ]);

      // Do the S3 upload.
      $result = $uploader->upload();

      // Debug the completed upload.
      _springboard_views_drush_export_guard_debug_output($debug, "Upload complete: {$result['ObjectURL']}\n");

      // Track the upload success.
      $s3_uploaded_files[] = $path;

      // Add the GetObject client method so we can retrieve a pre-signed request.
      $cmd = $s3_client->getCommand('GetObject', [
        'Bucket' => variable_get
        ('springboard_dw_reports_export_S3_bucket', NULL),
        'Key'    => $new_s3_file_key,
      ]);

      // Create the pre-signed request and get the url.
      $request = $s3_client->createPresignedRequest($cmd, '+180 minutes');
      $presignedUrl = (string) $request->getUri();

      // Add the pre-signed url to the array to add back to the export summary.
      $s3_pre_signed_urls[] = $presignedUrl;

    } catch (Aws\Exception\MultipartUploadException $e) {
      // Track the upload failure.
      $s3_failed_files[] = $path;

      // Build out the error details to log to watchdog.
      $error_details = array(
        'exception' => $e->getMessage(),
        'file'      => $local_file_to_upload,
      );

      // Construct the watchdog string.
      $watchdog_string = "<pre>Springboard reports S3 upload error: \n" . print_r($error_details, TRUE) . "</pre>";

      watchdog_exception('Drupal reports export', $watchdog_string);
    }
  }

  // Merge the arrays of uploaded and failed because we will want to remove
  // the local CSV export for both since it will change the next attempt.
  $all_files = array_merge($s3_uploaded_files, $s3_failed_files);

  // Remove the local file.
  foreach ($all_files as $local_file) {
    unset($local_file);
  }

  return $s3_pre_signed_urls;
}



