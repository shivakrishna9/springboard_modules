<?php

/**
 * Implements hook_drush_command().
 */
function commerce_sage_payments_drush_command() {

  $items['sage-tokenizer'] = array(
    'description' => 'Obtain sage payment tokens using a source file',
    'arguments' => array(
      'path' => 'path to the source file',
    ),
  );

  return $items;
}

function drush_commerce_sage_payments_sage_tokenizer($path){

  $gateway = _fundraiser_gateway_info('commerce_sage_payments_eft|commerce_payment_commerce_sage_payments_eft');

  // Need to batch, see webform_user_create_missing_users_batch_process();

  $accounts = commerce_sage_payments_load_accounts_from_csv($path);
  $results = [];
  foreach ($accounts as $index => $account) {

    $payment_details = array(
      'ROUTING_NUMBER' => $account['routing_number'],
      'ACCOUNT_NUMBER' => $account['account_number'],
      'C_ACCT_TYPE' => $account['account_type'],
    );

    $results[] = commerce_sage_payments_vault_request_insert($gateway['gateway_details'], $payment_details);
  }
}

function commerce_sage_payments_load_accounts_from_csv($path) {
  $file = file($path);
  $accounts = [];
  if (!empty($file)) {
    $accounts = array_map('str_getcsv', $file);
    array_walk($accounts, function (&$a) use ($accounts) {
      $a = array_combine($accounts[0], $a);
    });
    array_shift($accounts);
  }
  return $accounts;
}
