<?php
/**
 * @file
 * Provides mail functionality
 */

/**
 * Recurring payment pause mail admin form
 */
function sustainer_pause_mail_pause_form($form, &$form_state) {
  // Pull in the required include file to get access to mailtron helper functions.
  module_load_include('inc', 'mailtron', 'mailtron.api');

  // Set up the initial parameters for mailtron
  $params = array(
    'name' => 'sustainer_pause_mail_pause',
    'module' => 'sustainer_pause',
    'recipient_callback' => 'sustainer_pause_mail_recipient_callback',
    'tokens' => array(
      'subject' => 'user',
      'html_body' => 'user',
      'text_body' => 'user',
      ),
    'default' => array(
      'from_name' => variable_get('site_name', ''),
      'from_mail' => variable_get('site_mail', ''),
      'subject' => 'You have paused your recurring gift',
      'html_body' => '',
      'text_body' => '',
      ),
  );

  // Generate the mail configuration form
  $form = mailtron_mail_form(array(), $params);
  $form['description'] = array(
    '#value' => 'Configure this form to control the email that is sent out when a user <strong>pauses a recurring donation</strong>.',
    '#weight' => -10,
  );

  return $form;
}

/**
 * Recurring payment restart mail admin form
 */
function sustainer_pause_mail_restart_form($form, &$form_state) {
  // Pull in the required include file to get access to mailtron helper functions.
  module_load_include('inc', 'mailtron', 'mailtron.api');

  // Set up the initial parameters for mailtron
  $params = array(
    'name' => 'sustainer_pause_mail_restart',
    'module' => 'sustainer_pause',
    'recipient_callback' => 'sustainer_pause_mail_recipient_callback',
    'tokens' => array(
      'subject' => 'order',
      'html_body' => 'order',
      'text_body' => 'order',
      ),
    'default' => array(
      'from_name' => variable_get('site_name', ''),
      'from_mail' => variable_get('site_mail', ''),
      'subject' => 'Your recurring gift will resume',
      'html_body' => '',
      'text_body' => '',
      ),
  );

  // Generate the mail configuration form
  $form = mailtron_mail_form(array(), $params);
  $form['description'] = array(
     '#value' => 'Configure this form to control the email that is sent out when a user <strong>restarts a paused recurring donation</strong>.',
     '#weight' => -10,
  );

  return $form;
}

/**
 * Sustainer pause/unpause email
 */
function sustainer_pause_mail_send($form_id, $user) {
  module_load_include('inc', 'mailtron', 'mailtron.api');
  $params = array(
    'name' => $form_id,
    'module' => 'sustainer_pause',
  );
  $mail = mailtron_mail_load($params);
  $mail->recipients = $user->mail;
  mailtron_send_mail($mail);
}

/**
 * Mailtron recipient callback
 */
function sustainer_pause_mail_recipient_callback($mail, $params) {
  // We are manually setting the email in the send function & just passing it through,
  // but we keep this here so the recipient field is hidden on the admin forms.
  return $mail->recipients;
}
