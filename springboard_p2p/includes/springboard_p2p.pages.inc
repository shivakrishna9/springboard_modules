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

  if (user_is_anonymous()) {
    $form['login'] = springboard_p2p_existing_user_login_form($form_state);
  }

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

  global $user;

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

  $fields_to_display = variable_get('springboard_p2p_registration_fields', array());

  // Attach field widgets, and hide the ones where the 'user_register_form'
  // setting is not on.
  $langcode = entity_language('user', $form['#user']);
  field_attach_form('user', $form['#user'], $form, $form_state, $langcode);

  foreach (field_info_instances('user', 'user') as $field_name => $instance) {
    if (isset($fields_to_display[$field_name]['enabled']) && $fields_to_display[$field_name]['enabled']) {
      $form[$field_name]['#weight'] = $fields_to_display[$field_name]['weight'];
    }
    else {
      $form[$field_name]['#access'] = FALSE;
    }
  }

  if ($admin) {
    // Redirect back to page which initiated the create request;
    // usually admin/people/create.
    $form_state['redirect'] = $_GET['q'];
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new account'),
  );

  $form['#validate'][] = 'user_register_validate';
  // Add the final user registration form submit handler.
  $form['#submit'][] = 'user_register_submit';

  return $form;
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

  $form['facebook_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Log in with Facebook',
  );

  return $form;
}

/**
 * Forms api array for the existing user login form.
 */
function springboard_p2p_existing_user_login_form(&$form_state) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => 'Existing user login',
  );

  $form['user_login'] = drupal_get_form('user_login');

  return $form;
}
