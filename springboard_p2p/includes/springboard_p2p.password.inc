<?php

/**
 * @file
 * Functions and pages related to resetting a password in P2P.
 */

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

  // @todo Change this to a Rules event so the email can be configured?
  drupal_mail('springboard_p2p', 'password_reset', $to, $language, $params);

  $output = array();
  $output['message'] = array(
    '#markup' => 'A password reset email has been sent',
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

  $form_state['redirect'] = 'user/' . $account->uid . '/personal-campaigns';
}
