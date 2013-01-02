<?php


/**
 * @file
 * Email Wrappers helper functions for 3rd party module development.
 */

/**
 * Drop this into your code to add/edit/delete mail settings.
 *
 * Be sure to provide module and name (unique per module, much like block deltas)
 *to ensure email wrappers can load your module-specific settings.
 *
 * @param $edit
 * Placeholder for form API edit cruft.
 *
 * @param $params
 * Associative array of additional parameters. Supported values:
 * - module (required): your module name
 * - name (required): the name of your email configuration (unique per module, similar to block deltas)
 * - send_callback (optional): callback function to invoke after a mail is sent.
 * - recipient_callback (optional): callback function to invoke to get mail recipient
 *   address. This is required if you aren't hardcoding the recipient in the mail configuration.
 */
function email_wrappers_mail_form($edit, $params = array()) {
  $settings = email_wrappers_mail_load($params);
  $form['mid'] = array(
    '#type' => 'value',
    '#value' => isset($settings->mid) ? $settings->mid : '',
  );
  $form['module'] = array(
    '#type' => 'value',
    '#value' => isset($settings->module) ? $settings->module : $params['module'],
  );
  $form['delta'] = array(
    '#type' => 'value',
    '#value' => isset($settings->delta) ? $settings->delta : $params['delta'],
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => isset($settings->name) ? $settings->name : $params['name'],
  );
  // TODO: add template select with ajax import?
 /* $form['tid'] = array(
    '#type' => 'value',
    '#value' => isset($settings->tid) ? $settings->tid : 0,
  ); */
  $form['recipient_callback'] = array(
    '#type' => 'value',
    '#value' => isset($params['extra']['recipient_callback']) ? $params['extra']['recipient_callback'] : '',
  );
  $form['send_callback'] = array(
    '#type' => 'value',
    '#value' => isset($params['extra']['send_callback']) ? $params['extra']['send_callback'] : '',
  );
  $form['headers'] = array(
    '#type' => 'fieldset',
    '#title' => t('mail headers'),
    '#description' => t(''),
    '#collapsible' => TRUE,
  );
  $form['headers']['from_name'] = array(
    '#type' => 'textfield',
    '#title' => t('from name'),
    '#description' => t(''),
    '#default_value' => isset($settings->from_name) ? $settings->from_name : '',
    '#required' => TRUE,
  );
  $form['headers']['from_mail'] = array(
    '#type' => 'textfield',
    '#title' => t('From mail'),
    '#description' => t(''),
    '#default_value' => isset($settings->from_mail) ? $settings->from_mail : '',
    '#required' => TRUE,
  );
  $form['headers']['recipients'] = array(
    '#type' => 'textfield',
    '#title' => t('Recipient email address'),
    '#description' => t('The primary recipient email address(es). For more than one address, addresses should be separated by a comma. Example: test1@example.com,test2@example.com'),
    '#default_value' => isset($settings->recipients) ? $settings->recipients : '',
  );
  $form['headers']['cc'] = array(
    '#type' => 'textfield',
    '#title' => t('CC email address'),
    '#description' => t('Email addresses to CC when sending this mail. For more than one address, addresses should be separated by a comma. Example: test3@example.com,test4@example.com'),
    '#default_value' => isset($settings->cc) ? $settings->cc : '',
  );
  $form['headers']['bcc'] = array(
    '#type' => 'textfield',
    '#title' => t('BCC email address'),
    '#description' => t('Email  // TODO: add check on setting to mail send, format html body using appropriate
  // core filter. addresses to BCC when sending this mail. For more than one address, addresses should be separated by a comma. Example: test5@example.com,test6@example.com'),
    '#default_value' => isset($settings->bcc) ? $settings->bcc : '',
  );
  $form['message'] = array(
    '#type' => 'fieldset',
    '#title' => t('mail body'),
    '#description' => t(''),
    '#collapsible' => TRUE,
  );
  $form['message']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t(''),
    '#default_value' => isset($settings->subject) ? $settings->subject : '',
    '#required' => TRUE,
  );
  $form['message']['html_body'] = array(
    '#type' => 'text_format',
    '#title' => t('HTML message version'),
    '#description' => t('If you wish to send HTML or multipart HTML/text email enter the HTML version of the message body here.'),
    '#default_value' => isset($settings->html_message) ? $settings->html_message : '',
    '#format' => isset($settings->html_message_format) ? $settings->html_message_format : 'filtered_html',
  );
  $form['message']['html_body_format'] = array(
    '#type' => '',
    '#title' => t(''),
    '#description' => t(''),
    '#default_value' => isset($settings->html_message_format) ? $settings->html_message_format : '',
  );
  $form['message']['text_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Text message version'),
    '#description' => t('If you wish to send plaintext or multipart HTML/text email enter the text-only version of the message body here.'),
    '#default_value' => isset($settings->text_message) ? $settings->text_message : '',
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#validate'][] = 'email_wrappers_mail_form_validate';
  $form['#submit'][] = 'email_wrappers_mail_form_submit';
  return $form;
}

