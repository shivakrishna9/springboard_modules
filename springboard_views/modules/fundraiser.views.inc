<?php
/**
 * @file
 * Provides views table data for fundraiser related tables.
 */

/**
 * Implements hook_views_data().
 */
function springboard_views_fundraiser_views_data() {
  $data = array();
  /**
   * Fundraiser table metadata.
   */
  $data['fundraiser']['table'] = array(
    'group' => t('Fundraiser donation forms'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
    ),
  );
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

  /**
   * Fundraiser_donation table metadata. Relationships to fundraiser node, and donationg user.
   */
  $data['fundraiser_donation']['table'] = array(
    'group' => t('Fundraiser donations'),
    'base' => array(
      'field' => 'did',
      'title' => t('Donations'),
      'help' => t('Donations made.'),
    ),
  );
  $data['fundraiser_donation']['nid'] = array(
    'title' => t('Fundraiser node'),
    'help' => t('Fundraiser node related to a donation.'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Fundraiser node'),
    ),
  );
  $data['fundraiser_donation']['uid'] = array(
    'title' => t('Donating user'),
    'help' => t('Donating user related to a donation.'),
    'relationship' => array(
      'base' => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Donating user'),
    ),
  );
  /*
  $data['fundraiser_donation']['sfid'] = array(
    'title' => t('Salesforce object'),
    'help' => t('Salesforce object related to a donation.'),
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
  */
  $data['fundraiser_donation']['gateway'] = array(
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
  $data['fundraiser_donation']['txn_id'] = array(
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

  /**
   * Fundraiser_tracking table metadata. Left joined to the charged back donation.
   */
  $data['fundraiser_refund']['table'] = array(
    'group' => t('Fundraiser refunds'),
    'join' => array(
      'donation' => array(
        'left_field' => 'did',
        'field' => 'refund_id',
      ),
    ),
  );
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
  $data['fundraiser_refund']['did'] = array(
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

  /**
   * Fundraiser_tracking table metadata. Left joined to the tracked fundraiser node.
   */
  $data['fundraiser_tracking']['table'] = array(
    'group' => t('Fundraiser tracking'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
    ),
  );
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

  return $data;
}
