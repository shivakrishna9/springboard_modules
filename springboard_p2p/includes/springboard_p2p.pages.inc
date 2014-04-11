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

  $form['social_login'] = drupal_get_form('springboard_p2p_social_login_form');

  $form['new_account'] = drupal_get_form('springboard_p2p_user_register_form');

  $form['login'] = drupal_get_form('springboard_p2p_existing_user_login_form');

  return $form;
}

/**
 * Forms api array for registering users.
 *
 * Uses springboard_p2p_registration_fields to determine weights and
 * enabledness of fields.
 */
function springboard_p2p_user_register_form($form, &$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Create new account',
  );

  springboard_p2p_campaign_id_form_element($form);

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
      // @todo This requiredness isn't working.
      // $form[$field_name]['#required'] = TRUE;
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

  // Change the redirect.
  $form['#submit'][] = 'springboard_p2p_user_register_submit';

  // Mess with the default form.
  // Don't show the user the username field.
  $form['account']['name']['#access'] = FALSE;

  // @todo Having some trouble with the submit function when mail is taken out
  // of account.
  // $form['mail'] = $form['account']['mail'];
  // $form['mail']['#required'] = FALSE;
  // $form['mail']['#weight'] = $fields_to_display['mail']['weight'];
  // unset($form['account']['mail']);
  // $form['account']['pass']['#required'] = FALSE;

  $form['account']['roles']['#access'] = FALSE;
  $form['account']['notify']['#access'] = FALSE;
  $form['account']['status']['#access'] = FALSE;

  return $form;
}

/**
 * Validation handler for user registration.
 *
 * Does its own requiredness checking because I can't get the normal drupal
 * required working on the profile fields.
 *
 * Sets the name value so the username will be the email address.
 */
function springboard_p2p_user_register_validate($form, &$form_state) {
  $fields = springboard_p2p_get_enabled_registration_fields();

  // Fill in username.
  $form_state['values']['name'] = $form_state['values']['mail'];

  foreach ($fields as $key => $field) {
    // Need to skip the mail field.
    if ($key != 'mail') {
      $lang_code = $form[$key]['#language'];
      if (empty($form_state['values'][$key][$lang_code][0]['value'])) {
        if (isset($form[$key][$lang_code][0]['#title'])) {
          $title = $form[$key][$lang_code][0]['#title'];
        }
        else {
          $title = $form[$key][$lang_code]['#title'];
        }
        form_set_error($key, $title . ' is required.');
      }

    }
  }
}

/**
 * Submit handler.
 *
 * Saves the campaign approval status and changes the redirect
 * after the user register submit handler.
 */
function springboard_p2p_user_register_submit($form, &$form_state) {
  $campaign_id = $form_state['values']['springboard_p2p_campaign_nid'];
  if (!empty($campaign_id)) {
    $campaign = node_load($campaign_id);
    if ($campaign->type == 'p2p_campaign') {
      $private = $campaign->field_p2p_campaigns_approval[$campaign->language][0]['value'];

      $approval = new SpringbaordP2pPrivateCampaignApproval($form_state['uid'], $campaign_id);

      if ($private) {
        $approval->request();
        drupal_set_message('You have been added to the approval queue for ' . $campaign->title);
      }
      else {
        $approval->approve();
        drupal_set_message('You can now create a personal campaign in ' . $campaign->title);
      }
    }
  }

  $form_state['redirect'] = 'springboard_p2p/dashboard/' . $form_state['uid'];
}

/**
 * Forms api array for the social login form.
 */
function springboard_p2p_social_login_form($form, &$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Social login',
  );

  springboard_p2p_campaign_id_form_element($form);

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

/**
 * Submit handler for social login.
 */
function springboard_p2p_social_login_submit($form, &$form_state) {
  drupal_set_message('You clicked the log in with facebook button.');
}

/**
 * Forms api array for the existing user login form.
 */
function springboard_p2p_existing_user_login_form($form, &$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Existing user login',
  );

  springboard_p2p_campaign_id_form_element($form);

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
  $form['#submit'] = array('user_login_submit', 'springboard_p2p_existing_user_login_submit');
  return $form;
}

/**
 * Submit handler for login.  Changes the redirect.
 */
function springboard_p2p_existing_user_login_submit($form, &$form_state) {
  $form_state['redirect'] = 'springboard_p2p/dashboard/' . $form_state['uid'];
}

/**
 * Get only the reg fields that have been enabled in the p2p settings.
 *
 * @return array
 *   The enabled profile fields.
 */
function springboard_p2p_get_enabled_registration_fields() {
  $fields = variable_get('springboard_p2p_registration_fields', array());

  foreach ($fields as $key => $field) {
    if (!$field['enabled']) {
      unset($fields[$key]);
    }
  }

  return $fields;
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
 * Gets a campaign ID from the URL.
 *
 * @return int|null
 *   The campaign ID, or NULL if one can't be found.
 */
function springboard_p2p_get_campaign_id_from_request() {
  if (isset($_GET['campaign']) && is_numeric($_GET['campaign'])) {
    return $_GET['campaign'];
  }

  return NULL;
}

/**
 * Add the campaign ID value form element to a form.
 *
 * @param array $form
 *   The form array to insert the element into.
 */
function springboard_p2p_campaign_id_form_element(&$form) {
  $form['springboard_p2p_campaign_nid'] = array(
    '#type' => 'value',
    '#value' => springboard_p2p_get_campaign_id_from_request(),
  );
}