function email_wrappers_mail_form_validate(&$form, $form_state) {
  if (!email_wrappers_validate_address($form_state['values']['from_mail'])) {
    form_set_error('from_mail', t('You have entered an invalid email address in the "from mail" field: %mail', array('%mail' => $form_state['values']['from_mail'])));
  }

  if ($form_state['values']['recipients']) {
    $recipients = explode(',', $form_state['values']['recipients']);
    if (!count($recipients)) {
      $recipients = array($recipients);
    }
    foreach ($recipients as $recipient) {
      if (!email_wrappers_validate_address($recipient)) {
        form_set_error('recipients', t('You have entered an invalid email address in the "recipients" field: %mail', array('%mail' => $recipient)));
      }
    }
  }

  if ($form_state['values']['cc']) {
    $ccs = explode(',', $form_state['values']['cc']);
    if (!count($ccs)) {
      $ccs = array($ccs);
    }
    foreach ($ccs as $cc) {
      if (!email_wrappers_validate_address($cc)) {
        form_set_error('cc', t('You have entered an invalid email address in the "cc" field: %mail', array('%mail' => $cc)));
      }
    }
  }

  if ($form_state['values']['bcc']) {
    $bccs = explode(',', $form_state['values']['bcc']);
    if (!count($bccs)) {
      $bccs = array($bccs);
    }
    foreach ($bccs as $bcc) {
      if (!email_wrappers_validate_address($bcc)) {
        form_set_error('bcc', t('You have entered an invalid email address in the "bcc" field: %mail', array('%mail' => $bcc)));
      }
    }
  }

  if (!$form_state['values']['html_body'] && !$form_state['values']['text_body']) {
    form_set_error('html_body', t('You must include either a text or html body.'));
  }
}

function email_wrappers_mail_form_submit($form, $form_state) {
  $settings = _email_wrappers_concat_settings_from_submission($form_state['values']);
  dsm($settings);
  if (!$settings->mid) {
    email_wrappers_save($settings);
  }
  else {
    email_wrappers_update($settings);
  }
}

/**
 *
 */
function email_wrappers_validate_address($mail) {
  return filter_var($mail, FILTER_VALIDATE_EMAIL);
}

/**
 * Convert submitted form values into a mail object.
 */
function _email_wrappers_concat_settings_from_submission($values) {
  $settings = array(
    'mid' => $values['mid'] ? $values['mid'] : NULL,
    //'tid' => $values['tid'] ? $values['tid'] : NULL,
    'name' => $values['name'],
    'module' => $values['module'],
    'delta' => $values['delta'],
    'from_name' => $values['from_name'],
    'from_mail' => $values['from_mail'],
    'recipients' => $values['recipients'],
    'cc' => $values['cc'],
    'bcc' => $values['bcc'],
    'subject' => $values['subject'],
    'html_message' => $values['html_body']['value'],
    'html_message_format' => $values['html_body']['format'],
    'text_message' => $values['text_body'],
    'extra' => serialize(array(
      'send_callback' => isset($values['send_callback']) ? $values['send_callback'] : NULL,
      'recipient_callback' => isset($values['recipient_callback']) ? $values['recipient_callback'] : NULL,
    )),
  );
  return (object) $settings;
}

/**
 * Helper function, call this to send mail.
 *
 * @param $mail
 * mail object
 *
 * @param $params
 * Optional associative array containing additional parameters to send to callback functions.
 */
function mailtron_send_mail($mail, $params = array()) {
  // TODO: thoroughly examine dependency on mimemail, replace with hook_mail call
  // unless mimemail is actually needed.
  module_load_include('module', 'mimemail');
  module_load_include('inc', 'mimemail', 'theme/mimemail.theme');
  if (isset($mail->recipient_callback) && $mail->recipient_callback) {
    $function = $mail->recipient_callback;
    $mail->recipients = $function($mail, $params);
  }

  module_invoke_all('mimemail_send_mail_alter', $mail);

  $settings = mailtron_convert_settings($mail);
  mimemail($settings['sender'], $settings['recipient'], $settings['subject'], $settings['body'], FALSE, $settings['headers'], $settings['text']);

  // optionally call callback provided in $mail
  if (isset($mail->send_callback) && $mail->send_callback) {
    $function = $mail->send_callback;
    $function($mail, $params);
  }
}

function email_wrappers_save($settings) {
  drupal_write_record('email_wrappers', $settings);
}

function email_wrappers_update($settings) {
  drupal_write_record('email_wrappers', $settings, 'mid');
}