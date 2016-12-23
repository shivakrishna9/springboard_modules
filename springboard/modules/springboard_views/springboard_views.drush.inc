<?php

/**
 * Implements hook_drush_command().
 *
 * Adds a drush command which saves a CSV file containing donation data from the Drupal DB.
 */
function springboard_views_drush_command() {
  return array();
/*  return array(
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
  ); */
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
    'donation_id'      => 'did',
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
    'donation_id'      => 'fundraiser_donation.did',
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



