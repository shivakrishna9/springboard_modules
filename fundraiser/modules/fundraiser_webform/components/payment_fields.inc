<?php

/**
 * @file
 * Webform module event_table component.
 */

// Payment Fields depends on functions provided by fieldset.
webform_component_include('fieldset');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_payment_fields() {
  return array(
    'name' => 'Payment Fields',
    'form_key' => 'payment_fields',
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'collapsible' => 0,
      'collapsed' => 0,
      'description' => '',
      'title_display' => 0,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_payment_fields($component) {
  $form = _webform_edit_fieldset($component);
  $form['edit_notice']['#markup'] = '<div>' . t('The settings for this field are controlled by Fundraiser.') . '</div>';
  $form['display']['#access'] = FALSE;
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_payment_fields($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  $element = _webform_render_fieldset($component, $value, $filter);
  $element['#pre_render'][] = '_fundraiser_component_payment_fields_pre_render';

  // Loop through the payment methods and add the fields for each type. Unsupported will be hidden.
  $payment_methods = array('none', 'credit', 'paypal', 'debit', 'check');
  foreach ($payment_methods as $payment_method) {
    // Load up the instance
    $element[$payment_method] = array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'id' => 'fundraiser-payment-method-' . str_replace('_', '-', $payment_method),
        'class' => array('fundraiser-payment-method'),
        ),
      '#states' => array(
        'visible' => array(
          ':input[name="submitted[payment_information][payment_method]"]' => array('value' => $payment_method),
        ),
      ),
    );
    switch ($payment_method) {
      case 'none':
        $element[$payment_method]['none_message'] = array(
          '#markup' => '<div class="payment-method-message">' . t('Enjoy your free stuff.') . '</div>',
        );
      break;
      case 'debit':
        $element[$payment_method]['debit_routing'] = array(
          '#type' => 'textfield',
          '#title' => t('Bank routing number'),
        );
        $element[$payment_method]['debit_account'] = array(
          '#type' => 'textfield',
          '#title' => t('Bank account number'),
        );
      break;
      case 'paypal':
        $element[$payment_method]['paypal_email'] = array(
          '#type' => 'textfield',
          '#title' => t('Paypal email'),
        );
      break;
      case 'check':
        $element[$payment_method]['check_message'] = array(
          '#markup' => '<div class="payment-method-message">' . t('Please remember to mail your check in.') . '</div>',
        );
      break;
      case 'credit':
      default:
        $element[$payment_method]['card_number'] = array(
          '#type' => 'textfield',
          '#title' => t('Credit card number'),
        );
        $months[1] = 'January';
        $months[2] = 'February';
        $months[3] = 'March';
        $months[4] = 'April';
        $months[5] = 'May';
        $months[6] = 'June';
        $months[7] = 'July';
        $months[8] = 'August';
        $months[9] = 'September';
        $months[10] = 'October';
        $months[11] = 'November';
        $months[12] = 'December';
        $element[$payment_method]['card_expiration_month'] = array(
          '#type' => 'select',
          '#options' => $months,
          '#title' => t('Expiration month'),
        );
        $this_year = date('Y');
        $years[$this_year] = $this_year;
        for ($i = 1; $i <= 15; $i++) {
          $years[$this_year + $i] = $this_year + $i;
        }
        $element[$payment_method]['card_expiration_year'] = array(
          '#type' => 'select',
          '#options' => $years,
          '#title' => t('Expiration year'),
        );
/*
  // Combine expiration month and year into a single component.
  $cc_exp_month_field =& _fundraiser_webform_find_field($form, $component_hierarchy['card_expiration_month']);
  $cc_exp_year_field =& _fundraiser_webform_find_field($form, $component_hierarchy['card_expiration_year']);
  $cc_info_field =& _fundraiser_webform_find_field($form, $component_hierarchy['credit_card_information']);
  $cc_exp_month_field['#default_value'] = date('n');
  $cc_exp_year_field['#default_value'] = date('Y');

  // Make sure the year field always has a good range of years.
  $this_year = date('Y');
  $years = array($this_year => $this_year);
  for ($i = 1; $i <= 10; $i++) { // Up to 10 years out.
    $years[$this_year + $i] = $this_year + $i;
  }
  $cc_exp_year_field['#options'] = $years;

  // Check to see if the credit card information fieldset still exists because it may have been removed.
  if (is_array($cc_info_field)) {
    $cc_info_field['expiration_date'] = array();
    $cc_info_field['expiration_date']['card_expiration_month'] = $cc_exp_month_field;
    $cc_info_field['expiration_date']['card_expiration_year'] = $cc_exp_year_field;
    $cc_info_field['expiration_date']['#weight'] = isset($cc_exp_month_field['#weight']) ? $cc_exp_month_field['#weight'] : 0;
    $cc_info_field['expiration_date']['#theme'] = 'fundraiser_credit_card_expiration_date';
  }
  else {
    // Add the new expiration_date directly to the form with the same weight as the cc_exp_month field.
    $form['expiration_date'] = array();
    $form['expiration_date']['card_expiration_month'] = $cc_exp_month_field;
    $form['expiration_date']['card_expiration_year'] = $cc_exp_year_field;
    $form['expiration_date']['#weight'] = $cc_exp_month_field['#weight'];
    $form['expiration_date']['#theme'] = 'fundraiser_credit_card_expiration_date';
  }
*/
        $element[$payment_method]['card_cvv'] = array(
          '#type' => 'textfield',
          '#title' => t('CVV'),
        );
      break;
    }
    // Allow other modules to alter the fields before adding to form.
    drupal_alter('payment_method_fields', $element[$payment_method], $payment_method);
  }
  return $element;
}

/**
 * Pre-render function to add our id to the js settings
 */
function _fundraiser_component_payment_fields_pre_render($element) {
  $node = isset($element['#webform_component']['nid']) ? node_load($element['#webform_component']['nid']) : NULL;

  // Add the id of the outer fieldset to the js settings
  drupal_add_js(array('fundraiserWebform' => array('components' => array('payment_fields' => array('id' => $element['#attributes']['id'])))), 'setting');
  return $element;
}