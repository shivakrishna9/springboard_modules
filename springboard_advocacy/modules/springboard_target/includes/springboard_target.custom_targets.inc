<?php

/**
 * @file
 * Form definition for Target creation and editing.
 */

/*
 * Implements hook_form
 *
 * Serves a dual function as target add and edit form
 */
function springboard_target_form($target_id = NULL) {
  $form = array();

  if (isset($target_id) && is_numeric($target_id)) {
    $loader = new AdvocacyApiClientLoader();
    $client = $loader->getClient();
    $target = $client->getCustomTarget($target_id);
    if (isset($target->error)) {
      $form = array();
      $form['not_found']['#markup'] = $target->error->message;
      return $form;
    }
    $target = $target->data;
  }

  //@TODO: uncomment "required" as development nears completion.

  $form['salutation'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => isset($target->salutation) ? $target->salutation : '',
    '#size' => 10,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-salutation')),
  );

  $form['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
    '#default_value' => isset($target->first_name) ? $target->first_name : '',
    '#size' => 30,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-first-name')),
  );

  $form['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#default_value' => isset($target->last_name) ? $target->last_name : '',
    '#size' => 30,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-last-name')),
  );

  $form['organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization/Affiliation'),
    '#default_value' => isset($target->organization) ? $target->organization : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-organization')),
  );

  $form['target_type'] = array(
    '#type' => 'select',
    '#title' => t('Target Type'),
    '#default_value' => isset($target->target_type) ? $target->target_type : '',
    '#options' => springboard_target_types(),
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-target-type')),
  );

  $form['party'] = array(
    '#type' => 'select',
    '#title' => t('Party (If applicable)'),
    '#default_value' => isset($target->party) ? array($target->party) : '',
    '#options' => springboard_target_parties(), 
    '#attributes' => array('class' => array('target-add', 'target-party')),
  );

  $form['gender'] = array(
    '#type' => 'select',
    '#title' => t('Gender'),
    '#default_value' => isset($target->gender) ? array($target->gender) : '',
    '#options' => array('M' => 'Male', 'F' => 'Female', 'O' => 'Not Specified'), 
    '#attributes' => array('class' => array('target-add', 'target-first-name')),
  );

  $form['area_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone Area code'),
    '#default_value' => isset($target->addresses[0]->area_code) ? $target->addresses[0]->area_code : '',
    '#size' => 10,
    '#maxlength' => 3,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-phone-area-code')),
  );

  $form['phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone'),
    '#default_value' => isset($target->addresses[0]->phone) ? $target->addresses[0]->phone : '',
    '#size' => 20,
    '#maxlength' => 8,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-phone')),
  );

  $form['ext'] = array(
    '#type' => 'textfield',
    '#title' => t('Extension'),
    '#default_value' => isset($target->addresses[0]->ext) ? $target->addresses[0]->ext : '',
    '#size' => 10,
    '#maxlength' => 6,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-phone-extension')),
  );

  $form['fax_area_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Fax Area Code'),
    '#default_value' => isset($target->addresses[0]->fax_area_code) ? $target->addresses[0]->fax_area_code : '',
    '#size' => 10,
    '#maxlength' => 3,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-fax-area-code')),
  );

  $form['fax'] = array(
    '#type' => 'textfield',
    '#title' => t('Fax'),
    '#default_value' => isset($target->addresses[0]->fax) ? $target->addresses[0]->fax : '',
    '#size' => 20,
    '#maxlength' => 8,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-fax')),
  );

  $form['street'] = array(
    '#type' => 'textfield',
    '#title' => t('Address'),
    '#default_value' => isset($target->addresses[0]->street) ? $target->addresses[0]->street : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-address')),
  );

    $form['street_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Address Line 2'),
    '#default_value' => isset($target->addresses[0]->street_2) ? $target->addresses[0]->street_2 : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-address-2')),
  );

    $form['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => isset($target->addresses[0]->city) ? $target->addresses[0]->city : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-city')),
  );

    $form['state'] = array(
    '#type' => 'select',
    '#title' => t('State'),
    '#default_value' => isset($target->addresses[0]->state) ? array($target->addresses[0]->state) : '',
    '#options' => springboard_target_states(),
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-state')),
  );

  $form['zip_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Zip Code'),
    '#default_value' => isset($target->addresses[0]->zip_code) ? $target->addresses[0]->zip_code : '',
    '#size' => 10,
    '#maxlength' => 10,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-zip')),
  );

  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Address'),
    '#default_value' => isset($target->email) ? $target->email : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-email')),
  );

  $form['facebook'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook'),
    '#default_value' => isset($target->facebook) ? $target->facebook : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-facebook')),
  );

    $form['twitter'] = array(
    '#type' => 'textfield',
    '#title' => t('Twitter'),
    '#default_value' => isset($target->twitter) ? $target->twitter : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-twitter')),
  );

  $form['website'] = array(
    '#type' => 'textfield',
    '#title' => t('Web site'),
    '#default_value' => isset($target->website) ? $target->website : '',
    '#size' => 60,
    '#maxlength' => 128,
    //'#required' => TRUE,
    '#attributes' => array('class' => array('target-add', 'target-website')),
  );

  $form['class_name'] = array(
    '#type' => 'hidden',
    '#value' => 'Target'
  );

  return $form;
}

/*
 * Return an array of target types.
 * This could become user-configurable
 */
function springboard_target_types() {
  $types = array('legislator' => 'Elected Offical', 'target' => 'Custom');
  return $types;
}

/*
 * Return an array of target parties.
 */
function springboard_target_parties() {
  $parties = array('D' => 'Democrat', 'R' => 'Republican', 'I' => 'Independent', 'O' => 'Other');
  return $parties;
}

function springboard_target_states() {
  $states = array(
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

// @TODO: remove this if there's no ulimate use for it. 
// Loads targets and outputs a paginated table

// function springboard_target_custom_targets() {
//  $header = array('Name', 'Title', 'Organization', 'Email', 'Phone', 'Address', 'Actions');
//   $rows = array();

//   $loader = new AdvocacyApiClientLoader();
//   $client = $loader->getClient();
//   $targets = $client->getCustomTargets();

//   foreach ($targets as $target) {
//     $links = array();
//     $links[] = l('edit', 'target/' . $target->id . '/edit');
//     $links[] = l('view', 'view/' . $target->id);
//     $links = implode(' | ', $links);
//     $rows[] = array(
//       'data' => array(
//         array('data' => $target->first_name . ' ' . $target->last_name),
//         array('data' => $target->salutation),
//         array('data' => ''),
//         array('data' => $target->email),
//         array('data' => ''),
//         array('data' => ''),
//         array('data' => $links),
//       ),
//     );
//   }

//   $rows = springboard_target_pager_default_initialize($rows, 1);

//   $output = theme('table', array('header' => $header, 'rows' => $rows));
//   $output .= theme('pager', array('quantity' => 1));
//   return $output;
// }

// //modified version of pager.inc function pager_default_initialize()
// function springboard_target_pager_default_initialize($rows, $limit = 9, $element = 0) {
//  global $pager_page_array, $pager_total, $pager_total_items, $pager_limits;

//   $page = pager_find_page($element);

//   // We calculate the total of pages as ceil(items / limit).
//   $pager_total_items[$element] = count($rows);
//   $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
//   $pager_page_array[$element] = max(0, min($page, ((int) $pager_total[$element]) - 1));
//   $pager_limits[$element] = $limit;

//   return array_slice($rows, $pager_page_array[$element] * $limit, $limit, TRUE);
// }