<?php


/**
 * Provides content for the legislator lookup block.
 */
function sba_leg_lookup_legislator_lookup($delta, $mb_delta) {
  switch ($delta) {
    case 'sba_legislator_lookup':
      return drupal_get_form('sba_leg_lookup_legislator_lookup_form', $mb_delta);
  }
  return NULL;
}

/**
 * Theme function to render list of legislators.
 */
function theme_sba_leg_lookup_legislators($variables) {
  $legislator_roles = sba_leg_lookup_legislator_roles();
  $output = '<div id="sba-legislators">';
  foreach ($variables['legislators'] as $role => $legislators) {
    $label = $legislator_roles[$role];
    if (in_array($role, array(
      'Federal Representative',
      "Federal Senator",
      'State Representative',
      'State Senator',
    ))) {
      $num = count($legislators);
      $label = format_plural($num, $legislator_roles[$role], $legislator_roles[$role] . 's');
    }
    $output .= '<div class="sba-legislator-role"><strong>' . $label . '</strong>';
    foreach ($legislators as $legislator) {
      $output .= sba_leg_lookup_legislator_markup($legislator);
    }
    $output .= '</div>';
  }

  return $output;
}

/**
 * Creates the HTML markup to render a single legislator.
 */
function sba_leg_lookup_legislator_markup($legislator) {
  $markup = '<div class="sba-legislator" itemscope itemtype="http://schema.org/Person">';
  $markup .= '<span itemprop="jobTitle">' . $legislator->salutation . '</span> <span itemprop="name">' . $legislator->first_name . ' ' . $legislator->last_name .' (' . $legislator->party . ')</span>';
  $markup .= '<div itemprop="address" itemscope itemtype="http://schema.org/PostalAddress">';
  $markup .= '<span class="sba-legislator-address" itemprop="streetAddress">';

  if (!empty($legislator->addresses[0]->building)) {
    $markup .= $legislator->addresses[0]->building . '<br/>';
  }

  $markup .= $legislator->addresses[0]->street . '<br/>';
  $markup .= $legislator->addresses[0]->city . ', ' . $legislator->addresses[0]->state . ' ' . $legislator->addresses[0]->zip_code . '</span></div>';
  if (!empty($legislator->addresses[0]->phone)) {
    $markup .= '<span class="sba-legislator-phone" itemprop="telephone">' . $legislator->addresses[0]->area_code . '-' . $legislator->addresses[0]->phone . '</span><br />';
  }
  if (!empty($legislator->email)) {
    $markup .= '<a class="sba-legislator-email" href="mailto:' . $legislator->email . '" itemprop="email">' . $legislator->email . '</a><br />';
  }
  if (!empty($legislator->facebook)) {
    $markup .= '<a class="sba-legislator-facebook" href="' . $legislator->facebook . '">Facebook</a><br />';
  }

  if (!empty($legislator->twitter)) {
    $markup .= '<a class="sba-legislator-twitter" href="' . $legislator->twitter . '">Twitter</a><br />';
  }
  $markup .= '</div>';
  return $markup;
}

/**
 * Block content for sba_legislator_lookup block.
 *
 * Displays a form for users to enter their address and perform a legislator
 * lookup. After successful submission the form is hidden and a list of
 * elected officials id displayed in it's place.
 */
