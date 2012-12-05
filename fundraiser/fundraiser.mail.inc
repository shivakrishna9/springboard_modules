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
    'send_callback' => 'fundraiser_mail_send_callback',
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
    'send_callback' => 'fundraiser_mail_send_callback',
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

//TODO: recipient callbacks

//TODO: mail send callbacks