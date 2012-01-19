<?php

/**
 * @file
 * Action and trigger related stuff for the fundraiser module.
 * These functions are split off with the idea that eventually this could become it's own module, linking fundraiser and triggers.
 */

/**
 * Implementation of hook_hook_info().
 */
function fundraiser_hook_info() {
  return array(
    'fundraiser' => array(
      'fundraiser' => array(
        'sustainer_payment_declined' => array(
          'runs when' => t('A sustainer\'s credit card payment is declined'),
        ),
      ),
    ),
  );
}

/**
 * Implementation of hook_fundraiser().
 */
function fundraiser_fundraiser($op, $order) {
  if (module_exists('trigger')) {
    $aids = _trigger_get_hook_aids('fundraiser', $op);
    $context = array(
      'hook' => 'fundraiser',
      'op' => $op,
      'order' => $order,
    );
    $dummy = new stdClass();
    actions_do(array_keys($aids), $dummy, $context);
  }
}

/**
 * Implementation of hook_action_info_alter().
 */
function fundraiser_action_info_alter(&$info) {
  if (isset($info['token_actions_send_email_action']['hooks']['fundraiser'])) {
    array_merge($info['token_actions_send_email_action']['hooks']['fundraiser'], array('sustainer_payment_declined'));
  }
  else {
    $info['token_actions_send_email_action']['hooks']['fundraiser'] = array('sustainer_payment_declined');
  }
}