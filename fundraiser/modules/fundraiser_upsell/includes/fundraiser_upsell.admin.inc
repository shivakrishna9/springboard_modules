<?php
/**
 * @file
 * fundraiser_upsell.admin.inc - Admin forms and functions
 */

/**
 * Admin configuration form for the Fundraiser Upsell global settings.
 */
function fundraiser_upsell_admin_settings_form($form, &$form_state) {
  // Set the default bracket info
  $brackets_default = array(
    0 => array('low' => 0, 'high' => 20, 'upsell' => 10),
    1 => array('low' => 21, 'high' => 40, 'upsell' => 25),
    2 => array('low' => 41, 'high' => 60, 'upsell' => 50),
  );
  $brackets = variable_get("fundraiser_upsell_brackets", $brackets_default);
  // Set the delta for the number of brackets plus one extra blank
  if (isset($_POST['brackets_delta'])) {
    $delta = $_POST['brackets_delta']+1;
  }
  elseif (count($brackets)) {
    $delta = count($brackets) + 1;
  }
  else {
    $delta = 3;
  }
  $default_content = '<h3>Become a</h3><h2>MONTHLY DONOR</h2><p>Your donation will repeat every month.</p>';
  $default_disclaimer = '<em>By signing up to be a monthly donor, your credit card will be charged the monthly amount you specify beginning next month.</em>';
  // Build the form
  $form = array();
  $form['fundraiser_upsell_brackets'] = array(
    '#title' => t('Upsell Brackets'),
    '#type' => 'fieldset',
    '#prefix' => '<div class="clear-block" id="brackets-wrapper">',
    '#suffix' => '</div>',
    '#description' => 'Leave a row of fields blank to delete that row of brackets.',
  );
  // This is the container that hold the brackets and is rebuilt with AJAX call
  $form['fundraiser_upsell_brackets']['brackets'] = array(
    '#prefix' => '<div id="bracket-amounts">',
    '#suffix' => '</div>',
  );
  // Build and fill the brackets
  for ($i = 0; $i < $delta; ++$i) {
    $form['fundraiser_upsell_brackets']['brackets']['fundraiser_upsell_brackets_low_' . $i] = array(
      '#title' => t('Low Value'),
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => isset($brackets[$i]) ? $brackets[$i]['low'] : '',
      '#prefix' => '<div class="upsell_low" style="float: left; clear: left; margin-right: 3em;">',
      '#suffix' => "</div>",
    );
    $form['fundraiser_upsell_brackets']['brackets']['fundraiser_upsell_brackets_high_' . $i] = array(
      '#title' => t('High Value'),
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => isset($brackets[$i]) ? $brackets[$i]['high'] : '',
      '#prefix' => '<div class="upsell_high" style="float: left; margin-right: 3em;">',
      '#suffix' => "</div>",
    );
    $form['fundraiser_upsell_brackets']['brackets']['fundraiser_upsell_brackets_upsell_' . $i] = array(
      '#title' => t('Upsell Value'),
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => isset($brackets[$i]) ? $brackets[$i]['upsell'] : '',
      '#prefix' => '<div class="upsell_value" style="float: left; margin-right: 3em;">',
      '#suffix' => "</div>",
    );
  }
  // Store the number of brakcets for later
  $form['fundraiser_upsell_brackets']['brackets']['brackets_delta'] = array(
    '#type' => 'hidden',
    '#value' => $delta,
  );
  // Button to add another bracket and rebuild this section
  $form['fundraiser_upsell_brackets']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add another'),
    '#description' => t("Click here to add more brackets."),
    '#submit' => array('fundraiser_upsell_admin_settings_ajax_submit'), // If no javascript action.
    '#ajax' => array(
      'callback' => '_fundraiser_upsell_admin_settings_ajax',
      'wrapper' => 'bracket-amounts',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#prefix' => '<div style="float: left; clear: both; padding-top: 1em">',
    '#suffix' => "</div>",
  );
  // Build the rest of the form
  $form['fundraiser_upsell_default_content'] = array(
    '#title' => t('Default content for Upsell'),
    '#type' => 'textarea',
    '#default_value' => variable_get('fundraiser_upsell_default_content', $default_content),
  );
  // @TODO : allow WYSIWYG format selection
  //$form['fundraiser_upsell_default_content']['format'] = filter_form($myfield1['format'], NULL, array('field1_format'));
  // @TODO : add in the token list
  $form['fundraiser_upsell_default_content_disclaimer'] = array(
    '#title' => t('Default disclaimer for Upsell content'),
    '#type' => 'textfield',
    '#maxlength' => 256,
    '#default_value' => variable_get('fundraiser_upsell_default_content_disclaimer', $default_disclaimer),
  );
  $form['fundraiser_upsell_sf_campaign_id'] = array(
    '#title' => t('Sustainer Salesforce Campaign ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('fundraiser_upsell_sf_campaign_id', ''),
  );
  $form['fundraiser_upsell_thank_you_content'] = array(
    '#title' => t('Default content for thank you message'),
    '#type' => 'textarea',
    '#default_value' => variable_get('fundraiser_upsell_thank_you_content', 'Thank you for your recurring donation!'),
  );
  // @TODO : allow WYSIWYG format selection
  //$form['fundraiser_upsell_thank_you_content']['format'] = filter_form($myfield1['format'], NULL, array('field1_format'));
  // @TODO : add in the token list
  $form['fundraiser_upsell_acceptance'] = array(
    '#title' => t('Acceptance Cookie'),
    '#type' => 'fieldset',
  );
  $form['fundraiser_upsell_acceptance']['fundraiser_upsell_acceptance_enabled'] = array(
    '#title' => t('Enabled'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('fundraiser_upsell_acceptance_enabled', 0),
  );
  $form['fundraiser_upsell_acceptance']['fundraiser_upsell_acceptance_lifetime'] = array(
    '#title' => t('Cookie lifetype in days'),
    '#type' => 'textfield',
    '#default_value' => variable_get('fundraiser_upsell_acceptance_lifetime', ''),
  );
  $form['fundraiser_upsell_rejection'] = array(
    '#title' => t('Rejection Cookie'),
    '#type' => 'fieldset',
  );
  $form['fundraiser_upsell_rejection']['fundraiser_upsell_rejection_enabled'] = array(
    '#title' => t('Enabled'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('fundraiser_upsell_rejection_enabled', 0),
  );
  $form['fundraiser_upsell_rejection']['fundraiser_upsell_rejection_lifetime'] = array(
    '#title' => t('Cookie lifetype in days'),
    '#type' => 'textfield',
    '#default_value' => variable_get('fundraiser_upsell_rejection_lifetime', ''),
  );
  $form['fundraiser_upsell_debug'] = array(
    '#title' => t('Debug Mode'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('fundraiser_upsell_debug', 0),
    '#description' => t('Debug mode will open the upsell box regardless of cookie settings,'),
  );
  // Additional submit handler to package the bracket data into a single variable
  $form['#submit'][] = 'fundraiser_upsell_admin_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Validation handler for the admin upsell config
 */
function fundraiser_upsell_admin_settings_form_validate($form, &$form_state) {
  $vals = $form_state['values'];
  // Make sure that all of the bracket settings are numeric only
  $delta = $vals['brackets_delta'];
  // Loop though the bracket values
  for ($i = 0; $i < $delta; ++$i) {
    if (!empty($vals['fundraiser_upsell_brackets_low_' . $i]) && !is_numeric($vals['fundraiser_upsell_brackets_low_' . $i])) {
      form_set_error("fundraiser_upsell_brackets][brackets][fundraiser_upsell_brackets_low_$i", t('The bracket values must be numbers only.'));
    }
    if (!empty($vals['fundraiser_upsell_brackets_high_' . $i]) && !is_numeric($vals['fundraiser_upsell_brackets_high_' . $i])) {
      form_set_error('upsell', t('The bracket values must be numbers only.'));
    }
    if (!empty($vals['fundraiser_upsell_brackets_upsell_' . $i]) && !is_numeric($vals['fundraiser_upsell_brackets_upsell_' . $i])) {
      form_set_error('upsell', t('The bracket values must be numbers only.'));
    }
  }
  // If either cookie is set, it must be a number greater than 0
  if ($vals['fundraiser_upsell_acceptance_enabled'] == 1 && (!is_numeric($vals['fundraiser_upsell_acceptance_lifetime']) || $vals['fundraiser_upsell_acceptance_lifetime'] < 1)) {
    form_set_error('upsell', t('Acceptance cookie lifetime must be a number greater than 0.'));
  }
  if ($vals['fundraiser_upsell_rejection_enabled'] == 1 && (!is_numeric($vals['fundraiser_upsell_rejection_lifetime']) || $vals['fundraiser_upsell_rejection_lifetime'] < 1)) {
    form_set_error('upsell', t('Rejection cookie lifetime must be a number greater than 0.'));
  }
}

/**
 * Additional submit handler for the admin upsell config
 * This is to collect the upsell brackets into an array and set that to a system
 * variable instead of allow the default setting of one variable per bracket setting.
 */
function fundraiser_upsell_admin_settings_form_submit($form, &$form_state) {
  $delta = $form_state['values']['brackets_delta'];
  $brackets = array();
  // Loop though the bracket values
  for ($i = 0; $i < $delta; ++$i) {
    // Collect the bracket info into a single array (if it exists)
    if ((!empty($form_state['values']['fundraiser_upsell_brackets_low_' . $i]) || $form_state['values']['fundraiser_upsell_brackets_low_' . $i] == 0) && !empty($form_state['values']['fundraiser_upsell_brackets_high_' . $i]) && !empty($form_state['values']['fundraiser_upsell_brackets_upsell_' . $i])) {
      $brackets[$i]['low'] = $form_state['values']['fundraiser_upsell_brackets_low_' . $i];
      $brackets[$i]['high'] = $form_state['values']['fundraiser_upsell_brackets_high_' . $i];
      $brackets[$i]['upsell'] = $form_state['values']['fundraiser_upsell_brackets_upsell_' . $i];
    }
    // Unset the individual bracket fields to prevent addinga  variable for each
    unset($form_state['values']['fundraiser_upsell_brackets_low_' . $i]);
    unset($form_state['values']['fundraiser_upsell_brackets_high_' . $i]);
    unset($form_state['values']['fundraiser_upsell_brackets_upsell_' . $i]);
  }
  $form_state['values']['fundraiser_upsell_brackets'] = $brackets;
}

/**
 * Callback handler for the donation ask amount form #ajax.
 */
function _fundraiser_upsell_admin_settings_ajax($form, $form_state) {
  return $form['fundraiser_upsell_brackets']['brackets'];
}

/**
 * Submit handler for ask amount more button.
 */
function fundraiser_upsell_admin_settings_ajax_submit(&$form, &$form_state) {
  ++$form_state['values']['brackets_delta'];
  $form_state['rebuild'] = TRUE;
}

/**
 * Admin configuration thank you form.
 */
function fundraiser_upsell_admin_thank_you_settings($form, &$form_state) {
  $site_mail = variable_get('site_mail', '');
  $form = array();
  $form['fundraiser_upsell_thank_you_content'] = array(
    '#title' => t('Default content for Upsell Thank You'),
    '#type' => 'textarea',
    '#default_value' => variable_get('fundraiser_upsell_thank_you_content', ''),
  );
  $form['fundraiser_upsell_thank_you_content_tokens']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fundraiser_upsell_thank_you_content_tokens']['tokens']['help'] = array(
    '#value' => theme('token_help', 'upsell'),
  );
  $form['fundraiser_upsell_email'] = array(
    '#title' => t('Email Defaults'),
    '#type' => 'fieldset',
  );
  $form['fundraiser_upsell_email']["fundraiser_upsell_email_from"] = array(
    '#title' => t('From Address'),
    '#type' => 'textfield',
    '#default_value' => variable_get('fundraiser_upsell_email_from', $site_mail),
    '#description' => 'Format: "From Name" &lt;email@address&gt;',
  );
  $form['fundraiser_upsell_email']["fundraiser_upsell_email_bcc"] = array(
    '#title' => t('BCC Address'),
    '#type' => 'textfield',
    '#default_value' => variable_get('fundraiser_upsell_email_bcc', ''),
    '#description' => 'Format: "From Name" &lt;email@address&gt;',
  );
  $form['fundraiser_upsell_email']['fundraiser_upsell_email_subject'] = array(
    '#title' => 'Email Subject',
    '#type' => 'textfield',
    '#default_value' => variable_get('fundraiser_upsell_email_subject', 'Thank you for becoming a sustaining partner.'),
  );
  $form["fundraiser_upsell_thank_you_email_html"] = array(
    '#title' => t('Thank You HTML Email Content'),
    '#type' => 'textarea',
    '#default_value' => variable_get('fundraiser_upsell_thank_you_email_html', ''),
  );
  $form['fundraiser_upsell_thankyou_form']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fundraiser_upsell_thankyou_form']['tokens']['help'] = array(
    '#value' => theme('token_help', 'upsell'),
  );
  $form["fundraiser_upsell_thank_you_email_text"] = array(
    '#title' => t('Thank You TEXT Email Content'),
    '#type' => 'textarea',
    '#default_value' => variable_get('fundraiser_upsell_thank_you_email_text', ''),
  );
  $form['fundraiser_upsell_thankyou_form_text']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fundraiser_upsell_thankyou_form_text']['tokens']['help'] = array(
    '#value' => theme('token_help', 'upsell'),
  );
  return system_settings_form($form);
}