function sba_leg_lookup_legislator_lookup_form($form, &$form_state, $mb_delta) {
  $form['#attached']['css'][] = drupal_get_path('module', 'sba_leg_lookup') . '/css/lookup.css';

  // If form has been submitted and we have some legislators, show them.
  if (!empty($form_state['legislators'])) {
    $form['submission'] = array(
      '#markup' => theme('sba_leg_lookup_legislators', array('legislators' => $form_state['legislators'])),
    );
  }
  else {
    $form['sbp_address'] = array(
      '#type' => 'textfield',
      '#title' => t('Street'),
      '#required' => TRUE,
    );

    $form['sbp_city'] = array(
      '#type' => 'textfield',
      '#title' => t('City'),
      '#required' => TRUE,
    );

    module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets');
    $form['sbp_state'] = array(
      '#type' => 'select',
      '#title' => t('State'),
      '#options' => sba_leg_lookup_states(),
      '#required' => TRUE,
      '#maxlength' => 2,
    );

    $form['sbp_zip'] = array(
      '#type' => 'textfield',
      '#title' => t('ZIP Code'),
      '#required' => TRUE,
      '#maxlength' => 9,
    );

    $form['mb_delta'] = array(
      '#type' => 'hidden',
      '#value' => $mb_delta,
    );

    $form['plus4'] = array(
      '#type' => 'hidden',
      '#value' => '',
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => variable_get('sba_legislator_lookup_submit_text_' . $mb_delta, array('Find my legislators')),
    );
  }

  return $form;
}

/**
 * Validation handler for sba_leg_lookup_legislator_lookup_form.
 *
 * Performs address validation and if successful stashes the plus4
 * in form_state to be used in the submission handler.
 */
function sba_leg_lookup_legislator_lookup_form_validate($form, &$form_state) {
  form_load_include($form_state, 'inc', 'springboard_advocacy', 'includes/springboard_advocacy.smarty_streets');

  $form_error = &drupal_static('form_set_error');
  if (empty($form_error)) {
    $geo = springboard_advocacy_smarty_lookup($form_state['values']);
  }

  if (!empty($geo) && !empty($geo['zip']) && !empty($geo['plus4'])) {
    form_set_value($form['plus4'], $geo['plus4'], $form_state);
  }
  else {
    form_set_error('sbp_address', 'Unable to verify the submitted address. Please try again.');
  }
}

/**
 * Submit handler for sba_leg_lookup_legislator_lookup_form.
 *
 * Invokes the advocacy server getLegislator method and stashes the
 * result in form_state to user in the form display.
 */
function sba_leg_lookup_legislator_lookup_form_submit($form, &$form_state) {
  $full_zip = $form_state['values']['sbp_zip'] . '-' . $form_state['values']['plus4'];
  $api_call = new AdvocacyApiCall();
  $response = $api_call->invokeClientMethod('getLegislators', $full_zip);
  if (isset($response->data) && $response->status_code == 200) {
    $form_state['legislators'] = sba_leg_lookup_sort_legislators(get_object_vars($response->data), $form_state['values']['mb_delta']);
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Breaks legislators down by role.
 *
 * Prunes out roles according to the
 * block settings.
 */
function sba_leg_lookup_sort_legislators($legislators, $mb_delta) {
  $sorted_legislators = array();
  $sba_legislator_lookup_types = variable_get('sba_legislator_lookup_types_' . $mb_delta,
    array('FS', 'FR', 'SS', 'SR', 'GOVNR', 'PRES01', 'PRES02', 'PRES03')
  );

  foreach ($legislators as $legislator) {
    // Prune out types based on the block configuration.
    if (in_array($legislator->role, $sba_legislator_lookup_types)) {
      $sorted_legislators[$legislator->role][] = $legislator;
    }
  }

  return $sorted_legislators;
}

/**
 * Returns the legislator roles.
 */
function sba_leg_lookup_legislator_roles() {
  return array(
    'FS' => t('Federal Senator'),
    'FR' => t('Federal Representative'),
    'SS' => t('State Senator'),
    'SR' => t('State Representative'),
    'GOVNR' => t('Governor'),
    'PRES01' => t('President'),
    'PRES02' => t('First Lady'),
    'PRES03' => t('Vice President'),
  );
}

function sba_leg_lookup_states() {
  $states = array(
    '' => '',
    'AL' => 'Alabama',
    'AK' => 'Alaska',
    'AZ' => 'Arizona',
    'AR' => 'Arkansas',
    'CA' => 'California',
    'CO' => 'Colorado',
    'CT' => 'Connecticut',
    'DE' => 'Delaware',
    'DC' => 'District Of Columbia',
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

  return $states;
}
