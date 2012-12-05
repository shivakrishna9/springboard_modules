<?php
/**
* Fundraiser welcome mail admin form
*/
function fundraiser_mail_welcome_form(&$form) {
  // Pull in the required include file to get access to mailtron helper functions.
  module_load_include('inc', 'mailtron', 'mailtron.api');
  
  // Set up the initial parameters for mailtron
  $params = array(
    'name' => 'fundraiser_mail_welcome',
    'module' => 'fundraiser',
    'recipient_callback' => 'fundraiser_mail_recipient_callback',
    'default' => array (
      'from_name' => variable_get('site_name', ''), 
      'from_mail' => variable_get('site_mail', ''), 
      'subject' => 'Thank you for your recurring gift',
      'html_body' => '',
      'text_body' => '', 
      ),
    'tokens' => array(
      'subject' => 'order',
      'html_body' => 'order', 
      'text_body' => 'order', 
      ),
    'default' => array (
      'from_name' => variable_get('site_name', ''), 
      'from_mail' => variable_get('site_mail', ''), 
      'subject' => 'Thank you for your recurring gift',
      'html_body' => '',
      'text_body' => '', 
      ),
    );
  
  // Generate the mail configuration form
  $form = mailtron_mail_form(array(), $params);
  $form['header'] = array(
    '#value' => 'Configure this form to control the email that is sent out when a user <strong>starts a recurring donation</strong>.',
    '#weight' => -10, 
  );
  
  return $form;
}

/**
* Fundraiser end mail admin form
*/
function fundraiser_mail_end_form(&$form) {
  // Pull in the required include file to get access to mailtron helper functions.
  module_load_include('inc', 'mailtron', 'mailtron.api');
  
  // Set up the initial parameters for mailtron
  $params = array(
    'name' => 'fundraiser_mail_end',
    'module' => 'fundraiser',
    'recipient_callback' => 'fundraiser_mail_recipient_callback',
    'tokens' => array(
      'subject' => 'order', 
      'html_body' => 'order', 
      'text_body' => 'order', 
      ),
    'default' => array (
      'from_name' => variable_get('site_name', ''), 
      'from_mail' => variable_get('site_mail', ''), 
      'subject' => 'Your recurring gift has ended',
      'html_body' => '',
      'text_body' => '', 
      ),
  );

  // Generate the mail configuration form
  $form = mailtron_mail_form(array(), $params);
  $form['desccription'] = array(
    '#value' => 'Configure this form to control the email that is sent out when a <strong>recurring donation ends</strong>.',
    '#weight' => -10, 
  );

  return $form;
}


/**
 * Fundraiser welcome/end email
 */
function fundraiser_mail_send($form_id, $user) {
  module_load_include('inc', 'mailtron', 'mailtron.api');
  $params = array(
    'name' => $form_id,
    'module' => 'fundraiser',
  );
  $mail = mailtron_mail_load($params);
  $mail->recipients = $user->mail;
  mailtron_send_mail($mail);
}

/**
 * Mailtron recipient callback
 */
function fundraiser_mail_recipient_callback($mail, $params) {
  // We are manually setting the email in the send function & just passing it through,
  // but we keep this here so the recipient field is hidden on the admin forms.
  return $mail->recipients;
}
