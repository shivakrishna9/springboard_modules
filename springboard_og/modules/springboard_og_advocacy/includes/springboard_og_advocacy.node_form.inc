<?php

/**
 * Implements hook_node_form_alter().
 *
 * Add the advocacy settings to the goup node's vertical tab.
 */
function springboard_og_advocacy_form_node_form_alter(&$form, &$form_state) {

  if ($form['#node']->type == 'springboard_group') {
    // First check if advocacy is configured.
    $unconfigured = springboard_advocacy_api_unconfigured('springboard');
    if ($unconfigured) {
      $items = implode(', ', $unconfigured);
      $message = t('Please <a href="!url">configure</a> the Springboard Advocacy API settings. There are empty values in :items. This feature will not function without required configuration.', array(
        '!url' => url('admin/config/services/advocacy'),
        ':items' => $items,
      ));
      $form['advocacy_settings']['not_configured']['#markup'] = $message;
    }
    else {
      // Get the subscription info.
      $api_call = springboard_advocacy_api_call();
      $subscription = $api_call->getSubscription();
      if ($subscription) {
        // Add the form.
        springboard_og_advocacy_build_subscription_form($form, $subscription);
      }
    }
  }

}


/**
 * Builds the subscription form based on allowed subscription levels.
 *
 * Settings are stored in db table sb_og_advocacy:
 *   gid
 *   group_subscription_type
 *   master_subscription_type
 *   allowed_states.
 *
 * @param array $form
 *   The node edit form.
 * @param array $subscription
 *   The advocacy subscription details.
 */
function springboard_og_advocacy_build_subscription_form(&$form, $subscription) {

  $form['#attached']['css'][] = drupal_get_path('module', 'springboard_og_advocacy') . '/css/sb-og-advocacy.css';

  // Complete set of subscription options prior to subscription  check.
  $options = array(
    'federal-and-states' => 'Federal + All States',
    'federal-only' => 'Federal Only',
    'state-only' => 'All States',
    'federal-and-states-selected' => 'One or More States (Plus Federal)',
    'states-selected' => 'One or More States (No Federal)',
  );

  // All states. Can be subject to filtering later.
  $state_options = springboard_og_advocacy_get_all_states();

  // If the main API account is not Federal plus All States, we need to
  // limit the options presented on the group node according to the main API
  // account options.
  switch ($subscription['subscription_type']) {
    case 'federal-only':
      unset($options['federal-and-states']);
      unset($options['state-only']);
      unset($options['federal-and-states-selected']);
      unset($options['states-selected']);
      break;

    case 'state-only':
      unset($options['federal-and-states']);
      unset($options['federal-only']);
      unset($options['federal-and-states-selected']);
      break;

    case 'federal-and-states-selected':
      unset($options['federal-and-states']);
      unset($options['state-only']);
      // Get the allowed states for this subscription level.
      $state_options = springboard_og_advocacy_get_allowed_states($subscription);
      break;

    case 'states-selected':
      unset($options['federal-and-states']);
      unset($options['state-only']);
      unset($options['federal-only']);
      unset($options['federal-and-states-selected']);
      // Get the allowed states for this subscription level.
      $state_options = springboard_og_advocacy_get_allowed_states($subscription);
      break;
  }

  // Variable to store check of whether subscription level previously stored
  // in Drupal no longer matches the current master API server subscription
  // level. This would only happen if the subscription setting was changed on
  // the API server, which should be a *very* rare occurrence.
  $subscription_conflict = FALSE;

  if (!empty($form['#node']->nid)) {
    // Retrieve stored subscription settings for this node.
    $settings = springboard_og_advocacy_get_saved_group_subscription($form['#node']->nid);
    if (!empty($settings['master_subscription_type']) && $settings['master_subscription_type'] != $subscription['subscription_type']) {
      // If the subscription level is stored in this node is incompatible
      // with the master subscription, set a warning.
      if (!in_array($settings['group_subscription_type'], array_keys($options))) {
        $subscription_conflict = TRUE;
        drupal_set_message(t("This group's advocacy subscription setting is out of date. The master subscription on the API server is @level. The master subscription when this group was last updated was @oldlevel. Please update the group subscription level.", array(
          '@level' => check_plain($subscription['subscription_type']),
          '@oldlevel' => check_plain($settings['master_subscription_type']),
        )));
      }
    }
  }

  $default_subscription_type = [];
  // Get the previously stored subscription level if any.
  if (!empty($settings['group_subscription_type'])  && $subscription_conflict == FALSE) {
    $default_subscription_type = $settings['group_subscription_type'];
  }
  elseif (count($options) == 1 && $subscription_conflict == FALSE) {
    // If there's no previously stored subscription level, and there is only
    // one subscription level available, make it the default.
    $default_subscription_type = array_keys($options)[0];
  }

  // Container form element.
  $form['advocacy_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advocacy settings'),
    '#collapsible' => FALSE,
    '#group' => 'additional_settings',
  );

  // Grab the master subscription type.
  $form['advocacy_settings']['master_subscription_type'] = array(
    '#type' => 'value',
    '#value' => $subscription['subscription_type'],
  );

  // REtrieve/set the group subscription type.
  $form['advocacy_settings']['group_subscription_type'] = array(
    '#type' => 'radios',
    '#title' => t('Subscription Level'),
    '#options' => $options,
    '#default_value' => $default_subscription_type,
  );

  // Retrieve set the allowed states, if the subscription is state-limited.
  $default_saved_states = array();
  if (!empty($settings['allowed_states']) && $subscription_conflict == FALSE) {
    $default_saved_states = unserialize($settings['allowed_states']);
  }
  $form['advocacy_settings']['us_states'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Available states'),
    '#options' => $state_options,
    '#default_value' => $default_saved_states,
    '#states' => array(
      'visible' => array(
        array(
          array(':input[name*="group_subscription_type"]' => array('value' => 'federal-and-states-selected')),
          array(':input[name*="group_subscription_type"]' => array('value' => 'states-selected')),
        ),
      ),
    ),
  );

  $form['#validate'][] = 'springboard_og_advocacy_form_validate';
}

