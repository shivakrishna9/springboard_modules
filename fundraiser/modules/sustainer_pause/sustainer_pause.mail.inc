<?php
/**
* sustainer_pause pause mail admin form
*/
function sustainer_pause_mail_pause_form(&$form) {
  // Pull in the required include file to get access to mailtron helper functions.
  module_load_include('inc', 'mailtron', 'mailtron.api');
  
  // Set up the initial parameters for mailtron
  $params = array(
    'name' => 'sustainer_pause_mail_pause',
    'module' => 'sustainer_pause',
    'recipient_callback' => 'sustainer_pause_mail_recipient_callback',
    'send_callback' => 'sustainer_pause_mail_send_callback',
    'tokens' => array(
      'subject' => 'user', 
      'html_body' => 'user', 
      'text_body' => 'user', 
      ),
    'default' => array (
      'from_name' => variable_get('site_name', ''), 
      'from_mail' => variable_get('site_mail', ''), 
      'subject' => 'You have paused your recurring gift',
      'html_body' => '',
      'text_body' => '', 
      ),
  );
  
  // Generate the mail configuration form
  $form['pause'] = mailtron_mail_form(array(), $params);
  
  return $form;
}

/**
* sustainer_pause restart mail admin form
*/
function sustainer_pause_mail_restart_form(&$form) {
  // Pull in the required include file to get access to mailtron helper functions.
  module_load_include('inc', 'mailtron', 'mailtron.api');
  
  // Set up the initial parameters for mailtron
  $params = array(
    'name' => 'sustainer_pause_mail_restart',
    'module' => 'sustainer_pause',
    'recipient_callback' => 'sustainer_pause_mail_recipient_callback',
    'send_callback' => 'sustainer_pause_mail_send_callback',
    'tokens' => array(
      'subject' => 'user', 
      'html_body' => 'user', 
      'text_body' => 'user', 
      ),
    'default' => array (
      'from_name' => variable_get('site_name', ''), 
      'from_mail' => variable_get('site_mail', ''), 
      'subject' => 'Your recurring gift will resume',
      'html_body' => '',
      'text_body' => '', 
      ),
  );

  // Generate the mail configuration form
  $form['restart'] = mailtron_mail_form(array(), $params);
  
  return $form;
}