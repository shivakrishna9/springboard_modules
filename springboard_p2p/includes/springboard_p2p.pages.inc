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

  // This validate handler fills in the name value with the email address,
  // and so needs to happen before the user module's validation.
  $form['#validate'][] = 'springboard_p2p_user_register_validate';

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

  $form['#validate'][] = 'user_register_validate';

  // Add the final user registration form submit handler.
  $form['#submit'][] = 'user_register_submit';

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

  $form['#validate'][] = 'springboard_p2p_user_register_post_validate';

  // Remove the validate callback added by user_account_form().
  // We instead do that validation in our own validate handler.
  $new_validate = array();
  foreach ($form['#validate'] as $callback) {
    if ($callback != 'user_account_form_validate') {
      $new_validate[] = $callback;
    }
  }
  $form['#validate'] = $new_validate;

  springboard_p2p_campaign_id_form_element($form);

  $form['#submit'][] = 'springboard_p2p_register_for_p2p_submit';

  return $form;
}

/**
 * Submit handler for the user register form.
 *
 * Registers the user for p2p.
 */
function springboard_p2p_register_for_p2p_submit($form, &$form_state) {
  dpm($form_state);
  springboard_p2p_register_user_for_p2p($form_state['user']->uid);
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
  // Check if mail is already registerd.
  elseif ((bool) db_select('users')->fields('users', array('uid'))->condition('uid', $account->uid, '<>')->condition('mail', db_like($mail), 'LIKE')->range(0, 1)->execute()->fetchField()) {
    // Format error message dependent on whether the user is logged in or not.
    if ($GLOBALS['user']->uid) {
      form_set_error('mail', t('The e-mail address %email is already taken.', array('%email' => $mail)));
    }
    else {
      form_set_error('mail', t('The e-mail address %email is already registered. <a href="@password">Have you forgotten your password?</a>', array('%email' => $mail, '@password' => url('user/password'))));
    }
  }
  // Check if mail exists as a username.
  elseif ((bool) db_select('users')->fields('users', array('uid'))->condition('uid', $account->uid, '<>')->condition('name', db_like($mail), 'LIKE')->range(0, 1)->execute()->fetchField()) {
    form_set_error('mail', t('The e-mail address %mail is already registered.', array('%mail' => $mail)));
  }

  // Set the username as the email address.
  $form_state['values']['name'] = $mail;

  // Move mail around so other user validate handlers will work.
  $form['account']['mail'] = $form['mail'];
  unset($form['mail']);
}

/**
 * Last validate handler for the p2p register form.
 *
 * Moves the mail element back to where we want it in the form.
 */
function springboard_p2p_user_register_post_validate(&$form, &$form_state) {
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
 * Implements hook_mail().
 */
function springboard_p2p_mail($key, &$message, $params) {
  switch ($key) {
    case 'new_user':
      springboard_p2p_new_user_mail($message, $params);
      break;

    case 'New_user_private_campaign':
      springboard_p2p_new_user_private_campaign_mail($message, $params);
      break;

    case 'user_requests_approval':
      springboard_p2p_user_requests_approval_mail($message, $params);
      break;

    case 'admin_approval_request':
      springboard_p2p_admin_approval_request_mail($message, $params);
      break;

  }
}

/**
 * Mail template to send to a user who registers for P2P.
 *
 * @see springboard_p2p_mail()
 */
function springboard_p2p_new_user_mail(&$message, $params) {
  $options = array(
    'langcode' => $message['language']->language,
  );

  $message['subject'] = t('Welcome to P2P', $options);
  $message['body'][] = t('Thank you for creating a P2P account.', $options);
}

/**
 * Mail template for a user registering for P2P coming from a private campaign.
 *
 * @see springboard_p2p_mail()
 */
function springboard_p2p_new_user_private_campaign_mail(&$message, $params) {
  $options = array(
    'langcode' => $message['language']->language,
  );

  $message['subject'] = t('Welcome to P2P', $options);
  $message['body'][] = t('Thank you for creating a P2P account.  We will be in touch when you are approved.', $options);
}

/**
 * Mail template when a P2P user requests approval from a private campaign.
 *
 * @see springboard_p2p_mail()
 */
function springboard_p2p_user_requests_approval_mail(&$message, $params) {
  $options = array(
    'langcode' => $message['language']->language,
  );

  $message['subject'] = t('Next steps', $options);
  $message['body'][] = t('Thanks for showing interest in this campaign.  We will be in touch soon.', $options);
}

/**
 * Mail template to an admin when a P2P user request private campaign approval.
 *
 * @see springboard_p2p_mail()
 */
function springboard_p2p_admin_approval_request_mail(&$message, $params) {
  $options = array(
    'langcode' => $message['language']->language,
  );

  $message['subject'] = t('User requests approval', $options);
  $message['body'][] = t('A user has requested approval for a campaign.', $options);
}
