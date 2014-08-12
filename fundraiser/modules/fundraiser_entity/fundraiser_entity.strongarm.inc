<?php
/**
 * @file
 * fundraiser_entity.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function fundraiser_entity_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'fundraiser_entity_default_map';
  $strongarm->value = array(
    'amount' => array(
      'source' => 'commerce_order--commerce_order_total:amount_decimal',
      'other' => '',
    ),
    'currency' => array(
      'source' => 'commerce_order--commerce_order_total:currency_code',
      'other' => '',
    ),
    'gateway' => array(
      'source' => 'commerce_payment_transaction--payment_method',
      'other' => '',
    ),
    'txn_id' => array(
      'source' => 'commerce_payment_transaction--remote_id',
      'other' => '',
    ),
    'form_url' => array(
      'source' => 'node--url',
      'other' => '',
    ),
    'field_fde_donor_address:country' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:country',
      'other' => '',
    ),
    'field_fde_donor_address:name_line' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:name_line',
      'other' => '',
    ),
    'field_fde_donor_address:first_name' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:first_name',
      'other' => '',
    ),
    'field_fde_donor_address:last_name' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:last_name',
      'other' => '',
    ),
    'field_fde_donor_address:organisation_name' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:organisation_name',
      'other' => '',
    ),
    'field_fde_donor_address:administrative_area' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:administrative_area',
      'other' => '',
    ),
    'field_fde_donor_address:sub_administrative_area' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:sub_administrative_area',
      'other' => '',
    ),
    'field_fde_donor_address:locality' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:locality',
      'other' => '',
    ),
    'field_fde_donor_address:dependent_locality' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:dependent_locality',
      'other' => '',
    ),
    'field_fde_donor_address:postal_code' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:postal_code',
      'other' => '',
    ),
    'field_fde_donor_address:thoroughfare' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:thoroughfare',
      'other' => '',
    ),
    'field_fde_donor_address:premise' => array(
      'source' => 'commerce_customer_profile--commerce_customer_address:premise',
      'other' => '',
    ),
    'field_fde_eft_routing_number' => array(
      'source' => 'other',
      'other' => '[donation:bank_routing]',
    ),
    'field_fde_eft_account_number' => array(
      'source' => 'other',
      'other' => '[donation:bank_account]',
    ),
    'field_fde_cc_number' => array(
      'source' => 'other',
      'other' => '[donation:card_number]',
    ),
    'field_fde_cc_type' => array(
      'source' => 'other',
      'other' => '[donation:card_type]',
    ),
    'field_fde_cc_exp_year' => array(
      'source' => 'other',
      'other' => '[donation:card_expiration_year]',
    ),
    'field_fde_cc_exp_month' => array(
      'source' => 'other',
      'other' => '[donation:card_expiration_month]',
    ),
    'field_fde_payment_method' => array(
      'source' => 'other',
      'other' => '[donation:payment_method]',
    ),
  );
  $export['fundraiser_entity_default_map'] = $strongarm;

  return $export;
}
