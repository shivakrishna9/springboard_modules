<?php

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_paywithmybank_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('paywithmybank'),
    'offsite_processing' => array('paywithmybank'),
    'form callback' => '',
    'refund callback' => '',
  );
}
