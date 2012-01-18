<?php

/**
 * @file
 * Theme related functions for the fundraiser module
 */

/**
 * Implementation of hook_theme().
 */
function fundraiser_theme($existing, $type, $theme, $path) {
  return array(
    'fundraiser_mapping_form' => array(
      'variables' => array(),
    ),
    'sustainer_management_page' => array(
      'variables' => array('order_id' => NULL, 'orders' => NULL, 'payment_info' => NULL, 
        'billing_info' => NULL, 'amount_form' => NULL, 'billing_form' => NULL, 
        'cancel_form' => NULL, 'payment_schedule' => NULL, 'cancelled' => FALSE),
      'template' => 'templates/sustainer-management-page',
    ),
    'fundraiser_credit_card_expiration_date' => array(
      'variables' => array(),
    ),
    'fundraiser_confirmation' => array(
      'variables' => array('node' => NULL, 'sid' => NULL),
      'template' => 'templates/fundraiser-confirmation',
      'pattern' => 'fundraiser_confirmation_[0-9]+',
    ),
    'fundraiser_confirmation_tokens' => array(
      'variables' => array('tokens' => NULL),
      'template' => 'templates/fundraiser-token-list',
    ),
  );
}

/**
 * Implements hook_theme_registry_alter().
 */
function fundraiser_theme_registry_alter(&$theme_registry) {
  // Tell the theme registry to look in this module's directory for theme files.
  $theme_registry['page']['theme path'] = drupal_get_path('module', 'fundraiser') . '/templates';
}

/**
 * Implements hook_preprocess_page().
 * Add a template suggestion that renders a blank page template for our confirmation page token picker.
 */
function fundraiser_preprocess_page(&$variables) {
  if (arg(2) == 'confirmation') {
    // Add the node to $variables.
    $variables['node'] = node_load(arg(1));
  }
}


/**
 * Theme function for the sustainer management page.
 *
 * @param $vars
 *   The variables passed into the theme function
 */
function fundraiser_preprocess_sustainer_management_page(&$vars) {
  global $user;
  $profile_uid = arg(1);
  // convert the array of orders to a nice data table
  $header = array('Amount', 'Start Date', 'Next Charge Date', 'Expire Date', 'Status');
  $rows = array();

  // format the dates
  $class = '';
  foreach ($vars['orders'] as $id => &$order) {
    $edit_link = l('$' . money_format('%i', $order['amount']), 'user/' . $profile_uid . '/recurring_overview/' . $id);
    if ($id == $vars['order_id']) {
      $class = 'recurring-order active';
    }
    else {
      $class = 'recurring-order';
    }
    $rows[] = array(
      'data' => array(
        'amount' => $edit_link,
        'start_date' => date('m/d/y', $order['start_date']),
        'next_charge_date' => date('m/d/y', $order['next_charge_date']),
        'expire_date' => date('m/d/y', $order['expire_date']),
        'status' => $order['status'] == DONATION_CANCELLED ? 'cancelled' : 'active',
      ),
      'class' => $class,
    );
  }

  $vars['orders'] = theme('table', array('header' => $header, 'rows' => $rows));

  // format payment schedule into a data table
  $payment_schedule_header = array('Amount', 'Charge Date', 'Processed Status');
  // format some of the data in the array before output
  foreach ($vars['payment_schedule'] as &$payment_schedule) {
    $payment_schedule['amount'] = '$' . money_format('%i', $payment_schedule['amount']);
    $payment_schedule['charge_date'] = date('m/d/y', $payment_schedule['charge_date']);
    $payment_schedule['status'] = empty($payment_schedule['status']) ? 'pending' : $payment_schedule['status'];
  }
  $vars['payment_schedule'] = theme('table', array('header' => $payment_schedule_header, 'rows' => $vars['payment_schedule']));
}

/**
 * Theme Fundraiser mapping form.
 */
function theme_fundraiser_mapping_form($form) {
  if ($form['mapping']['cid']) {
    foreach ($form['mapping']['cid'] as $key => $cid) {
      if (is_numeric($key)) {
        $row = array();
        $row[] = drupal_render($form['mapping']['cid'][$key]);
        $row[] = drupal_render($form['mapping']['type'][$key]);
        $rows[] = $row;
      }
    }
    $row = array();
    $row[] = drupal_render($form['submit']);
    $row[] = '&nbsp;';
    $rows[] = $row;

    $header = array("Webform Component", "Profile Field");
    $output = drupal_render($form['info']);
    $output .= theme('table', array('header' => $header, 'row' => $rows, 'attributes' => array('id' => 'fundraiser_mapping_form')));
    $output .= drupal_render($form);
  }
  else {
    $output = "<p>You must add some fields to the webform before you can map them.</p>";
  }
  return $output;
}

/**
 * Theme the credit card expiration date form field.
 */
function theme_fundraiser_credit_card_expiration_date($element) {
  $element['card_expiration_month']['#title'] = t('Expiration Date');
  $month = drupal_render($element['card_expiration_month']);
  $element['card_expiration_year']['#title'] = '';
  $year = drupal_render($element['card_expiration_year']);

  preg_match('#<select.*/select>#ms', $year, $matches);
  $month = str_replace('</select>', "</select>$matches[0]", $month);

  $expiration_date = "<div class='expiration-date-wrapper clear-block'>$month</div>";
  return $expiration_date . drupal_render($element); // Always end any form theming with drupal_render() for cleanup.
}
