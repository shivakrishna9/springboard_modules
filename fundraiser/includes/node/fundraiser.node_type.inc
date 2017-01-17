<?php
/**
 * @file
 * Node type related hooks for the Fundraiser module.
 */

/**
 * Implements hook_form_alter().
 *
 * Alter content type settings to add "Enable fundraiser?" checkbox.
 */
function fundraiser_form_node_type_form_alter(&$form, &$form_state) {
  if (empty($form['type'])) {
    return;
  }

  $form['fundraiser_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fundraiser settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );
  $form['fundraiser_type']['fundraiser'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable fundraiser?'),
    '#default_value' => variable_get('fundraiser_' . $form['#node_type']->type, FALSE),
    '#description' => t('Enable this checkbox if this content type should provide a donation field.'),
  );
}

/**
 * Implements hook_node_type_insert().
 */
function fundraiser_node_type_insert($type) {
  if (!fundraiser_is_donation_type($type->type)) {
    return;
  }

  module_load_include('inc', 'fundraiser', 'includes/fundraiser.fields');
  _fundraiser_add_default_fields($type->type);
}

/**
 * Implements hook_node_type_update().
 */
function fundraiser_node_type_update($type) {
  if (!fundraiser_is_donation_type($type->type)) {
    return;
  }

  module_load_include('inc', 'fundraiser', 'includes/fundraiser.fields');
  _fundraiser_add_default_fields($type->type);
}

/**
 * Fundraiser form. This form collects the information to create donation forms per donation node types.
 * This is added to the node display via form_alter when editting nodes of the correct type.
 *
 * It ultimately populates the fundraiser table from which donation forms are created :
 * nid, gateway, receipt_email_from, receipt_email_address, receipt_email_subject, receipt_email_message,
 * donation_amounts, show_other_amount, minimum_donation_amount, confirmation_page_title,
 * confirmation_page_body, confirmation_page_format, redirect_url.
 */
function _fundraiser_donation_settings_form(&$form, &$form_state) {
  // Load the node up from form data so we have everything.
  if (empty($form['#node']) || !fundraiser_is_donation_type($form['#node']->type)) {
    return;
  }

  $node = $form['#node'];

  // Payment methods.
  $form['payment_methods'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment methods'),
    '#weight' => -4.95,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );

  // Get avaiable gateways and provide an interface on the fundraiser settings
  // to edit the payment_method fields later.
  $gateway_options = array();
  $gateways = _fundraiser_gateway_info();
  if (empty($gateways)) {
    $form['payment_methods']['gateways'] = array(
      '#markup' => '<div>' . t('There are no gateways configured. Please create some gateways in the store configuration.') . '</div>',
    );
  }
  else {
    $fieldset = array(
      '#type' => 'fieldset',
      '#title' => t('Payment methods'),
      '#description' => t('Enable donation form payment methods and their corresponding gateways.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#theme' => 'fundraiser_form_payment_methods',
    );
    $configuration = !empty($node->gateways) ? $node->gateways : variable_get('fundraiser_payment_method_config_defaults', array());
    $form['payment_methods']['gateways'] = _fundraiser_form_payment_method_config_fieldset($gateways, $configuration, $fieldset, 'gateways');
    // add a validate callback.
    $form['#validate'][] = '_fundraiser_form_payment_method_validate';
  }

  // Fundraiser Settings (i.e., ask amounts).
  $form['fundraiser_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fundraiser settings'),
    '#weight' => -4.96,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );

  // Make changes to the currency settings field.
  if (!empty($form['field_fundraiser_currency'])) {
    $currency_field = &$form['field_fundraiser_currency'][$form['field_fundraiser_currency']['#language']];
    // Remove the none option.
    unset($currency_field['#options']['_none']);

    // If there is only one value left hide the field and set the value to the default currency.
    if (count($currency_field['#options']) === 1) {
      $currency_field['#type'] = 'value';
      $currency_field['#default_value'] = commerce_default_currency();
      $currency_field['#required'] = FALSE;
    }
    // Else, move it into the fundraiser group.
    else {
      $form['fundraiser_settings']['currency'] = array(
        '#type' => 'fieldset',
        '#title' => t('Currency'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      $currency_field['#title_display'] = 'none';
      $form['fundraiser_settings']['currency']['field_fundraiser_currency'] = $form['field_fundraiser_currency'];
      unset($form['field_fundraiser_currency']);
    }
  }

  // Add specific CSS settings for IE support. Pending
  // http://drupal.org/node/1015798 being resolved, and UC Store accounting for
  // that update. (Should be block not inline-block).
  $style = '.vertical-tabs fieldset fieldset legend {display: block;}';
  drupal_add_css($style, 'inline');
  $form['fundraiser_settings']['amount_wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ask amounts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => '<div class="clear-block" id="ask-wrapper">',
    '#suffix' => '</div>',
    // This is important, this allows the deltas below to function as part of
    // the array.
    '#tree' => TRUE,
  );
  $form['fundraiser_settings']['amount_wrapper']['donation_amounts'] = array(
    '#prefix' => '<div id="ask-amounts">',
    '#suffix' => '</div>',
    '#theme' => 'fundraiser_ask_amounts',
    '#tree' => TRUE,
  );

  // AJAXy bits.
  $donation_amounts = array();
  // The last submission takes precedence.
  if (!empty($form_state['values']['amount_wrapper']['donation_amounts'])) {
    foreach ($form_state['values']['amount_wrapper']['donation_amounts'] as $values) {
      $donation_amounts[$values['weight']] = array($values['amount'], $values['label']);
    }
    ksort($donation_amounts);
  }
  else {
    // If the form hasn't been submitted, then grab the defaults from the node
    // or empty. Donation amounts were loaded on node_load, and exploded at that
    // time. If they exist, they're on the node.
    $donation_amounts = !empty($node->donation_amounts) ? $node->donation_amounts : array();

    // If not there, then grab some defaults.
    if (count($donation_amounts) == 0) {
      $default_amounts_default_value = "10|$10\n20|$20\n50|$50\n100|$100";
      $default_amounts = variable_get('fundraiser_default_amounts', $default_amounts_default_value);
      $default_amounts = explode("\n", $default_amounts);
      $default_amounts = array_unique($default_amounts);
      $exploded_amounts = array();
      foreach ($default_amounts as $default_amount) {
        $these_amounts = explode('|', $default_amount);
        if (count($these_amounts) == 1) {
          $these_amounts[1] = '$' . $these_amounts[0];
        }
        $exploded_amounts[] = $these_amounts;
      }
      $donation_amounts = $exploded_amounts;
    }
  }

  // Default provide one spare blank for users to fill in.
  // If the button clicked was to add another, this line will add an extra field as expected on reload.
  $donation_amounts[] = array('', '');
  // Generate a form set for each existing amount. (To be themed into table format.)
  $index = 0;
  foreach ($donation_amounts as $weight => $donation_amount) {
    // Filter out other|Other combinations. We don't want to display those in this form.
    // And if we did, the value would fail validation (not a number).
    // There is probably a better way to track this, but for now this'll do.
    if (count($donation_amount) == 2 && $donation_amount[0] != 'other') {
      $form['fundraiser_settings']['amount_wrapper']['donation_amounts'][$index] = _fundraiser_ask_form($donation_amount, $index, $node, $weight);
    }
    ++$index;
  }

  // AJAX fix: reorder donation amounts in $form_state['input'] by weight.
  // Corrects the display order of donation amounts when an ajax event is
  // triggered.
  if (isset($form_state['input']['amount_wrapper']['donation_amounts'])) {
    $old_donation_amounts = $form_state['input']['amount_wrapper']['donation_amounts'];
    unset($form_state['input']['amount_wrapper']['donation_amounts']);
    foreach ($old_donation_amounts as $donation_amount) {
      $new_donation_amounts[$donation_amount['weight']] = $donation_amount;
    }
    ksort($new_donation_amounts);
    foreach ($new_donation_amounts as $donation_amount) {
      $form_state['input']['amount_wrapper']['donation_amounts'][] = $donation_amount;
    }
  }

  $form['fundraiser_settings']['amount_wrapper']['amount_more'] = array(
    '#type' => 'button',
    '#submit' => array(),
    '#value' => t('Add another'),
    '#description' => t('Click here to add more choices.'),
    '#ajax' => array(
      'callback' => '_fundraiser_donation_settings_form_amounts',
      'wrapper' => 'ask-amounts',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  // Additional fundraiser settings.
  $form['fundraiser_settings']['amount_wrapper']['show_other_amount'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show other amount option'),
    '#description' => t('Use this option if you want to provide an "Other Amount" field in conjunction with the list ' .
      'of Donation Amounts.'),
    '#default_value' => isset($node->show_other_amount) ? $node->show_other_amount : variable_get('fundraiser_default_other', TRUE),
  );
  $form['fundraiser_settings']['amount_wrapper']['minimum_donation_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum donation amount'),
    '#description' => t('The minimum acceptable donation amount.'),
    '#default_value' => isset($node->minimum_donation_amount) ?
      number_format($node->minimum_donation_amount, 2) : variable_get('fundraiser_default_minimum', 10.00),
  );

  $form['#validate'][] = '_fundraiser_form_amounts_validate';

  // @todo this should probably go on a different table, not here.
  // Hidden fields. These are set later on other forms, included here to prevent
  // overwrite during saves.
  $form['receipt_email_from'] = array(
    '#type' => 'value',
    '#value' => isset($node->receipt_email_from) ? $node->receipt_email_from : '',
  );
  $form['receipt_email_address'] = array(
    '#type' => 'value',
    '#value' => isset($node->receipt_email_address) ? $node->receipt_email_address : '',
  );
  $form['receipt_email_subject'] = array(
    '#type' => 'value',
    '#value' => isset($node->receipt_email_subject) ? $node->receipt_email_subject : '',
  );
  $form['receipt_email_message'] = array(
    '#type' => 'value',
    '#value' => isset($node->receipt_email_message) ? $node->receipt_email_message : '',
  );

  // Share the "Display" fieldset with related modules (e.g., Form Layouts).
  if (empty($form['springboard_display'])) {
    $form['springboard_display'] = array(
      '#type' => 'fieldset',
      '#title' => t('Display settings'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#group' => 'additional_settings',
    );
  }

  // Add CSS and JS + hide "Display settings" if it is empty during after build.
  $form['#after_build'][] = '_fundraiser_node_form_after_build';
}

/**
 * Afterbuild for node type form, to avoid AJAX losing js, css.
 *
 * Also, this hides the "Display settings" fieldset Fundraiser provides
 * if another module has not altered the form to insert something into it.
 */
function _fundraiser_node_form_after_build($form, &$form_state) {
  // Hide "Display settings" fieldset if empty.
  $display_settings_is_empty = count(element_properties($form['springboard_display'])) == 0;
  if ($display_settings_is_empty) {
    $form['springboard_display']['#description'] = t('No display settings are enabled at this time.');
  }

  // Add no-settings note if Fundraiser Settings has been emptied by alter
  // hooks.
  $fundraiser_settings_is_empty = TRUE;
  foreach (element_children($form['fundraiser_settings']) as $key) {
    if (!isset($form['fundraiser_settings'][$key]['#access']) || $form['fundraiser_settings'][$key]['#access']) {
      $form['fundraiser_settings']['#description'] = t("There are no Fundraiser settings to display.");
      break;
    }
  }

  // Add additional js and css for this form.
  $module_path = drupal_get_path('module', 'fundraiser');
  drupal_add_css($module_path . '/css/fundraiser.css');
  drupal_add_js($module_path . '/js/fundraiser.js');

  return $form;
}

/**
 * Creates the options for the payment method configuration form.
 *
 * @param array $gateways
 *   Array of gateways for this site.
 * @param array $configuration
 *   An array of configuration values, usually from the field configuration.
 * @param array $fieldset
 *   FormAPI fieldset.
 * @param string $name
 *   Name of the fieldset.
 *
 * @return array
 *   An array of fields to display on a payment method configuration form.
 */
function _fundraiser_form_payment_method_config_fieldset($gateways, $configuration, $fieldset, $name) {
  $all_payment_methods = array();

  // Create an array of gateways by the type of available payment methods.
  foreach ($gateways as $gateway) {
    if (!empty($gateway['payment_method'])) {
      $types = $gateway['payment_method'];
      foreach ($types as $type) {
        $all_payment_methods[$type][] = $gateway;
      }
    }
  }

  // Sort all payment methods by the configured weight of the fields.
  foreach (array_keys($all_payment_methods) as $method_key) {
    $all_payment_methods[$method_key]['weight'] = !empty($configuration[$method_key]['weight']) ? $configuration[$method_key]['weight'] : 0;
  }
  uasort($all_payment_methods, '_fundraiser_payment_method_weight_compare');

  $method_count = count($all_payment_methods);
  // Loop through each method, comparing to those set on this node.
  foreach ($all_payment_methods as $method_key => $method) {
    // Remove the weight value
    unset($method['weight']);
    $field_method_settings = isset($configuration[$method_key]) ? $configuration[$method_key] : array();
    // Build the #options array for the payment processors supporting this payment method.
    $processor_options = array();
    foreach (element_children($method) as $key) {
      $processor_options[$method[$key]['id']] = $method[$key]['title'];
    }
    $fieldset[$method_key] = array('#tree' => TRUE);
    $fieldset[$method_key]['status'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable'),
      '#default_value' => isset($field_method_settings['status']) ? $field_method_settings['status'] : 0,
    );
    // Add auto check.
    if ($method_count == 1) {
      $fieldset[$method_key]['status']['#default_value'] = TRUE;
    }
    $fieldset[$method_key]['_status'] = array(
      '#type' => 'markup',
      '#title' => t('Method'),
      '#markup' => $method_key, // Could use a pretty name here
    );
    $fieldset[$method_key]['method'] = array(
      '#type' => 'value',
      '#value' => $method_key,
    );
    $fieldset[$method_key]['id'] = array(
      '#type' => 'select',
      '#title' => t('Payment Processor'),
      '#description' => t('The payment processor for this payment method.'),
      '#options' => $processor_options,
      '#default_value' => isset($field_method_settings['id']) ? $field_method_settings['id'] : NULL,
    );
    // Include these fields if there is more than one payment method.
    if ($method_count > 1) {
      $fieldset[$method_key]['label'] = array(
        '#type' => 'textfield',
        '#description' => t('The label displayed for this payment method. Required when multiple payment methods are enabled.'),
        '#title' => t('Label'),
        '#size' => 20,
        '#default_value' => isset($field_method_settings['label']) ? $field_method_settings['label'] : NULL,
      );
      $fieldset[$method_key]['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#delta' => 25,
        '#default_value' => isset($field_method_settings['weight']) ? $field_method_settings['weight'] : 0,
        '#attributes' => array('class' => array('fundraiser-payment-methods-weight')),
      );
      $fieldset[$method_key]['_default'] = array(
        '#type' => 'radio',
        '#title' => t('Default'),
        '#default_value' => !empty($field_method_settings['default']) ? $method_key : NULL,
        '#return_value' => $method_key,
        '#parents' => array($name, '_default'),
      );
    }
  }
  return $fieldset;
}

/**
 * Validate handler for our component, also sets the default value
 */
function _fundraiser_form_payment_method_validate($form, &$form_state) {
  // Check the set payment methods.
  $values = $form_state['values']['gateways'];
  $default_method = isset($values['_default']) ? $values['_default'] : '';
  $enabled_methods = array_filter($values, '_fundraiser_payment_method_array_filter_enabled');
  unset($form_state['values']['gateways']['_default']);
  unset($enabled_methods['_default']);
  // Check that there is at least one method enabled
  if (empty($enabled_methods)) {
    form_set_error('payment_methods', t('At least one payment method must be enabled.'));
  }
  else {
    // Check that the marked payment type actually have selected gateways to match.
    foreach ($enabled_methods as $method => $enabled_method) {
      if (empty($enabled_method['id'])) {
        form_set_error('gateways][' . $method . '][id', t('All enabled payment methods must select a gateway.'));
      }
      if (count($enabled_methods) > 1 && empty($enabled_method['label'])) {
        form_set_error('gateways][' . $method . '][label', t('Please enter a label for the %method gateway.', array('%method' => $method)));
      }
    }
    // Check that a default has been set.
    if (!isset($enabled_methods[$default_method])) {
      if (count($enabled_methods) == 1) {
        $default_method = key($enabled_methods);
        $form_state['values']['gateways'][$default_method]['default'] = 1;
      }
      else {
        form_set_error('fundraiser_settings][gateways][_default', t('Default method must be one of the enabled methods.'));
      }
    }
    else {
      $form_state['values']['gateways'][$default_method]['default'] = 1;
    }
  }
}

/**
 * Validate handler for amounts configuration.
 */
function _fundraiser_form_amounts_validate($form, &$form_state) {
  // Check the ask amounts, if we have a label and we dong have an amount, throw a fit.
  // If we have neither, assume it's meant to be empty and leave it be (it will be removed).
  $amounts = $form_state['values']['amount_wrapper']['donation_amounts'];
  foreach ($amounts as $index => $amount_set) {
    if (!empty($amount_set['amount']) && empty($amount_set['label'])) {
      form_set_error('amount_wrapper][donation_amounts][' . $index . '][label',
        t('The label cannot be empty when an amount is set.'));
    }
    if (empty($amount_set['amount']) && !empty($amount_set['label'])) {
      form_set_error('amount_wrapper][donation_amounts][' . $index . '][amount',
        t('The amount cannot be empty when a label is set.'));
    }
    if (!is_numeric($amount_set['amount']) && !empty($amount_set['amount'])) {
      form_set_error('amount_wrapper][donation_amounts][' . $index . '][amount',
        t('The amount must be a number.'));
    }
  }

  // Checking the minimum amount.
  $minimum_donation_amount = $form_state['values']['amount_wrapper']['minimum_donation_amount'];
  foreach ($amounts as $amount) {
    if (empty($amount['remove']) && $amount['amount'] !='') {
      $min = (isset($min) && $amount['amount'] > $min) ? $min : $amount['amount'];
    }
  }
  // If no min ask amount is found (no ask amounts set) default to
  // the minimum donation amount. This prevents errors when all ask amounts
  // are removed from a donation form.
  $min = isset($min) ? $min : $minimum_donation_amount;
  if ($min < $minimum_donation_amount) {
    form_set_error('amount_wrapper][minimum_donation_amount', t('The minimum donation amount must not be higher than the minimum ask amount.'));
  }
  if (!empty($minimum_donation_amount) && !is_numeric($minimum_donation_amount)) {
    form_set_error('amount_wrapper][minimum_donation_amount', t('The minimum donation amount must be a numerical value.'));
  }
}

/**
 * Array_filter callback for filtering by method status.
 */
function _fundraiser_payment_method_array_filter_enabled($method_settings) {
  return !empty($method_settings['status']);
}

/**
 * Sort payment methods by weight
 */
function _fundraiser_payment_method_weight_compare($a, $b) {
  $weight = $a['weight'] - $b['weight'];
  return $weight;
}

/**
 * Generate donation ask amount fields, used by donation form
 */
function _fundraiser_ask_form($amount_values = array(), $index, $node, $weight) {
  $form['default_amount'] = array(
    '#type' => 'checkbox',
    '#default_value' => isset($node->default_amount) ? ($node->default_amount == $amount_values[0]) : FALSE,
  );
  $form['amount'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($amount_values[0]) ? $amount_values[0] : '',
    '#size' => 10,
  );
  $form['label'] = array(
    '#type' => 'textfield',
    '#default_value' => isset($amount_values[1]) ? $amount_values[1] : '',
    '#size' => 30,
  );
  $form['remove'] = array(
    '#type' => 'checkbox',
    '#return_value' => 1,
    '#default_value' => 0,
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => $weight,
    '#delta' => 10,
    '#title_display' => 'invisible',
  );
  return $form;
}

/**
 * Callback handler for the donation ask amount form #ajax.
 */
function _fundraiser_donation_settings_form_amounts(&$form, &$form_state) {
  return $form['fundraiser_settings']['amount_wrapper']['donation_amounts'];
}
