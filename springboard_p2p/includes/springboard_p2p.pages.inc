<?php
/**
 * @file
 * User facing page callbacks and forms for Springboard P2P.
 */

/**
 * Page callback for the user registration page.
 *
 * @see user_register_form()
 */
function springboard_p2p_register_page($form, &$form_state) {
  $form = array();

  $campaign_id = springboard_p2p_get_campaign_id_from_request();
  if (!empty($campaign_id)) {
    $campaign = node_load($campaign_id);
    if ($campaign->type == 'p2p_campaign' && isset($campaign->field_p2p_form_header) && !empty($campaign->field_p2p_form_header[$campaign->language])) {

      $display = array(
        'settings' => array(
          'image_link' => 'content',
          'image_style' => 'large',
        ),
      );

      $form['header'] = image_field_formatter_view(
        'p2p_campaign',
        $campaign,
        array(),
        array(),
        $campaign->language,
        $campaign->field_p2p_form_header[$campaign->language],
        $display
      );
    }

  }

  if (module_exists('fboauth')) {
    $form['social_login'] = array(
      '#type' => 'fieldset',
      '#title' => 'Log in with Facebook',
      'link' => array(
        '#markup' => fboauth_action_display('connect', 'springboard_p2p/fb?campaign=' . $campaign_id),
      ),
    );
  }

  $form['new_account'] = array(
    '#type' => 'fieldset',
    '#title' => 'New account',
    'form' => drupal_get_form('springboard_p2p_user_register_form'),
  );

  $form['login'] = array(
    '#type' => 'fieldset',
    '#title' => 'Log in',
    'form' => drupal_get_form('springboard_p2p_existing_user_login_form'),
  );

  return $form;
}

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
 * Submit handler for the user register form.
 *
 * Registers the user for p2p.
 */
