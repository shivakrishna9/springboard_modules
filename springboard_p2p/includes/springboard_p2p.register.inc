<?php

/**
 * @file
 * The register/login page and related functions for P2P.
 */

/**
 * Forms api array for registering users.
 *
 * Uses springboard_p2p_registration_fields to determine weights and
 * enabledness of fields.
 */
function springboard_p2p_user_register_form($form, &$form_state) {
  $form = array();

  $admin = user_access('administer users');

  // Pass access information to the submit handler. Running an access check
  // inside the submit function interferes with form processing and breaks
  // hook_form_alter().
  $form['administer_users'] = array(
    '#type' => 'value',
    '#value' => $admin,
  );

  $form['#user'] = drupal_anonymous_user();
  $form['#user_category'] = 'register';

  $form['#attached']['library'][] = array('system', 'jquery.cookie');
  $form['#attributes']['class'][] = 'user-info-from-cookie';

  // Start with the default user account fields.
  user_account_form($form, $form_state);

  $fields_to_display = springboard_p2p_get_enabled_registration_fields();

  // Attach field widgets.
  $langcode = entity_language('user', $form['#user']);
  field_attach_form('user', $form['#user'], $form, $form_state, $langcode);

  foreach (field_info_instances('user', 'user') as $field_name => $instance) {
    if (isset($fields_to_display[$field_name]['enabled'])) {

      // Set the required.
      $required = isset($fields_to_display[$field_name]['required']) && $fields_to_display[$field_name]['required'];
      springboard_p2p_set_registration_field_required($form, $field_name, $required);

      $form[$field_name]['#weight'] = $fields_to_display[$field_name]['weight'];
    }
    else {
      $form[$field_name]['#access'] = FALSE;
    }
  }

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new account'),
  );

  // Don't show the user the username field.
  $form['account']['name']['#access'] = FALSE;

  // Need to move the mail field outside of account so we can weight it.
  // This gets moved back in our first validate handler so user module can
  // find it.  In our post validate function we move it back in the form.
  $form['mail'] = $form['account']['mail'];
  $form['mail']['#weight'] = $fields_to_display['mail']['weight'];
  unset($form['account']['mail']);

  // User doesn't need to see these.
  $form['account']['roles']['#access'] = FALSE;
  $form['account']['notify']['#access'] = FALSE;
  $form['account']['status']['#access'] = FALSE;

  $form['#validate'][] = 'springboard_p2p_user_register_validate';

  // Remove the validate callback added by user_account_form().
  // We instead do that validation in our own validate handler.
  $new_validate = array();
  foreach ($form['#validate'] as $callback) {
    if ($callback != 'user_account_form_validate') {
      $new_validate[] = $callback;
    }
  }
  $form['#validate'] = $new_validate;

  $form['#submit'][] = 'springboard_p2p_user_register_form_submit';

  springboard_p2p_campaign_id_form_element($form);

  return $form;
}

/**
 * Validation handler for user registration.
 *
 * Validates that the email will be a valid username.
 *
 * Sets the name value so the username will be the email address.
 *
 * Moves the mail element back to where user module expects it.
 *
 * @see user_account_form_validate()
 */
function springboard_p2p_user_register_validate(&$form, &$form_state) {
  $account = $form['#user'];
  $form_state['new_account'] = TRUE;

  // Trim whitespace from mail, to prevent confusing 'e-mail not valid'
  // warnings often caused by cutting and pasting.
  $mail = trim($form_state['values']['mail']);
  form_set_value($form['mail'], $mail, $form_state);

  // Validate the e-mail address.
  if ($error = user_validate_mail($mail)) {
    form_set_error('mail', $error);
  }
  // Validate that the e-mail address will make a valid username.
  elseif ($error = springboard_p2p_validate_email_as_username($mail)) {
    // Generic error message.
    form_set_error('mail', $error);
  }
  // Check for existing accounts.
  else {
    $uid = db_select('users')
      ->fields('users', array('uid'))
      ->condition('uid', $account->uid, '<>')
      ->condition('mail', db_like($mail), 'LIKE')
      ->range(0, 1)
      ->execute()
      ->fetchField();

    if (!$uid) {
      $uid = db_select('users')
        ->fields('users', array('uid'))
        ->condition('uid', $account->uid, '<>')
        ->condition('name', db_like($mail), 'LIKE')
        ->range(0, 1)
        ->execute()
        ->fetchField();
    }

    if ($uid) {
      // Existing user is interested in p2p.
      springboard_p2p_register_user_for_p2p($uid);
      // Format error message dependent on whether the user is logged in.
      if ($GLOBALS['user']->uid) {
        form_set_error('mail', t('The e-mail address %email is already taken.', array('%email' => $mail)));
      }
      else {

        $password_reset_request_url = url('springboard_p2p/password/' . $uid, array(
            'query' => array(
              'p2p_cid' => $form_state['values']['springboard_p2p_campaign_nid'],
            ),
          ));

        form_set_error('mail', t('The e-mail address %email is already registered. <a href="@password">Send me a password reset email.</a>', array('%email' => $mail, '@password' => $password_reset_request_url)));
      }
    }
  }

  // Set the username as the email address.
  $form_state['values']['name'] = $mail;

  // Move mail around so other user validate handlers will work.
  $form['account']['mail'] = $form['mail'];
  unset($form['mail']);

  user_register_validate($form, $form_state);

  $form['mail'] = $form['account']['mail'];
  unset($form['account']['mail']);
}

/**
 * Submit handler for the user register form.
 *
 * Registers the user for p2p.
 */
function springboard_p2p_user_register_form_submit($form, &$form_state) {
  user_register_submit($form, $form_state);

  $account = NULL;
  if (!empty($form['#user'])) {
    $account = $form['#user'];
  }
  elseif (!empty($form_state['user'])) {
    $account = $form_state['user'];
  }

  if (!is_null($account)) {
    // Reload user.
    $account = user_load($account->uid);

    $event = new SpringboardP2pEvents();
    $event->userRegistersForP2P($account);
  }
}

/**
 * Forms api array for the existing user login form.
 */
function springboard_p2p_existing_user_login_form($form, &$form_state) {
  $form = array();

  // Display login form:
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#size' => 60,
    '#maxlength' => USERNAME_MAX_LENGTH,
    '#required' => TRUE,
  );

  $form['name']['#description'] = t('Enter your @s username.', array('@s' => variable_get('site_name', 'Drupal')));

  $form['pass'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#description' => t('Enter the password that accompanies your username.'),
    '#required' => TRUE,
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Log in'),
  );

  $form['springboard_p2p'] = array(
    '#type' => 'value',
    '#value' => 1,
  );

  $form['#validate'] = user_login_default_validators();
  $form['#submit'][] = 'user_login_submit';

  springboard_p2p_campaign_id_form_element($form);

  return $form;
}