/**
 * Form validation handler.
 */
function springboard_og_advocacy_form_validate($form, $form_state) {
  $api_call = springboard_advocacy_api_call();
  $subscription = $api_call->getSubscription();
  if ($subscription) {
    if (empty($form_state['values']['group_subscription_type'])) {
      form_set_error('group_subscription_type', t('Advocacy Group Subscription Level cannot be empty.'));
    }
  }
}

/**
 * Filters US state list for allowed states.
 *
 * @param array $subscription
 *    The subscription level data.
 *
 * @return array
 *   An array of US states allowed for this subscription.
 */
function springboard_og_advocacy_get_allowed_states($subscription) {
  $arr = array_reverse($subscription['states_custom'], TRUE);
  $subscription['states_custom'] = array_reverse($arr, TRUE);
  $states = springboard_og_advocacy_get_all_states();
  $state_options = array_flip(
    array_intersect(
      array_flip($states),
      $subscription['states_custom']
    )
  );
  return $state_options;
}

/**
 * US state list.
 *
 * @return array
 *   All US states.
 */
function springboard_og_advocacy_get_all_states() {
  return array(
    'AL' => 'Alabama',
    'AK' => 'Alaska',
    'AZ' => 'Arizona',
    'AR' => 'Arkansas',
    'CA' => 'California',
    'CO' => 'Colorado',
    'CT' => 'Connecticut',
    'DE' => 'Delaware',
    'DC' => 'District of Columbia',
    'FL' => 'Florida',
    'GA' => 'Georgia',
    'HI' => 'Hawaii',
    'ID' => 'Idaho',
    'IL' => 'Illinois',
    'IN' => 'Indiana',
    'IA' => 'Iowa',
    'KS' => 'Kansas',
    'KY' => 'Kentucky',
    'LA' => 'Louisiana',
    'ME' => 'Maine',
    'MD' => 'Maryland',
    'MA' => 'Massachusetts',
    'MI' => 'Michigan',
    'MN' => 'Minnesota',
    'MS' => 'Mississippi',
    'MO' => 'Missouri',
    'MT' => 'Montana',
    'NE' => 'Nebraska',
    'NV' => 'Nevada',
    'NH' => 'New Hampshire',
    'NJ' => 'New Jersey',
    'NM' => 'New Mexico',
    'NY' => 'New York',
    'NC' => 'North Carolina',
    'ND' => 'North Dakota',
    'OH' => 'Ohio',
    'OK' => 'Oklahoma',
    'OR' => 'Oregon',
    'PA' => 'Pennsylvania',
    'RI' => 'Rhode Island',
    'SC' => 'South Carolina',
    'SD' => 'South Dakota',
    'TN' => 'Tennessee',
    'TX' => 'Texas',
    'UT' => 'Utah',
    'VT' => 'Vermont',
    'VA' => 'Virginia',
    'WA' => 'Washington',
    'WV' => 'West Virginia',
    'WI' => 'Wisconsin',
    'WY' => 'Wyoming',
  );
}