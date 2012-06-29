<?php

/**
 * Fundraiser confirmation features. Provided, but expected to be over ridden by glue
 * modules that handle submission forms. Webform for example already has it's own.
 */

/**
 * Menu access callback, check for donation type and node access.
 */
function fundraiser_confirmations_perm($perm, $node) {
  if (fundraiser_is_donation_type($node->type) && node_access($perm, $node)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Menu callback, display fundraiser confirmation page.
 */
function fundraiser_donation_confirmation_page($node, $did) {
  drupal_set_title(check_plain($node->confirmation_page_title));
  if (empty($output)) {
    $message = '';
    $donation = fundraiser_donation_get_donation($did);
    if (!empty($donation)) {
      if (!empty($donation->node->confirmation_page_body)) {
        $message = _fundraiser_replace_tokens($donation->node->confirmation_page_body, $donation);
      }
    }
    $output = theme(
      array('fundraiser_confirmation_' . $node->nid, 'fundraiser_confirmation'),
      array('node' => $node,'message' => $message)
    );
  }
  return $output;
}

/**
 * Form for fundraiser confirmation page settings.
 */
function fundraiser_confirmation_settings_form($form, &$form_state, $node) {
  // Additional values have been loaded on node_load, so are available here.
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  $form['confirmation_page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Confirmation page settings'),
    '#collapsible' => TRUE,
  );
  $form['confirmation_page']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page title'),
    '#default_value' => isset($node->confirmation_page_title) ? $node->confirmation_page_title : '',
  );
  $body_format = $node->confirmation_page_format ? $node->confirmation_page_format : NULL;
  $form['confirmation_page']['body'] = array(
    '#type' => 'text_format',
    '#title' => t('Confirmation message'),
    '#default_value' => isset($node->confirmation_page_body) ? $node->confirmation_page_body : '',
    '#format' => isset($body_format) ? $body_format : NULL,
  );
  $form['confirmation_page']['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available confirmation message tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['confirmation_page']['tokens']['help'] = array(
    // TODO follow up on this to make sure that the tokens we are using are the right ones.
    // Currently displays both regular tokens and webform tokens.
    '#markup' => theme('token_tree', array('token_types' => array('node'))) . theme('webform_token_help', array()),
  );
  // Redirection settings.
  if (isset($node->redirect_url) && strpos($node->redirect_url, '<') === 0) {
    $redirect = trim($node->redirect_url, '<>');
    // Redirection is set to front page.
    if ($redirect == 'front') {
      $redirect = 'url';
      $redirect_url = $node->redirect_url;
    }
    else {
      $redirect_url = '';
    }
  }
  else {
    $redirect = 'url';
    $redirect_url = isset($node->redirect_url) ? $node->redirect_url : '';
  }
  $form['confirmation_page']['redirection'] = array(
    '#type' => 'item',
    '#title' => t('Redirection location'),
    '#theme' => 'webform_advanced_redirection_form',
    '#description' => t('Choose where to redirect the user upon successful submission.'),
  );
  $form['confirmation_page']['redirection']['redirect'] = array(
    '#type' => 'radios',
    '#default_value' => $redirect,
    '#options' => array(
      'confirmation' => t('Confirmation page'),
      'url' => t('Custom URL'),
    ),
  );
  $form['confirmation_page']['redirection']['redirect_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect URL'),
    '#description' => t('URL to redirect the user to upon successful submission.'),
    '#default_value' => $redirect_url,
    '#maxlength' => 255,
  );
  $form['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('Confirmation email settings'),
    '#collapsible' => TRUE,
  );
  $form['email']['receipt_email_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Receipt Email Address'),
    '#description' => t('The email address that the receipt email will come from.'),
    '#weight' => -4.6,
    '#maxlength' => 255,
    '#default_value' => isset($node->receipt_email_address) ? $node->receipt_email_address : '',
    '#required' => TRUE,
  );
  $form['email']['receipt_email_from'] = array(
    '#type' => 'textfield',
    '#title' => t('Receipt Email From'),
    '#description' => t('The name that is associated with the receipt email address.'),
    '#weight' => -4.5,
    '#default_value' => isset($node->receipt_email_from) ? $node->receipt_email_from : '',
    '#required' => TRUE,
  );
  $form['email']['receipt_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Receipt Email Subject'),
    '#description' => t('The subject of the donation receipt email.'),
    '#weight' => -4.4,
    '#default_value' => isset($node->receipt_email_subject) ? $node->receipt_email_subject : '',
    '#required' => TRUE,
  );
  $form['email']['receipt_email_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Confirmation Email Message'),
    '#weight' => -4.3,
    '#default_value' => isset($node->receipt_email_message) ? $node->receipt_email_message : '',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#weight' => 300,
  );
  return $form;
}

