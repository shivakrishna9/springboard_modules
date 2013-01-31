<?php

/**
 * @file
 */
/**
 * Implementation of hook_views_data().
 */
function springboard_views_fundraiser_views_data() {
  $data = array();

  // Fundraiser table metadata
  $data['fundraiser']['table'] = array(
    'group' => t('Fundraiser donation forms'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
    ),
  );

  // Fundraiser fields
  $data['fundraiser']['gateway'] = array(
    'title' => t('Gateway'),
    'help' => t('The payment gateway the donation form uses to process credit cards.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['receipt_email_from'] = array(
    'title' => t('Receipt email from'),
    'help' => t('The name that will appear in the from line in the receipt email.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['receipt_email_address'] = array(
    'title' => t('Receipt email address'),
    'help' => t('The from email address that will be used in the receipt email.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['receipt_email_subject'] = array(
    'title' => t('Receipt email subject'),
    'help' => t('The subject line of the receipt email.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['receipt_email_message'] = array(
    'title' => t('Receipt email message'),
    'help' => t('The message that will appear in the receipt email.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['donation_amounts'] = array(
    'title' => t('Donation amounts'),
    'help' => t('Ask amounts to be displayed on the donation form.'),
    'field' => array(
      'handler' => 'fundraiser_handler_field_donation_amounts',
    ),
  );
  $data['fundraiser']['show_other_amount'] = array(
    'title' => t('Show other amount'),
    'help' => t('True or false, whether the donation form allows other amounts.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['minimum_donation_amount'] = array(
    'title' => t('Minimum donation amount'),
    'help' => t('The minimum donation amount the donation form will accept.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['internal_name'] = array(
    'title' => t('Internal name'),
    'help' => t('Internal name of the donation form.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['confirmation_page_title'] = array(
    'title' => t('Confirmation page title'),
    'help' => t('The page title of the confirmation page.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['confirmation_page_body'] = array(
    'title' => t('Confirmation page body'),
    'help' => t('The body text of the confirmation page.'),
    'field' => array(
      'handler' => 'views_handler_field_markup',
      'format' => 'confirmation_page_format', // The name of the format field
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser']['redirect_url'] = array(
    'title' => t('Redirect URL'),
    'help' => t('Redirect URL for donation form submissions.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // --------------------------------------------------------------------------
  // fundraiser_webform_order table metadata
  $data['fundraiser_webform_order']['table'] = array(
    'group' => t('Fundraiser webform orders'),
    'join' => array(
      'webform_submissions' => array(
        'left_field' => 'sid',
        'field' => 'sid',
      ),
      'uc_orders' => array(
        'left_field' => 'order_id',
        'field' => 'order_id',
      ),
    ),
  );

  // fundraiser_webform_order fields
  $data['fundraiser_webform_order']['webform_nid'] = array(
    'title' => t('Fundraiser webform'),
    'help' => t('Webform related to a fundraiser webform submission.'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Fundraiser webform'),
    ),
  );
  $data['fundraiser_webform_order']['order_id'] = array(
    'title' => t('Fundraiser order'),
    'help' => t('Ubercart order related to a fundraiser webform submission.'),
    'relationship' => array(
      'base' => 'uc_orders',
      'base field' => 'order_id',
      'handler' => 'views_handler_relationship',
      'label' => t('Fundraiser order'),
    ),
  );
  $data['fundraiser_webform_order']['sfid'] = array(
    'title' => t('Salesforce object'),
    'help' => t('Salesforce object related to a fundraiser webform submission.'),
    'relationship' => array(
      'base' => 'salesforce_management_object_map',
      'base field' => 'sfid',
      'field' => 'sfid',
      'extra' => array(
        array(
          'field' => 'drupal_type',
          'value' => 'donation',
        ),
      ),
      'handler' => 'views_handler_relationship',
      'label' => t('Salesforce object'),
    ),
  );
  $data['fundraiser_webform_order']['gateway'] = array(
    'title' => t('Gateway'),
    'help' => t('The payment gateway used on this donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_webform_order']['txn_id'] = array(
    'title' => t('Transaction ID'),
    'help' => t('The transaction ID used on this donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_webform_order']['recurring_status'] = array(
    'title' => t('Recurring status'),
    'help' => t('True or false, whether the donation is recurring.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // --------------------------------------------------------------------------
  // Fundraiser_refund table metadata
  $data['fundraiser_refund']['table'] = array(
    'group' => t('Fundraiser refunds'),
    'join' => array(
      'uc_orders' => array(
        'left_field' => 'order_id',
        'field' => 'order_id',
      ),
    ),
  );

  // Fundraiser_refund fields
  $data['fundraiser_refund']['refund_id'] = array(
    'title' => t('Refund ID'),
    'help' => t('References a single refund.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_refund']['receipt_id'] = array(
    'title' => t('Receipt ID'),
    'help' => t('References a single refund receipt.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_refund']['amount'] = array(
    'title' => t('Refund amount'),
    'help' => t('Amount of a single refund.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_refund']['txn_id'] = array(
    'title' => t('Refund transaction ID'),
    'help' => t('The transaction ID of a single refund.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_refund']['reason'] = array(
    'title' => t('Refund reason'),
    'help' => t('The reason for a refund.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // --------------------------------------------------------------------------
  // Fundraiser_recurring table metadata
  $data['fundraiser_recurring']['table'] = array(
    'group' => t('Fundraiser recurring donations'),
    'join' => array(
      'uc_orders' => array(
        'left_field' => 'order_id',
        'field' => 'order_id',
      ),
    ),
  );

  // Fundraiser_recurring fields
  $data['fundraiser_recurring']['master_order_id'] = array(
    'title' => t('Master order'),
    'help' => t('Master Ubercart order for this recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_recurring']['order_id'] = array(
    'title' => t('Recurring order'),
    'help' => t('A single recurring donation Ubercart order.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Order is recurring'),
      'accept null' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_recurring']['next_charge'] = array(
    'title' => t('Date of next charge'),
    'help' => t('Date of the next recurring charge.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
  );
  //$data['fundraiser_recurring']['sf_opportunity_id'] = array();
  //$data['fundraiser_recurring']['sf_recurring_id'] = array();
  //$data['fundraiser_recurring']['sf_stagename'] = array();
  //$data['fundraiser_recurring']['sf_sync_status'] = array();
  $data['fundraiser_recurring']['gateway_resp'] = array(
    'title' => t('Gateway response'),
    'help' => t('Response from the payment gateway for a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_in_operator',
      'options callback' => 'springboard_views_gateway_response_options',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_recurring']['attempts'] = array(
    'title' => t('Attempts'),
    'help' => t('Number of attempted charges for a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_recurring']['cancellation_reason'] = array(
    'title' => t('Cancellation reason'),
    'help' => t('The reason provided for the cancellation of a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_recurring']['gateway'] = array(
    'title' => t('Gateway'),
    'help' => t('The payment gateway used on a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_recurring']['txn_id'] = array(
    'title' => t('Transaction ID'),
    'help' => t('The transaction ID used on a recurring donation.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // --------------------------------------------------------------------------
  // Fundraiser_component_map table metadata
  // TODO

  // --------------------------------------------------------------------------
  // Fundraiser_tracking table metadata
  $data['fundraiser_tracking']['table'] = array(
    'group' => t('Fundraiser tracking'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
    ),
  );

  // Fundraiser_tracking fields
  $data['fundraiser_tracking']['pageviews'] = array(
    'title' => t('Page views'),
    'help' => t('Number of times the donation form has been viewed.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_tracking']['conversions'] = array(
    'title' => t('Conversions'),
    'help' => t('Number of times the donation form has been submitted successfully.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_tracking']['local_failures'] = array(
    'title' => t('Local failures'),
    'help' => t('Number of times form submission has resulted in form_set_error().'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['fundraiser_tracking']['gateway_failures'] = array(
    'title' => t('Gateway failures'),
    'help' => t('Number of times submission has resulted in a failure reported by the payment gateway.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // --------------------------------------------------------------------------
  return $data;
}
