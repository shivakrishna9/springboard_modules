<?php
/**
 * @file
 * fundraiser_upsell.mail.inc - Mail forms and functions
 */

/**
 * Admin form for the thank you email functions
 */
function fundraiser_upsell_admin_thank_you_settings() {
  // Get some default values
  $form = array();
  $site_mail = variable_get('site_mail', '');
  $site_name = variable_get('site_name', '');
  $thank_you_content = variable_get('fundraiser_upsell_thank_you_content', '');
  $recipient_description = t("This is used to optionally send a test. The final email will be sent to the donation user. For more than one address, addresses should be separated by a comma. Example: test1@example.com,test2@example.com");
  // Add the thank you content fields
  $form['fundraiser_upsell_thank_you_content'] = array(
    '#title' => t('Default content for Upsell Thank You'),
    '#type' => 'textarea',
    '#default_value' => $thank_you_content,
    '#weight' => -20,
  );
  $form['fundraiser_upsell_thank_you_content_tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -19,
  );
  $form['fundraiser_upsell_thank_you_content_tokens']['help'] = array(
    '#value' => theme('token_help', 'upsell'),
  );
  $form['email_title'] = array(
    '#type' => 'item',
    '#title' => t('E-mail settings'),
    '#weight' => -10,
  );
  // Pull in the required include file to get access to email_wrappers helper functions.
  module_load_include('inc', 'email_wrappers', 'email_wrappers.api');
  module_load_include('module', 'email_wrappers');
  // Set up some initial parameters for our new mail configuration form.
  $params = array(
    'name' => 'email_admin_form',
    'module' => 'fundraiser_upsell',
    'delta' => 'thank_you_settings',
    'send_callback' => 'fundraiser_upsell_send_callback',
  );
  // Generate the mail configuration form.
  $email_form = email_wrappers_mail_form(array(), $params);
  // Add the email form
  $form['email'] = $email_form;
  $form['email']['#type'] = 'vertical_tabs';
  $form['email']['#weight'] = -9;
  // Modify the headers section
  $form['email']['headers']['#title'] = "Mail headers";
  $form['email']['headers']['#weight'] = -5;
  $form['email']['headers']['from_name']['#default_value'] = $site_name;
  $form['email']['headers']['from_mail']['#default_value'] = $site_mail;
  $form['email']['headers']['recipients']['#description'] = $recipient_description;
  $form['email']['headers']['recipients']['#weight'] = 1;
  $form['email']['headers']['recipients']['#default_value'] = $site_mail;
  // Add an additional button we can use to send a test mail.
  $form['email']['headers']['fundraiser_upsell_send'] = array(
    '#type' => 'button',
    '#value' => 'Send test email',
    '#executes_submit_callback' => TRUE,
    '#submit' => array('fundraiser_upsell_send_test'),
    '#weight' => 2,
  );
  // Modify the message section
  $form['email']['message']['#title'] = "Mail body";
  $form['email']['message']['#weight'] = -6;
  if (empty($form['email']['message']['subject']['#default_value'])) {
    $form['email']['message']['subject']['#default_value'] = "Thank you for becoming a sustaining partner.";
  }
  // Move the button and validation
  $form['save'] = $form['email']['save'];
  $form['#validate'] = $form['email']['#validate'];
  $form['#submit'] = $form['email']['#submit'];
  unset($form['email']['save']);
  unset($form['email']['#validate']);
  unset($form['email']['#submit']);

  return $form;
}

/**
 * Button submit callback, triggers mail send when clicked.
 */
function fundraiser_upsell_send_test($form, $form_state) {
  module_load_include('inc', 'email_wrappers', 'email_wrappers.api');
  $params = array(
    'module' => 'fundraiser_upsell',
  );
  $mail = email_wrappers_mail_load($params);
  email_wrappers_send_mail($mail, $params);
}

/**
 * Send the thank you email after the upsell.
 */
function fundraiser_upsell_send_thank_you($donation) {
  module_load_include('inc', 'email_wrappers', 'email_wrappers.api');
  $params = array(
    'module' => 'fundraiser_upsell',
  );
  // Load the email wrappers options
  $mail = email_wrappers_mail_load($params);
  // Modify the "to:" information to match the donation
  $mail['to_mail'] = $donation->donation['mail'];
  $mail['to_name'] = $donation->donation['first_name'] . ' ' . $donation->donation['last_name'];
  email_wrappers_send_mail($mail, $params);
}

/**
 * Invoked when a mail is sent.
 */
function fundraiser_upsell_send_callback($mail, $params = array()) {
  drupal_set_message('send callback invoked');
  drupal_set_message('Your mail has been sent!');
}
