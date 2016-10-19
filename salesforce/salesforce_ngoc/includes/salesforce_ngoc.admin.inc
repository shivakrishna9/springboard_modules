<?php

/**
 * @file
 * Administration page callbacks for the salesforce nonprofit starter pack module.
 */

/**
 * Page callback for the admin form.
 */
function salesforce_ngoc_admin_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'salesforce-ngoc-admin-helptext-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for NGO Connect.') . '</p>',
      '#prefix' => '<div class="ngoc-admin-helptext">',
      '#suffix' => '</div>',
    ),
    'salesforce_ngoc_admin_form' => drupal_get_form('salesforce_ngoc_admin_form'),
  );
  return $build;
}

/**
 * Configuration form for ngoc admin options.
 */
function salesforce_ngoc_admin_form() {
  $form = array();
  $sfapi = salesforce_get_api();
  $batch_upload_info = $sfapi->objectDescribe('rC_Connect__Batch_Upload__c');
  $form['salesforce_ngoc'] = array(
    '#type' => 'fieldset',
    '#title' => t('Batch Upload system fields'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => 'The batch upload object needs some system flags set. We set them here instead of implementing in the mapping interface because these flags have no donation counterpart.',
  );
  $form['salesforce_ngoc']['salesforce_ngoc_default_email_type'] = array(
    '#title' => t('Contact 1: Email Type'),
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => salesforce_picklist_values($batch_upload_info, 'rC_Connect__Contact_1_Email_Type__c'),
    '#default_value' => variable_get('salesforce_ngoc_default_email_type', SALESFORCE_NGOC_DEFAULT_EMAIL_TYPE),
  );
  $form['salesforce_ngoc']['salesforce_ngoc_default_address_type'] = array(
    '#title' => t('Address: Type'),
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => salesforce_picklist_values($batch_upload_info, 'rC_Connect__Address_Type__c'),
    '#default_value' => variable_get('salesforce_ngoc_default_address_type', SALESFORCE_NGOC_DEFAULT_ADDRESS_TYPE),
  );
  // @TODO This should probably be set dynamically, based on whether or not the
  // synced record is a payment or transaction. From the SF help text: "Provided
  // by external system, Designates Transaction as a Payment from constituent or
  // Refund to constituent."
  $form['salesforce_ngoc']['salesforce_ngoc_default_transaction_type'] = array(
    '#title' => t('Giving: Transaction Type'),
    '#type' => 'textfield',
    '#description' => '@TODO This should probably be set dynamically',
    '#default_value' => variable_get('salesforce_ngoc_default_transaction_type', SALESFORCE_NGOC_DEFAULT_TRANSACTION_TYPE),
  );
  // @TODO this should probably be set dynamically based on one-time or recur?
  $form['salesforce_ngoc']['salesforce_ngoc_default_frequency'] = array(
    '#title' => t('Giving: Payment Frequency'),
    '#type' => 'select',
    '#multiple' => FALSE,
    '#description' => '@TODO This should probably be set dynamically',
    '#options' => salesforce_picklist_values($batch_upload_info, 'rC_Connect__Giving_Payment_Frequency__c'),
    '#default_value' => variable_get('salesforce_ngoc_default_frequency', SALESFORCE_NGOC_DEFAULT_FREQUENCY),
  );
  $form['salesforce_ngoc']['salesforce_ngoc_default_payment_method_type'] = array(
    '#title' => t('Payment Method: Payment Type'),
    '#type' => 'select',
    '#multiple' => FALSE,
    '#options' => salesforce_picklist_values($batch_upload_info, 'rC_Connect__Payment_Method_Payment_Type__c'),
    '#default_value' => variable_get('salesforce_ngoc_default_payment_method_type', SALESFORCE_NGOC_DEFAULT_PAYMENT_METHOD_TYPE),
  );
  $form['salesforce_ngoc']['salesforce_ngoc_default_record_type'] = array(
    '#title' => t('Giving: Record Type'),
    '#type' => 'textfield',
    '#default_value' => variable_get('salesforce_ngoc_default_record_type', SALESFORCE_NGOC_DEFAULT_RECORD_TYPE),
  );
  return system_settings_form($form);
}

/**
 * Array of values from a Salesforce picklist field.
 *
 * Suitable for use as #options in a select field.
 *
 * @param array $object_info
 *   Data returned from Salesforce objectDescribe API call.
 *
 * @param  string $field_name
 *   Name of the field to return picklist values.
 *
 * @return array
 *   Assoc array of options.
 */
function salesforce_picklist_values($object_info, $field_name) {
  $options = array();
  if (isset($object_info['fields'])) {
    foreach ($object_info['fields'] as $field) {
      if (isset($field['name']) && $field['name'] == $field_name && isset($field['picklistValues'])) {
        foreach ($field['picklistValues'] as $pick) {
          $options[$pick['value']] = $pick['value'];
        }
      }
    }
  }
  return $options;
}