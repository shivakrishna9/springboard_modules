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

  if (isset($_GET['campaign']) && is_numeric($_GET['campaign'])) {
    $campaign = node_load($_GET['campaign']);

    if ($campaign->type == 'p2p_campaign' && isset($campaign->field_p2p_form_header) && !empty($campaign->field_p2p_form_header[$campaign->language])) {

      $form['p2p_campaign_nid'] = array(
        '#type' => 'value',
        '#value' => $campaign->nid,
      );

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

  $form['social_login'] = springboard_p2p_social_login_form($form_state);

  $form['new_account'] = springboard_p2p_user_register_form($form_state);

  $form['login'] = springboard_p2p_existing_user_login_form($form_state);

  return $form;
}

/**
 * Validation handler for registration page.
 */
function springboard_p2p_register_page_validate(&$form, &$form_state) {

}

/**
 * Sumibt handler for registration page.
 */
function springboard_p2p_register_page_submit(&$form, &$form_state) {

}

/**
 * Forms api array for registering users.
 *
 * Uses springboard_p2p_registration_fields to determine weights and
 * enabledness of fields.
 */
function springboard_p2p_user_register_form(&$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Create new account',
  );

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
    '#validate' => array('springboard_p2p_user_register_validate'),
  );

  $form['#validate'][] = 'user_register_validate';
  // Add the final user registration form submit handler.
  $form['#submit'][] = 'user_register_submit';

  // Mess with the default form.
  $form['account']['name']['#required'] = FALSE;
  $form['account']['name']['#access'] = FALSE;
  $form['mail'] = $form['account']['mail'];
  $form['mail']['#required'] = FALSE;
  $form['mail']['#weight'] = $fields_to_display['mail']['weight'];
  unset($form['account']['mail']);
  $form['account']['pass']['#required'] = FALSE;
  $form['account']['roles']['#access'] = FALSE;
  $form['account']['notify']['#access'] = FALSE;
  $form['account']['status']['#access'] = FALSE;

  return $form;
}

function springboard_p2p_user_register_validate($form, &$form_state) {
  $fields = springboard_p2p_get_enabled_registration_fields();
  foreach ($fields as $key => $field) {
    if ($key == 'mail') {
      if (empty($form_state['values']['mail'])) {
        form_set_error('new_account][mail', 'E-mail address is required.');
      }
    }
    else {
      $lang_code = $form['new_account'][$key]['#language'];
      if (empty($form_state['values'][$key][$lang_code][0]['value'])) {
        if (isset($form['new_account'][$key][$lang_code][0]['#title'])) {
          $title = $form['new_account'][$key][$lang_code][0]['#title'];
        }
        else {
          $title = $form['new_account'][$key][$lang_code]['#title'];
        }
        form_set_error('new_account][' . $key, $title . ' is required.');
      }

    }
  }

  if (empty($form_state['pass'])) {
    form_set_error('new_account][pass', 'Password is required.');
  }
}

/**
 * Forms api array for the social login form.
 */
function springboard_p2p_social_login_form(&$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Social login',
  );

  $form['facebook'] = array(
    '#markup' => '<div style="height: 100px; width: 180px; margin: 30px 0 0 0; text-align: center; font-size: 100px; color: blue;">f</div>',
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Log in with Facebook'),
    '#validate' => array('springboard_p2p_social_login_validate'),
    '#submit' => array('springboard_p2p_social_login_submit'),
  );

  return $form;
}

function springboard_p2p_social_login_validate($form, &$form_state) {
  // No validation.
}

/**
 * Submit handler for social login.
 */
function springboard_p2p_social_login_submit($form, &$form_state) {
  drupal_set_message('You clicked the log in with facebook button.');
}

/**
 * Forms api array for the existing user login form.
 */
function springboard_p2p_existing_user_login_form(&$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Existing user login',
  );

  // Display login form:
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#size' => 60,
    '#maxlength' => USERNAME_MAX_LENGTH,
    '#required' => FALSE,
  );

  $form['name']['#description'] = t('Enter your @s username.', array('@s' => variable_get('site_name', 'Drupal')));

  $form['pass'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#description' => t('Enter the password that accompanies your username.'),
    '#required' => FALSE,
  );

  $form['#validate'] = user_login_default_validators();

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Log in'),
    '#validate' => array('springboard_p2p_existing_user_login_validate'),
    '#submit' => array('springboard_p2p_existing_user_login_submit'),
  );

  return $form;
}

function springboard_p2p_existing_user_login_validate($form, &$form_state) {
  dpm($form);
  dpm($form_state);

  foreach (array('name', 'pass') as $key) {
    if (empty($form_state['values'][$key])) {
      form_set_error('login][' . $key, $form['login'][$key]['#title'] . ' is required.');
    }
  }
}

function springboard_p2p_existing_user_login_submit($form, &$form_state) {

}

function springboard_p2p_get_enabled_registration_fields() {
  $fields = variable_get('springboard_p2p_registration_fields', array());

  foreach ($fields as $key => $field) {
    if (!$field['enabled']) {
      unset($fields[$key]);
    }
  }

  return $fields;
}