/**
 * Validation function for fundraiser_webform_confirmation_settings_form().
 */
function fundraiser_confirmation_settings_form_validate($form, &$form_state) {
  // Validate the email address.
  if (!_fundraiser_validate_email($form_state['values']['receipt_email_address'])) {
    form_set_error('receipt_email_address', t('Receipt Email Address must be a valid email address.'));
  }
  // Ensure the entered redirect URL is valid.
  if ($form_state['values']['redirect'] == 'url') {
    $redirect_url = trim($form_state['values']['redirect_url']);
    if (empty($redirect_url)) {
      form_error($form['confirmation_page']['redirection']['redirect_url'], t('A valid URL is required for custom redirection.'));
    }
    elseif (strpos($redirect_url, 'http') === 0 && !valid_url($redirect_url, TRUE)) {
      form_error($form['confirmation_page']['redirection']['redirect_url'], t('The entered URL is not a valid address.'));
    }
    else {
      form_set_value($form['confirmation_page']['redirection']['redirect_url'], $redirect_url, $form_state);
    }
  }
  elseif ($form_state['values']['redirect'] == 'confirmation') {
    form_set_value($form['confirmation_page']['redirection']['redirect_url'], '<confirmation>', $form_state);
  }
  else {
    form_set_value($form['confirmation_page']['redirection']['redirect_url'], '<confirmation>', $form_state);
  }
}

/**
 * Submit function for fundraiser_webform_confirmation_settings_form().
 */
function fundraiser_confirmation_settings_form_submit($form, &$form_state) {
  // Get the fundraiser and all the node information that comes with it.
  $fundraiser = node_load($form_state['values']['nid']);
  // Update the fundraiser with the submitted material.
  $fundraiser->confirmation_page_title = check_plain($form_state['values']['title']);
  $fundraiser->confirmation_page_body = $form_state['values']['body']['value'];
  $fundraiser->confirmation_page_format = $form_state['values']['body']['format'];
  $fundraiser->receipt_email_from = check_plain($form_state['values']['receipt_email_from']);
  $fundraiser->receipt_email_address = $form_state['values']['receipt_email_address'];
  $fundraiser->receipt_email_subject = check_plain($form_state['values']['receipt_email_subject']);
  $fundraiser->receipt_email_message = $form_state['values']['receipt_email_message'];
  $fundraiser->redirect_url = $form_state['values']['redirect_url'];
  // Save it.
  _fundraiser_update_fundraiser($fundraiser);
}


/**
 * Submit handler, handles the final destination redirect after donation completed.
 */
function fundraiser_donation_confirmation($form, &$form_state) {
  // Check if we need to rebuild before we redirect anywhere.
  if ($form_state['rebuild']) {
    return;
  }
  // Everything worked, redirect to the confirmation page as the last submit handler.
  $node = $form['#node'];
  $redirect_url = trim($node->redirect_url);
  $did = $form_state['#donation']->did;

  // Prepare the correct redirect path.
  $redirect_url = preg_replace('/^' . preg_quote($GLOBALS['base_url'], '/') . '\//', '', $redirect_url);
  // This is the up path configured per node in fundraiser_menu. Used as default;
  $redirect = array('node/' . $node->nid . '/confirmation/' . $did);
  if ($redirect_url == '<confirmation>') {
    // This is the default path configured per node in fundraiser_menu.
    $redirect = array('node/' . $node->nid . '/confirmation/' . $did);
  }
  elseif (valid_url($redirect_url, TRUE)) {
    // This is the path set by the user.
    $redirect = array($redirect_url);
  }
  elseif ($redirect_url && strpos($redirect_url, 'http') !== 0) {
    // This is the reconstructed path from the user.
    $parts = parse_url($redirect_url);
    $query = $parts['query'] ? array_merge($parts['query'], array('did'=> $did)) : array('did' => $did);
    $redirect = array($parts['path'], array('query' => $query, 'fragment' => $parts['fragment']));
  }
  // And we add the redirect to the form processing for the final step.
  $form_state['redirect'] = $redirect;
}

/**
 * TODO figure out where this comes from - webform? - SeH 1/10/12
 */
function fundraiser_confirmation_page_tokens($type) {
  if ($type == 'donation_form') {
    $order_tokens = array();
    $items = token_get_list('order');
    foreach ($items as $module => $tokens) {
      foreach ($tokens as $token => $desc) {
        $order_tokens['order']['[' . $token . ']'] = $desc;
      }
    }
    return $order_tokens;
  }
}