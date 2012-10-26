<?php

/**
 * Admin configuration form.
 */
function sustainer_upsell_admin_settings_form(&$form) {
  $form = array();
  $form['sustainer_upsell_brackets'] = array(
    '#title' => t('Upsell Brackets'),
    '#type' => 'fieldset',
    '#prefix' => '<div class="clear-block" id="brackets-wrapper">',
    '#suffix' => '</div>',
  );
  $brackets = variable_get("sustainer_upsell_brackets",array());
  $form['sustainer_upsell_brackets']['brackets'] = array(
    '#prefix' => '<div id="bracket-amounts">',
    '#suffix' => '</div>',
  );
  if ($_POST['brackets_delta']) {
    $delta = $_POST['brackets_delta']+1;
  }
  elseif (count($brackets)) {
    $delta = count($brackets);
  }
  else {
    $delta = 3;
  }
  for($i = 0; $i < $delta; ++$i) {
    $form['sustainer_upsell_brackets']['brackets']['sustainer_upsell_brackets_low_'.$i] = array(
      '#title' => t('Low Value'),
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => $brackets[$i]['low'],
      '#prefix' => '<div style="float: left; clear: left; margin-right: 3em;">',
      '#suffix' => "</div>"
    );
    $form['sustainer_upsell_brackets']['brackets']['sustainer_upsell_brackets_high_'.$i] = array(
      '#title' => t('High Value'),
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => $brackets[$i]['high'],
      '#prefix' => '<div style="float: left; margin-right: 3em;">',
      '#suffix' => "</div>"
    );
    $form['sustainer_upsell_brackets']['brackets']['sustainer_upsell_brackets_upsell_'.$i] = array(
      '#title' => t('Upsell Value'),
      '#type' => 'textfield',
      '#size' => 15,
      '#default_value' => $brackets[$i]['upsell'],
      '#prefix' => '<div style="float: left; margin-right: 3em;">',
      '#suffix' => "</div>"
    );
  }
  $form['sustainer_upsell_brackets']['brackets']['brackets_delta'] = array(
    '#type' => 'hidden',
    '#value' => $delta,
  );
  $form['sustainer_upsell_brackets']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add another'),
    '#description' => t("Click here to add more brackets."),
    '#submit' => array('_sustainer_upsell_admin_settings_ahah_submit'), // If no javascript action.
    '#ahah' => array(
      'path' => 'admin/springboard/settings/fundraiser/sustainer_upsell/add_more_js',
      'wrapper' => 'bracket-amounts',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#prefix' => '<div style="float: left; clear: both; padding-top: 1em">',
    '#suffix' => "</div>"
  );
  $form['sustainer_upsell_default_content'] = array(
    '#title' => t('Default content for Upsell'),
    '#type' => 'textarea',
    '#default_value' => variable_get('sustainer_upsell_default_content',''),
  );
  $form['sustainer_upsell_sf_campaign_id'] = array(
    '#title' => t('Sustainer Salesforce Campaign ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('sustainer_upsell_sf_campaign_id',''),
  );
  $form['sustainer_upsell_acceptance'] = array(
    '#title' => t('Acceptance Cookie'),
    '#type' => 'fieldset'
  );
  $form['sustainer_upsell_acceptance']['sustainer_upsell_acceptance_enabled'] = array(
    '#title' => 'Enabled',
    '#type' => 'checkbox',
    '#default_value' => variable_get('sustainer_upsell_acceptance_enabled',0),
  );
  $form['sustainer_upsell_acceptance']['sustainer_upsell_acceptance_lifetime'] = array(
    '#title' => 'Cookie lifetype in days',
    '#type' => 'textfield',
    '#default_value' => variable_get('sustainer_upsell_acceptance_lifetime',''),
  );
  $form['sustainer_upsell_rejection'] = array(
    '#title' => t('Rejection Cookie'),
    '#type' => 'fieldset'
  );
  $form['sustainer_upsell_rejection']['sustainer_upsell_rejection_enabled'] = array(
    '#title' => 'Enabled',
    '#type' => 'checkbox',
    '#default_value' => variable_get('sustainer_upsell_rejection_enabled',0),
  );
  $form['sustainer_upsell_rejection']['sustainer_upsell_rejection_lifetime'] = array(
    '#title' => 'Cookie lifetype in days',
    '#type' => 'textfield',
    '#default_value' => variable_get('sustainer_upsell_rejection_lifetime',''),
  );
  $form['sustainer_upsell_debug'] = array(
    '#title' => 'Debug Mode',
    '#type' => 'checkbox',
    '#default_value' => variable_get('sustainer_upsell_debug',0),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save Configuration Settings',
  );
  // TODO - allow WYSIWYG format selection
  //$form['sustainer_upsell_default_content']['format'] = filter_form($myfield1['format'], NULL, array('field1_format'));
  return $form;
}

/**
 * Submit handler for ask amount more button.
 */
function _sustainer_upsell_admin_settings_ahah_submit(&$form, &$form_state) {
  unset($form_state['submit_handlers']);
  $delta = $form_state['values']['brackets_delta'];
  for ($i = 0; $i < $form_state['values']['brackets_delta']; ++$i) {
    if (!$form_state['values']['sustainer_upsell_brackets_low_' . $i]) {
      --$delta;
    }
  }
  ++$form_state['values']['brackets_delta'];

  form_execute_handlers('submit', $form, $form_state);
  $values = $form_state['values'];
  $form_state['rebuild'] = TRUE;
}


/**
 * Callback handler for the donation ask amount form #ahah.
 */
function _sustainer_upsell_admin_settings_ahah() {
  $form_state = array('storage' => NULL, 'submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  $backets_delta = $form_state['values']['brackets_delta'];
  $form = form_get_cache($form_build_id, $form_state);
  
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // Disable validation
  $form['#validate'] = NULL;
  $form_state['validate_handlers'] = NULL;
  //fundraiser_ahah_helper_disable_validation($form);
  
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  $amount_form = $form['sustainer_upsell_brackets']['brackets'];
  unset($amount_form['#prefix'], $amount_form['#suffix']);
 
  $output = theme('status_messages') . drupal_render($amount_form);

  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  drupal_json(array('status' => TRUE, 'data' => $output));
}


function sustainer_upsell_admin_settings_form_validate($form,&$form_state) {
  $vals = $form_state['values'];
  if(($vals['sustainer_upsell_acceptance_enabled'] == 1) && (!is_numeric($vals['sustainer_upsell_acceptance_lifetime']))) {
    form_set_error('upsell', t('Acceptance cookie lifetime must be a number greater than 0'));
  }
  if(($vals['sustainer_upsell_rejection_enabled'] == 1) && (!is_numeric($vals['sustainer_upsell_rejection_lifetime']))) {
    form_set_error('upsell', t('Rejection cookie lifetime must be a number greater than 0'));
  }
}

function sustainer_upsell_admin_settings_form_submit($form,&$form_state) {
  $vals = $form_state['values'];
  $brackets = array();
  foreach($vals AS $k => $v) {
    if(preg_match('/^sustainer_upsell_brackets_low_(\d*)/',$k,$m)) {
      $brackets["$m[1]"]["low"] = $v;
    }
    if(preg_match('/^sustainer_upsell_brackets_high_(\d*)/',$k,$m)) {
      $brackets["$m[1]"]["high"] = $v;
    }
    if(preg_match('/^sustainer_upsell_brackets_upsell_(\d*)/',$k,$m)) {
      $brackets["$m[1]"]["upsell"] = $v;
    }
  }
  foreach($brackets AS $k => $v) {
    if(($v['low']=='') || ($v['upsell']=='')) {
      unset($brackets[$k]);
    }
  }
  variable_set("sustainer_upsell_brackets",$brackets);
  variable_set("sustainer_upsell_default_content", $vals['sustainer_upsell_default_content']);
  variable_set("sustainer_upsell_sf_campaign_id", $vals['sustainer_upsell_sf_campaign_id']);
  variable_set("sustainer_upsell_acceptance_enabled",$vals['sustainer_upsell_acceptance_enabled']);
  variable_Set("sustainer_upsell_acceptance_lifetime", $vals['sustainer_upsell_acceptance_lifetime']);
  variable_set("sustainer_upsell_rejection_enabled", $vals['sustainer_upsell_rejection_enabled']);
  variable_Set("sustainer_upsell_rejection_lifetime", $vals['sustainer_upsell_rejection_lifetime']);
  variable_Set("sustainer_upsell_debug", $vals['sustainer_upsell_debug']);
}

/**
 * Admin configuration thank you form.
 */
function sustainer_upsell_admin_thank_you_settings() {
  $form = array();
  $form['sustainer_upsell_thank_you_content'] = array(
    '#title' => t('Default content for Upsell Thank You'),
    '#type' => 'textarea',
    '#default_value' => variable_get('sustainer_upsell_thank_you_content',''),
  );
  $form['sustainer_upsell_thank_you_content_tokens']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['sustainer_upsell_thank_you_content_tokens']['tokens']['help'] = array(
    '#value' => theme('token_help', 'order'),
  );
  $form['sustainer_upsell_email'] = array(
    '#title' => t('Email Defaults'),
    '#type' => 'fieldset'
  );
  $form['sustainer_upsell_email']["sustainer_upsell_email_from"] = array(
    '#title' => t('From Address'),
    '#type' => 'textfield',
    '#default_value' => variable_get('sustainer_upsell_email_from',''),
  );
  $form['sustainer_upsell_email']["sustainer_upsell_email_bcc"] = array(
    '#title' => t('BCC Address'),
    '#type' => 'textfield',
    '#default_value' => variable_get('sustainer_upsell_email_bcc',''),
  );
  $form["sustainer_upsell_thank_you_email_html"] = array(
    '#title' => t('Thank You HTML Email Content'),
    '#type' => 'textarea',
    '#default_value' => variable_get('sustainer_upsell_thank_you_email_html',''),
  );
  $form['sustainer_upsell_thankyou_form']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['sustainer_upsell_thankyou_form']['tokens']['help'] = array(
    '#value' => theme('token_help', 'order'),
  );
  $form["sustainer_upsell_thank_you_email_text"] = array(
    '#title' => t('Thank You TEXT Email Content'),
    '#type' => 'textarea',
    '#default_value' => variable_get('sustainer_upsell_thank_you_email_text',''),
  );
  $form['sustainer_upsell_thankyou_form_text']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['sustainer_upsell_thankyou_form_text']['tokens']['help'] = array(
    '#value' => theme('token_help', 'order'),
  );
  return system_settings_form($form);
}


