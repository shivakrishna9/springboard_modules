<?php

/**
 * @file
 * Theme related functions for the fundraiser module.
 * These are isolated to make them easy to locate and maintain.
 */

/**
 * Implementation of hook_theme().
 */
function fundraiser_theme($existing, $type, $theme, $path) {
  return array(
    'fundraiser_mapping_form' => array(
      'variables' => array(),
    ),
    'fundraiser_ask_amounts' => array(),
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
 * Implemenation of hook_preprocess_fundraiser_confirmation().
 * http://api.drupal.org/api/drupal/modules--system--theme.api.php/function/hook_preprocess/7
 */
function fundraiser_preprocess_fundraiser_confirmation(&$vars) {
  $sid = $vars['sid'];
  if ($sid) {
    // load up the id of the order
    $order_id = db_query('SELECT order_id FROM {fundraiser_webform_order} WHERE sid = :sid', array(':sid' => $sid))->fetchColumn();
    $order = uc_order_load($order_id);  // SHOULD BE HOOKED. TODO
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    $submission = webform_get_submission($vars['node']->nid, $sid);
    $message = $vars['node']->confirmation_page_body;
    // replace any submission level tokens
    $message = _webform_filter_values($message, $vars['node'], $submission, NULL, FALSE);
    // replace any order tokens
    $message = token_replace($message, 'order', $order);

    // set a variable that can be used in the template
    $vars['confirmation_message'] = $message;
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
  if (isset($form['mapping']) && $form['mapping']['cid']) {
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

/** 
 * Theme the ask amounts table on webform creation.
 */
function theme_fundraiser_ask_amounts($form){
  $amounts = array_shift($form);
  $header = array( t('Ask amount'), t('Label'), t('Actions') );
  $rows = array();
  for ($i=0; $i < $amounts['amount_delta']['#value']; $i++) {
    if (isset($amounts[$i]['amount_' . $i])) {
      $amount = drupal_render($amounts[$i]['amount_' . $i]);
      $label = drupal_render($amounts[$i]['label_' . $i]);
      $remove = drupal_render($amounts[$i]['remove_' . $i]);
      $rows[] = array($amount, $label, $remove);
    }
  }
  return theme('table', array('header' => $header, 'rows' => $rows));
}