function springboard_p2p_user_register_form_submit($form, &$form_state) {
  user_register_submit($form, $form_state);

  $uid = NULL;
  if (!empty($form['#user']->uid)) {
    $uid = $form['#user']->uid;
  }
  elseif (!empty($form_state['user']->uid)) {
    $uid = $form_state['user']->uid;
  }

  if ($uid) {
    springboard_p2p_register_user_for_p2p($uid);
    $account = user_load($uid);
    $params = array(
      'account' => $account,
    );
    $to = $account->mail;
    $language = user_preferred_language($account);
    drupal_mail('springboard_p2p', 'new_user', $to, $language, $params);
  }
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
        form_set_error('mail', t('The e-mail address %email is already registered. <a href="@password">Send me a password reset email.</a>', array('%email' => $mail, '@password' => url('springboard_p2p/password/' . $uid))));
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

/**
 * Page callback for the personal dashboard.
 *
 * @param object $user
 *   The user that matches the uid in the path.
 *
 * @return array
 *   Render array.
 */
function springboard_p2p_personal_dashboard($user) {
  return array(
    'comingsoon' => array(
      '#markup' => '<h2>Personal dashboard for ' . $user->name . ' coming soon</h2>',
    ),
  );
}

/**
 * Verify the email address will be a valid username.
 *
 * Returns email specific error messages.
 *
 * @param string $mail
 *   The email address to check.
 *
 * @return string|NULL
 *   The email specific error, or NULL for no errors.
 *
 * @see user_validate_name()
 */
function springboard_p2p_validate_email_as_username($mail) {
  if (strpos($mail, '  ') !== FALSE) {
    return t('The e-mail address cannot contain multiple spaces in a row.');
  }
  if (preg_match('/[^\x{80}-\x{F7} a-z0-9@_.\'-]/i', $mail)) {
    return t('The e-mail address contains an illegal character.');
  }
  // Non-printable ISO-8859-1 + NBSP.
  if (preg_match('/[\x{80}-\x{A0}' .
    // Soft-hyphen.
    '\x{AD}' .
    // Various space characters.
    '\x{2000}-\x{200F}' .
    // Bidirectional text overrides.
    '\x{2028}-\x{202F}' .
    // Various text hinting characters.
    '\x{205F}-\x{206F}' .
    // Byte order mark.
    '\x{FEFF}' .
    // Full-width latin.
    '\x{FF01}-\x{FF60}' .
    // Replacement characters.
    '\x{FFF9}-\x{FFFD}' .
    // NULL byte and control characters.
    '\x{0}-\x{1F}]/u',
    $mail)) {
    return t('The e-mail address contains an illegal character.');
  }
  if (drupal_strlen($mail) > USERNAME_MAX_LENGTH) {
    return t('The e-mail address %name is too long: it must be %max characters or less.', array('%name' => $mail, '%max' => USERNAME_MAX_LENGTH));
  }

}

/**
 * Page where user goes after connecting with facebook oauth.
 *
 * Checks if the user is already registered for p2p and if not, checks if
 * he has filled out all required fields.  If not, goes to the complete profile
 * page.
 *
 * If the user is already registered, go to the normal destination.
 */
function springboard_p2p_fb_page() {
  global $user;
  $uid = $user->uid;

  if (!springboard_p2p_user_is_registered_for_p2p($uid)) {
    springboard_p2p_register_user_for_p2p($uid);

    if (springboard_p2p_complete_profile_user_has_empty_required_fields($user)) {
      $redirect = springboard_p2p_complete_profile_get_redirect();
      drupal_goto($redirect['path'], $redirect['options']);
    }
  }

  // Faking the form submission so the approval status will get saved
  // if needed and we can find out where to go next.
  $form = array();
  $form_state = array(
    'values' => array(
      'springboard_p2p_campaign_nid' => springboard_p2p_get_campaign_id_from_request(),
    ),
    'uid' => $uid,
  );
  springboard_p2p_set_redirect($form, $form_state);

  drupal_goto($form_state['redirect']);

}

/**
 * Page placeholder for creating a new personal campaign under a campaign.
 */
function springboard_p2p_create_personal_campaign($campaign) {
  $output = array(
    'message' => array(
      '#markup' => 'Placeholder page for creating a new personal campaign. The parent campaign is ' . l($campaign->title, 'node/' . $campaign->nid),
    ),
  );

  return $output;
}

/**
 * Page callback for after the user registers but before the password is set.
 */
function springboard_p2p_welcome_page() {
  $output = array(
    '#markup' => 'Welcome. Your account must be verifed.  See the email for further instructions.',
  );

  return $output;
}

/**
 * Form callback for the custom password reset page.
 */
function springboard_p2p_set_password_form($form, &$form_state, $uid, $timestamp, $hashed_pass) {

  global $user;

  $form = array();
  $show_password_form = FALSE;

  // When processing the one-time login link, we have to make sure that a user
  // isn't already logged in.
  if ($user->uid) {
    // The existing user is already logged in.
    if ($user->uid == $uid) {
      drupal_set_message(t('You are logged in as %user.', array('%user' => $user->name)));
      $account = $user;
      $show_password_form = TRUE;
    }
    // A different user is already logged in on the computer.
    else {
      $reset_link_account = user_load($uid);
      if (!empty($reset_link_account)) {
        $form['invalid_link'] = array(
          '#markup' => t('Another user (%other_user) is already logged into the site on this computer, but you tried to use a one-time link for user %resetting_user. Please <a href="!logout">logout</a> and try using the link again.',
            array(
              '%other_user' => $user->name,
              '%resetting_user' => $reset_link_account->name,
              '!logout' => url('user/logout'),
            )
          ),
        );
      }
      else {
        // Invalid one-time link specifies an unknown user.
        $form['invalid_link'] = array(
          '#markup' => t('The one-time login link you clicked is invalid.'),
        );
      }

    }

  }
  else {
    // Time out, in seconds, until login URL expires. Defaults to 24 hours =
    // 86400 seconds.
    $timeout = variable_get('user_password_reset_timeout', 86400);
    $current = REQUEST_TIME;
    // Some redundant checks for extra security ?
    $users = user_load_multiple(array($uid), array('status' => '1'));
    if ($timestamp <= $current && $account = reset($users)) {
      // No time out for first time login.
      if ($account->login && $current - $timestamp > $timeout) {
        $form['expired_link'] = array(
          '#markup' => t('You have tried to use a one-time login link that has expired. <a href="@password">Send a new password reset email.</a>', array('@password' => url('springboard_p2p/password/' . $uid))),
        );

      }
      elseif ($account->uid && $timestamp >= $account->login && $timestamp <= $current && $hashed_pass == user_pass_rehash($account->pass, $timestamp, $account->login)) {

        // Set the new user.
        $user = $account;
        // user_login_finalize() also updates the login timestamp of the
        // user, which invalidates further use of the one-time login link.
        user_login_finalize();
        watchdog('user', 'User %name used one-time login link at time %timestamp.', array('%name' => $account->name, '%timestamp' => $timestamp));
        drupal_set_message(t('You have just used your one-time login link. It is no longer necessary to use this link to log in. Please set your password.'));

        $show_password_form = TRUE;

      }
      else {

        $form['invalid_link'] = array(
          '#markup' => t('You have tried to use a one-time login link that has either been used or is no longer valid. <a href="@password">Send a new password reset email.</a>', array('@password' => url('springboard_p2p/password/' . $uid))),
        );

      }
    }
    else {
      // Deny access, no more clues.
      // Everything will be in the watchdog's URL for the administrator to
      // check.
      drupal_access_denied();
      drupal_exit();
    }
  }

  if ($show_password_form) {
    $form['#user'] = $account;
    $form_state['user'] = $account;

    // Account information.
    $form['account'] = array(
      '#type'   => 'container',
      '#weight' => -10,
    );

    $form['account']['pass'] = array(
      '#type' => 'password_confirm',
      '#size' => 25,
      '#description' => t('Provide a password for the new account in both fields.'),
      '#required' => TRUE,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Set password'));

    $form['#submit'][] = 'springboard_p2p_set_password_form_submit';
  }

  return $form;
}

/**
 * Submit handler for the set password form.
 */
function springboard_p2p_set_password_form_submit($form, &$form_state) {

  $account = $form_state['user'];
  $edit = array(
    'pass' => $form_state['values']['pass'],
  );

  user_save($account, $edit);

  $form_state['redirect'] = 'springboard_p2p/dashboard/' . $account->uid;
}

/**
 * Page callback to request a password reset email.
 */
function springboard_p2p_request_password_reset_page($account) {
  if (!isset($account->uid) || !springboard_p2p_user_is_registered_for_p2p($account->uid)) {
    return MENU_NOT_FOUND;
  }

  $params = array(
    'account' => $account,
  );
  $to = $account->mail;
  $language = user_preferred_language($account);
  drupal_mail('springboard_p2p', 'password_reset', $to, $language, $params);

  $output = array();
  $output['message'] = array(
    '#markup' => 'A password reset email has been sent',
  );

  return $output;
}
