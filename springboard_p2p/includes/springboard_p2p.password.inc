<?php

/**
 * @file
 * Functions and pages related to resetting a password in P2P.
 */

/**
 * Form callback to request a password reset email.
 */
function springboard_p2p_request_password_reset_form($form, &$form_state) {
  $form = array();

  if (isset($_GET['p2p_cid']) && is_numeric($_GET['p2p_cid'])) {
    $cid = $_GET['p2p_cid'];
  }
  else {
    $cid = NULL;
  }

  $uid = arg(2);
  if ($uid) {
    $account = user_load($uid);

    if (!isset($account->uid) || !springboard_p2p_user_is_registered_for_p2p($account->uid)) {
      return MENU_NOT_FOUND;
    }

    $event = new SpringboardP2pEvents();
    $campaign = node_load($cid);
    // Convert any FALSE value to NULL.  For rules.
    $campaign = $campaign ? $campaign : NULL;
    $event->userRequestsPasswordReset($account, $campaign);
    drupal_set_message(t('Further instructions have been sent to your e-mail address.'));

    $options = array();
    if (!is_null($cid)) {
      $options['query'] = array(
        'p2p_cid' => $cid,
      );
    }

    drupal_goto('springboard_p2p/register', $options);

  }
  else {
    global $user;

    $form['name'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail address'),
      '#size' => 60,
      '#maxlength' => max(USERNAME_MAX_LENGTH, EMAIL_MAX_LENGTH),
      '#required' => TRUE,
      '#default_value' => isset($_GET['name']) ? $_GET['name'] : '',
    );

    // Allow logged in users to request this also.
    if ($user->uid > 0) {
      $form['name']['#type'] = 'value';
      $form['name']['#value'] = $user->mail;
      $form['mail'] = array(
        '#prefix' => '<p>',
        '#markup' => t('Password reset instructions will be mailed to %email. You must log out to use the password reset link in the e-mail.', array('%email' => $user->mail)),
        '#suffix' => '</p>',
      );
    }
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('E-mail new password'));

  }

  $form['p2p_cid'] = array(
    '#type' => 'value',
    '#value' => $cid,
  );

  return $form;
}

/**
 * Form validation for requesting a new password.
 */
function springboard_p2p_request_password_reset_form_validate($form, &$form_state) {
  $name = trim($form_state['values']['name']);
  // Try to load by email.
  $users = user_load_multiple(array(), array('mail' => $name, 'status' => '1'));
  $account = reset($users);
  if (!$account) {
    // No success, try to load by name.
    $users = user_load_multiple(array(), array('name' => $name, 'status' => '1'));
    $account = reset($users);
  }

  if (isset($account->uid)) {
    form_set_value(array('#parents' => array('account')), $account, $form_state);
  }
  else {
    form_set_error('name', t('Sorry, %name is not recognized as a registered e-mail address.', array('%name' => $name)));
  }
}

/**
 * Form submission for requesting a new password.
 */
function springboard_p2p_request_password_reset_form_submit($form, &$form_state) {
  $account = $form_state['values']['account'];
  $cid = $form_state['values']['p2p_cid'];

  $url_options = array();
  if (!empty($cid)) {
    $url_options['query'] = array(
      'p2p_cid' => $cid,
    );
  }

  $event = new SpringboardP2pEvents();
  $campaign = node_load($cid);
  // Convert any FALSE value to NULL.  For rules.
  $campaign = $campaign ? $campaign : NULL;
  $event->userRequestsPasswordReset($account, $campaign);
  drupal_set_message(t('Further instructions have been sent to your e-mail address.'));

  $form_state['redirect'] = array('springboard_p2p/register', $url_options);
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

  if ($show_password_form && isset($account)) {
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

    springboard_p2p_campaign_id_form_element($form, $form_state);
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
}
