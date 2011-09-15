<?php

/**
 * @file
 * Field management functions for the fundraiser module
 *
 */
 
/**
 * Creates all donation specific webform components
 *
 * @param $nid
 *   The webform nid
 * @param $donation_amounts
 *   A string containing a comma separated list of donation ask amounts
 * @param $show_other_amount
 *   A flag to indicate if a 'other amount' field will be added to the form
 * @param $min_donation_amount
 *   A minimum value for the other amount field that will be validated upon submission
 */
function fundraiser_create_webform_components($nid, $donation_amounts, $show_other_amount, $min_donation_amount) {
  module_load_include('inc', 'webform', 'includes/webform.components');
  // create an array to hold all of the created components
  $components = array();

	// market source id
	$market_source = array();
	$market_source['nid'] = $nid;
	$market_source['pid'] = 0; 
	$market_source['value'] = variable_get('market_source_default_campaign', 'default_ms');
	$market_source['form_key'] = 'ms'; 
	$market_source['name'] = 'Market Source';
	$market_source['type'] = 'hidden';  
	$market_source['extra'] = array(
		'description' => '',
	);
	$market_source['mandatory'] = 1; 
	$market_source['weight'] = 20; 
	$market_source['email'] = 1;
	$market_source['maps_to'] = 'profile_ms';
	
	$id = webform_component_insert($market_source);
	$components[$id] = $market_source;
	
	// if the sf_donation is installed, try to get the default campaign
	$default_campaign = '';
	if (variable_get('fundraiser_salesforce_integration', 0)) {
		$default_campaign = variable_get('sf_donation_default_campaign', 'default_cid');
	}
	
	// campaign id
	$campaign = array();
	$campaign['nid'] = $nid;
	$campaign['pid'] = 0; 
	$campaign['value'] = $default_campaign;
	$campaign['form_key'] = 'cid'; 
	$campaign['name'] = 'Campaign ID';
	$campaign['type'] = 'hidden';  
	$campaign['extra'] = array(
		'description' => '',
	);
	$campaign['mandatory'] = 1; 
	$campaign['weight'] = 21; 
	$campaign['email'] = 1;
	$campaign['maps_to'] = 'profile_cid';
	
	$id = webform_component_insert($campaign);
    $components[$id] = $campaign;
  
	// referrer
	$referrer = array();
	$referrer['nid'] = $nid;
	$referrer['pid'] = 0; 
	$referrer['value'] = '';
	$referrer['form_key'] = 'referrer'; 
	$referrer['name'] = 'Referrer';
	$referrer['type'] = 'hidden';  
	$referrer['extra'] = array(
		'description' => '',
	);
	$referrer['mandatory'] = 0; 
	$referrer['weight'] = 22; 
	$referrer['email'] = 1;
	$referrer['maps_to'] = 'profile_referrer';
	
	$id = webform_component_insert($referrer);
	$components[$id] = $referrer;
	
	// initial referrer
	$initial_referrer = array();
	$initial_referrer['nid'] = $nid;
	$initial_referrer['pid'] = 0; 
	$initial_referrer['value'] = '';
	$initial_referrer['form_key'] = 'initial_referrer'; 
	$initial_referrer['name'] = 'Initial Referrer';
	$initial_referrer['type'] = 'hidden';  
	$initial_referrer['extra'] = array(
		'description' => '',
	);
	$initial_referrer['mandatory'] = 0; 
	$initial_referrer['weight'] = 23; 
	$initial_referrer['email'] = 1;
	$initial_referrer['maps_to'] = 'profile_initial_referrer';
	
	$id = webform_component_insert($initial_referrer);
	$components[$id] = $initial_referrer;
	
	// donation amount
	$donation = array();
	$donation['nid'] = $nid;
	$donation['pid'] = 0; 
	$donation['form_key'] = 'donation'; 
	$donation['name'] = 'Donation';
	$donation['type'] = 'fieldset';  
	$donation['extra'] = array(
		'description' => '',
	);
	$donation['mandatory'] = 0; 
	$donation['weight'] = 0; 
	$donation['email'] = 0;
	
	$pid = webform_component_insert($donation);
	$components[$pid] = $donation;

	// determine the type of amount field to output
	if ($donation_amounts) {
		ksort($donation_amounts, SORT_NUMERIC); 
		$formatted_amounts = _fundraiser_create_amount_options($donation_amounts, $show_other_amount);
		
		if ($show_other_amount) {		
			// other amount
		    $component = _fundraiser_insert_other_amount_component($nid, $pid, $min_donation_amount);
			$components[$component['id']] = $component['other_amount'];
		}

		// ask amounts
		$component = _fundraiser_insert_amount_component($nid, $pid, $donation_amounts, $show_other_amount);
		$components[$component['id']] = $component['amount'];
	}
	else { // plain old textbox for donation amount
		$amount = array();
		$amount['nid'] = $nid;
		$amount['pid'] = $pid; 
		$amount['form_key'] = 'amount'; 
		$amount['name'] = 'Amount';
		$amount['type'] = 'textfield';
		$amount['extra'] = array(
			'description' => '',
			'width' => 10,
			'field_prefix' => '$',
		);
		$amount['mandatory'] = 1; 
		$amount['weight'] = 0; 
		$amount['email'] = 1;

		$id = webform_component_insert($amount);
		$components[$id] = $amount;
	}
	
	// donor information
	$donor_information = array();
	$donor_information['nid'] = $nid;
	$donor_information['pid'] = 0; 
	$donor_information['form_key'] = 'donor_information'; 
	$donor_information['name'] = 'Your Information';
	$donor_information['type'] = 'fieldset';  
	$donor_information['extra'] = array(
		'description' => '',
	);
	$donor_information['mandatory'] = 0; 
	$donor_information['weight'] = 1; 
	$donor_information['email'] = 0;
	
	$pid = webform_component_insert($donor_information);
	$components[$pid] = $donor_information;
	
	// first name
	$first_name = array();
	$first_name['nid'] = $nid;
	$first_name['pid'] = $pid; 
	$first_name['form_key'] = 'first_name'; 
	$first_name['name'] = 'First Name';
	$first_name['type'] = 'textfield';  
	$first_name['extra'] = array(
		'description' => '',
	);
	$first_name['mandatory'] = 1; 
	$first_name['weight'] = 0; 
	$first_name['email'] = 1;
	$first_name['maps_to'] = 'profile_first_name';
	
	$id = webform_component_insert($first_name);
	$components[$id] = $first_name;
	
	// last name
	$last_name = array();
	$last_name['nid'] = $nid;
	$last_name['pid'] = $pid; 
	$last_name['form_key'] = 'last_name'; 
	$last_name['name'] = 'Last Name';
	$last_name['type'] = 'textfield';  
	$last_name['extra'] = array(
		'description' => '',
	);
	$last_name['mandatory'] = 1; 
	$last_name['weight'] = 1; 
	$last_name['email'] = 1;
	$last_name['maps_to'] = 'profile_last_name';
	
	$id = webform_component_insert($last_name);
	$components[$id] = $last_name;

	// email
	$email = array();
	$email['nid'] = $nid;
	$email['pid'] = $pid; 
	$email['form_key'] = 'email'; 
	$email['name'] = 'Email';
	$email['type'] = 'textfield';  
	$email['extra'] = array(
		'description' => '',
	);
	$email['mandatory'] = 1; 
	$email['weight'] = 2; 
	$email['email'] = 1;
	$email['maps_to'] = 'mail';
	
	$id = webform_component_insert($email);
	$components[$id] = $email;
	
	// billing
	$billing_information = array();
	$billing_information['nid'] = $nid;
	$billing_information['pid'] = 0; 
	$billing_information['form_key'] = 'billing_information'; 
	$billing_information['name'] = 'Credit Card Billing Information';
	$billing_information['type'] = 'fieldset';  
	$billing_information['extra'] = array(
		'description' => '',
	);
	$billing_information['mandatory'] = 0; 
	$billing_information['weight'] = 3; 
	$billing_information['email'] = 0;
	
	$pid = webform_component_insert($billing_information);
	$components[$pid] = $billing_information;
	
	// billing address
	$billing_address = array();
	$billing_address['nid'] = $nid;
	$billing_address['pid'] = $pid; 
	$billing_address['form_key'] = 'address'; 
	$billing_address['name'] = 'Address';
	$billing_address['type'] = 'textfield';  
	$billing_address['extra'] = array(
		'description' => '',
	);
	$billing_address['mandatory'] = 1; 
	$billing_address['weight'] = 3; 
	$billing_address['email'] = 1;
	$billing_address['maps_to'] = 'profile_address';
	
	$id = webform_component_insert($billing_address);
	$components[$id] = $billing_address;

	// billing address 2
	$billing_address_2 = array();
	$billing_address_2['nid'] = $nid;
	$billing_address_2['pid'] = $pid; 
	$billing_address_2['form_key'] = 'address_line_2'; 
	$billing_address_2['name'] = 'Address Line 2';
	$billing_address_2['type'] = 'textfield';  
	$billing_address_2['extra'] = array(
		'description' => '',
	);
	$billing_address_2['mandatory'] = 0; 
	$billing_address_2['weight'] = 4; 
	$billing_address_2['email'] = 1;
	$billing_address_2['maps_to'] = 'profile_address_line_2';
	
	$id = webform_component_insert($billing_address_2);
	$components[$id] = $billing_address_2;

	// city
	$billing_city = array();
	$billing_city['nid'] = $nid;
	$billing_city['pid'] = $pid; 
	$billing_city['form_key'] = 'city'; 
	$billing_city['name'] = 'City';
	$billing_city['type'] = 'textfield';  
	$billing_city['extra'] = array(
		'description' => '',
	);
	$billing_city['mandatory'] = 1; 
	$billing_city['weight'] = 5; 
	$billing_city['email'] = 1;
	$billing_city['maps_to'] = 'profile_city';
	
	$id = webform_component_insert($billing_city);
	$components[$id] = $billing_city;

	$us = 840;
	$ca = 124;

	$include_all_countries = variable_get('fundraiser_all_countries', 0);
	
	// country
	$countries = uc_country_select(uc_get_field_name('country'), $us, NULL, 'name', uc_address_field_required('country'));
	
	// build option string for countries
	$options = '';
	if ($include_all_countries) {
		foreach($countries['#options'] as $key => $value) {
			$options .= $key .'|'. $value . "\n";
		}
	}
	else {
		foreach($countries['#options'] as $key => $value) {
			if ($key == $us || $key == $ca) {
				$options .= $key .'|'. $value . "\n";
			}
		}
	}
	
	$billing_country = array();
	$billing_country['nid'] = $nid;
	$billing_country['pid'] = $pid; 
	$billing_country['form_key'] = 'country'; 
	$billing_country['name'] = 'Country';
	$billing_country['type'] = 'select';  
	$billing_country['extra'] = array(
		'description' => '',
		'items' => $options,
		'multiple' => 0,
		'aslist' => 'Y',
	);
	$billing_country['mandatory'] = 1; 
	$billing_country['weight'] = 6; 
	$billing_country['email'] = 1;
	$billing_country['maps_to'] = 'profile_country';
	
	$id = webform_component_insert($billing_country);
	$components[$id] = $billing_country;

	// state
	$zones = uc_zone_select(uc_get_field_name('zone'), NULL, NULL, $us, 'name', uc_address_field_required('zone'));
	
	// build option string for states
	$options = '';
	foreach($zones['#options'] as $key => $value) {
		$options .= $key .'|'. $value . "\n";
	}
	
	$billing_state = array();
	$billing_state['nid'] = $nid;
	$billing_state['pid'] = $pid; 
	$billing_state['form_key'] = 'state'; 
	$billing_state['name'] = 'State/Province';
	$billing_state['type'] = 'select';  
	$billing_state['extra'] = array(
		'description' => '',
		'items' => $options,
		'multiple' => 0,
		'aslist' => 'Y',
	);
	$billing_state['mandatory'] = 1; 
	$billing_state['weight'] = 7; 
	$billing_state['email'] = 1;
	$billing_state['maps_to'] = 'profile_state';
	
	$id = webform_component_insert($billing_state);
	$components[$id] = $billing_state;
	
	// zip code
	$billing_zipcode = array();
	$billing_zipcode['nid'] = $nid;
	$billing_zipcode['pid']= $pid; 
	$billing_zipcode['form_key'] = 'zip'; 
	$billing_zipcode['name'] = 'ZIP/Postal Code';
	$billing_zipcode['type'] = 'textfield';  
	$billing_zipcode['extra'] = array(
		'description' => '',
		'width' => 10,
		'maxlength' => 5,
	);
	$billing_zipcode['mandatory'] = 1; 
	$billing_zipcode['weight'] = 8; 
	$billing_zipcode['email'] = 1;
	$billing_zipcode['maps_to'] = 'profile_zip';
	
	$id = webform_component_insert($billing_zipcode);
	$components[$id] = $billing_zipcode;
	
	// credit card information
	$credit_card_information = array();
	$credit_card_information['nid'] = $nid;
	$credit_card_information['pid'] = 0; 
	$credit_card_information['form_key'] = 'credit_card_information'; 
	$credit_card_information['name'] = 'Credit Card Information';
	$credit_card_information['type'] = 'fieldset';  
	$credit_card_information['extra'] = array(
		'description' => '',
	);
	$credit_card_information['mandatory'] = 0; 
	$credit_card_information['weight'] = 5;
	$credit_card_information['email'] = 0;
	
	$pid = webform_component_insert($credit_card_information);
	$components[$pid] = $credit_card_information;

	// credit card number
	$card_number = array();
	$card_number['nid'] = $nid;
	$card_number['pid'] = $pid; 
	$card_number['form_key'] = 'card_number'; 
	$card_number['name'] = 'Card Number';
	$card_number['type'] = 'textfield';  
	$card_number['extra'] = array(
		'description' => '',
		'width' => 20,
	);
	$card_number['mandatory'] = 1; 
	$card_number['weight'] = 0; 
	$card_number['email'] = 0;
	
	$id = webform_component_insert($card_number);
	$components[$id] = $card_number;
	
	// expiration month
	$expiration_month = fundraiser_create_month_field($nid, $pid);
	$id = webform_component_insert($expiration_month);
	$components[$id] = $expiration_month;
	
	// expiration year
	$expiration_year = fundraiser_create_year_field($nid, $pid);
	$id = webform_component_insert($expiration_year);
	$components[$id] = $expiration_year;

	// cvv
	$card_cvv = array();
	$card_cvv['nid'] = $nid;
	$card_cvv['pid']= $pid; 
	$card_cvv['form_key'] = 'card_cvv'; 
	$card_cvv['name'] = 'Card Security Code';
	$card_cvv['type'] = 'textfield';  
	$card_cvv['extra'] = array(
		'description' => '',
		'maxlength' => 4,
		'width' => 5,
	);
	$card_cvv['mandatory'] = 1; 
	$card_cvv['weight'] = 3; 
	$card_cvv['email'] = 0;
	
	$id = webform_component_insert($card_cvv);
	$components[$id] = $card_cvv;

	// recurring donation
	$recurs = array();
	$recurs['nid'] = $nid;
	$recurs['pid']= $pid; 
	$recurs['form_key'] = 'recurs_monthly'; 
	$recurs['name'] = 'Recurring Payment';
	$recurs['type'] = 'select';  
	$recurs['extra'] = array(
		'description' => 'Selecting "Yes" will cause your credit card to be charged once per month until its expiration date.',
		'items' => 'recurs|Yes',
		'multiple' => 'Y',
	);
	$recurs['mandatory'] = 0; 
	$recurs['weight'] = 4; 
	$recurs['email'] = 0;
	
	$id = webform_component_insert($recurs);
	$components[$id] = $recurs;
	
	return $components;
}

/**
 * Creates a year expiration drop down field to be used in donation forms.
 */
function fundraiser_create_year_field($nid, $pid) {
  $this_year = date('Y');
	$years = $this_year . "\n";
	for ($i = 1; $i <= 5; $i++) {
    $years .= ($this_year + $i) . "\n";  
  }
	
	// expiration date
	$expiration_year = array();
	$expiration_year['nid'] = $nid;
	$expiration_year['pid'] = $pid; 
	$expiration_year['form_key'] = 'card_expiration_year'; 
	$expiration_year['name'] = 'Expiration Year';
	$expiration_year['type'] = 'select';  
	$expiration_year['extra'] = array(
		'description' => '',
		'items' => $years,
		'multiple' => 0,
		'aslist' => 'Y',
  );
	$expiration_year['mandatory'] = 1; 
	$expiration_year['weight'] = 2; 
	$expiration_year['email'] = 0;
	return $expiration_year;
}

/**
 * Creates a month expiration drop down field to be used in donation forms.
 */
function fundraiser_create_month_field($nid, $pid) {
  // expiration month
  $months = "1|January\n";
  $months .= "2|Feburary\n";
	$months .= "3|March\n";
	$months .= "4|April\n";
	$months .= "5|May\n";
	$months .= "6|June\n";
	$months .= "7|July\n";
	$months .= "8|August\n";
	$months .= "9|September\n";
	$months .= "10|October\n";
	$months .= "11|November\n";
	$months .= "12|December\n";
	
	$expiration_month = array();
	$expiration_month['nid'] = $nid;
	$expiration_month['pid'] = $pid; 
	$expiration_month['form_key'] = 'card_expiration_month'; 
	$expiration_month['name'] = 'Expiration Month';
	$expiration_month['type'] = 'select';
	$expiration_month['extra'] = array(
		'description' => '',
		'items' => $months,
		'multiple' => 0,
		'aslist' => 'Y',
  );
	$expiration_month['mandatory'] = 1; 
	$expiration_month['weight'] = 1; 
	$expiration_month['email'] = 0;
	return $expiration_month;
}

function fundraiser_required_fields() {
  return array(
    'ms',
    'cid',
    'referrer',
    'initial_referrer',
    'amount',
    'first_name',
    'last_name',
    'email',
    'address',
    'address_line_2',
    'city',
    'country',
    'state',
    'zip',
    'card_number',
    'card_expiration_year',
    'card_expiration_month',
    'card_cvv',
  );
}

/**
 * Updates ask amount and show other amount webform components.
 * 
 * @param $node
 * Donation form node object.
 * 
 * @param $donation_amounts
 * An array of amount|label pairs.
 * 
 * @params $value
 * Default selected value for amount radios.
 */
function _fundraiser_update_amount_webform_component($node, $donation_amounts = array(), $value) {
  $pid = db_result(db_query('SELECT cid FROM {webform_component} WHERE nid = %d AND form_key="donation"', $node->nid));
  // Amount webform component should be a textfield if no donation amounts are available.
  if (!count($donation_amounts)) {
    $has_select = db_result(db_query('SELECT 1 FROM {webform_component} WHERE nid = %d AND form_key = "amount" AND type = "select"', $node->nid));
    if ($has_select) {
      $amount = array();
      $amount['nid'] = $node->nid;
      $amount['pid'] = $pid; 
      $amount['form_key'] = 'amount'; 
      $amount['name'] = 'Amount';
      $amount['type'] = 'textfield';
      $amount['extra'] = array(
        'description' => '',
        'width' => 10,
        'field_prefix' => '$',
      );
      $amount['mandatory'] = 1; 
      $amount['weight'] = 0; 
      $amount['email'] = 1;
      db_query('DELETE FROM {webform_component} WHERE nid =%d AND form_key = "amount"', $node->nid);
      webform_component_insert($amount);
    }
  }
  else {
    $formatted_amounts =  _fundraiser_create_amount_options($donation_amounts, $node->show_other_amount);
    $field_type = db_result(db_query('SELECT type FROM {webform_component} WHERE nid = %d AND form_key = "amount"', $node->nid));
    
    // If the current component is a select update with current donation amount values.
    if (!strcmp($field_type, 'select')) {
      $data = serialize(array('description' => "", 'items' => $formatted_amounts, 'multiple' => 0));
      db_query("UPDATE {webform_component} SET extra = '%s', value = %d WHERE nid = %d AND form_key = 'amount'", $data, $value, $node->nid);
      $message = t('The @node_title donation form has been updated.', array('@node_title' => $node->title));
      $result = db_result(db_query('SELECT extra FROM {webform_component} WHERE nid = %d AND form_key = "other_amount"', $node->nid));
    }
    
    // Since we have donation amounts we need to convert the amount component from textfield to select
    else {
      db_query('DELETE FROM {webform_component} WHERE nid = %d AND form_key = "amount"', $node->nid);
      $amount = _fundraiser_insert_amount_component($node->nid, $pid, $donation_amounts, $node->show_other_amount);
      webform_component_insert($amount['amount']);
    }
  }
  
  $result = db_result(db_query('SELECT 1 FROM {webform_component} WHERE nid = %d AND form_key = "other_amount"', $node->nid));
  if ($node->show_other_amount) {
    // Pre-existing entry in the DB for this form component and it's still enabled so we update.
    if ($result) {
      $extra = array(
        'description' => 'Minimum payment $' . $minimum_donation_amount . '.',
        'width' => 10,
        'field_prefix' => '$',
      );
      db_query("UPDATE {webform_component} SET extra = '%s' WHERE nid=%d AND form_key = 'other_amount'", serialize($extra), $node->nid);
      $message .= t('Other donation amount options have been updated.');
    }
    
    // Other amount has been enabled so we need to add the component data to the DB.
    else {
      _fundraiser_insert_other_amount_component($node->nid, $pid, $node->minimum_donation_amount);
      $message .= t('Other amount options have been added to the form.');
    }
  }

  // Other amount was enabled but is now disabled so the component needs to be removed from the DB to prevent the
  // associated form element(s) being rendered by webform.
  elseif ($result) {
    db_query('DELETE FROM {webform_component} WHERE nid=%d AND form_key ="other_amount"', $node->nid);
    $message .= t('Other amount options have been disabled on the form.');
  }
  watchdog('fundraiser', $message, array(), WATCHDOG_NOTICE, "node/$node->nid");
}

/**
 * Inserts amount webform component
 * 
 * @param $nid
 * Donation form node ID.
 * 
 * @param $pid
 * ID of parent webform component. This is typically the donation component.
 * 
 * @param $donation_amounts
 * An array of amount|label pairs.
 * 
 * @param $show_other_amount
 * Int (0, 1) indicating if show other amounts is enabled. 
 * 
 * @return
 * Returns a keyed array containing the ID of the component created and a keyed array of values used to create the component.
 */
function _fundraiser_insert_amount_component($nid, $pid, $donation_amounts = array(), $show_other_amount) {
  $formatted_amounts = _fundraiser_create_amount_options($donation_amounts, $show_other_amount);
  $amount = array();
  $amount['nid'] = $nid;
  $amount['pid'] = $pid; 
  $amount['form_key'] = 'amount'; 
  $amount['name'] = 'Please select your tax-deductible gift amount below';
  $amount['type'] = 'select';
  $amount['extra'] = array(
    'description' => '',
    'items' => $formatted_amounts,
    'multiple' => 0,
  );
  $keys = array_keys($donation_amounts);
  $amount['mandatory'] = 1; 
  $amount['weight'] = 0; 
  $amount['email'] = 1;
  $amount['value'] = $keys[0];
  $id = webform_component_insert($amount);
  return array('amount' => $amount, 'id' => $id);
}

/**
 *  Inserts other_amount webform component
 *  
 *  @param $nid
 *  Node id.
 *  
 *  @param $pid
 *  ID of parent webform component. This is typically the donation component.
 *  
 *  @param $min_donation_amount
 *  Minimum donation amount for the other amount component.
 *  
 *  @return
 *  Returns a keyed array containing the component values and the id of the created component.
 */
function _fundraiser_insert_other_amount_component($nid, $pid, $min_donation_amount) {
  $other_amount = array();
  $other_amount['nid'] = $nid;
  $other_amount['pid'] = $pid; 
  $other_amount['form_key'] = 'other_amount'; 
  $other_amount['name'] = 'Other';
  $other_amount['type'] = 'textfield';  
  $other_amount['extra'] = array(
    'description' => 'Minimum payment $' . $min_donation_amount . '.',
    'width' => 10,
    'field_prefix' => '$',
  );
  $other_amount['mandatory'] = 0; 
  $other_amount['weight'] = 1;
  $other_amount['email'] = 1;
  $id =  webform_component_insert($other_amount);
  return array('id' => $id, 'other_amount' => $other_amount);
}


/**
 * Helper function to fetch fids for the country and state profile fields.
 * 
 * @return
 * Returns a keyed array of location fields or an empty array if there was an error.
 */
function _fundraiser_get_profile_location_fields() {
  // TODO: this is brittle and will break in any instances where fundraiser is depoyed for a country that doesn't use states/provinces.
  $country_fid = variable_get('fundraiser_country_profile_field', NULL);
  $state_fid = variable_get('fundraiser_state_profile_field', NULL);
  
  if (!$country_fid || !$state_fid) {
    $results = db_query('SELECT fid,name FROM {profile_fields} WHERE name IN ("profile_country", "profile_state")');

    while ($profile_field = db_fetch_object($results)) {
      if ($profile_field->name == 'profile_country') {
        variable_set('fundraiser_country_profile_field', $profile_field->fid);
        $country_fid = $profile_field->fid;
      }
      if ($profile_field->name == 'profile_state') {
        variable_set('fundraiser_state_profile_field', $profile_field->fid);
        $state_fid = $profile_field->fid;
      }
    }
  }
  return array('country' => $country_fid, 'state' => $state_fid);
}


/**
 * Helper function to update user profile fields based on contry/state selected on the donation form.
 *  
 *  @param $submission
 *  Webform submission object.
 *  
 */
function _fundraiser_update_profile_location($submission) {
  global $user;
  $fids = _fundraiser_get_profile_location_fields();  
  
  // get the ids of the country and state webform components for node
  $country_cid = db_result(db_query("SELECT cid FROM {webform_component} WHERE nid = %d AND form_key = 'country'", $submission->nid));
  $state_cid = db_result(db_query("SELECT cid FROM {webform_component} WHERE nid = %d AND form_key = 'state'", $submission->nid));
  
  $country_code = $submission->data[$country_cid]['value'][0];
  $state_code = $submission->data[$state_cid]['value'][0];
  
  if ($fids['country']) {
    $country_name = db_result(db_query('SELECT country_iso_code_2 FROM {uc_countries} WHERE country_id = %d', $country_code));
    $has_country = db_result(db_query('SELECT 1 FROM {profile_values} WHERE uid = %d AND fid = %d', $user->uid, $fids['country']));
    if ($has_country) {
      db_query("UPDATE {profile_values} SET value = '%s' WHERE uid = %d AND fid = %d", db_escape_string($country_name), $user->uid, $fids['country']);
    }
    else {
      db_query("INSERT INTO {profile_values} (fid, uid, value) VALUES (%d, %d, %s)", $fids['country'], $user->uid, $country_name);
    }

  }

  if ($fids['state']) {
    $state_name = db_result(db_query('SELECT zone_code FROM {uc_zones} WHERE zone_id = %d AND zone_country_id = %d', $state_code, $country_code));
    $has_state = db_result(db_query('SELECT 1 FROM {profile_values} WHERE uid = %d AND fid = %d', $user->uid, $fids['state']));
    if ($has_state) {
      db_query("UPDATE {profile_values} SET value = '%s' WHERE uid = %d AND fid = %d", $state_name, $user->uid, $fids['state']);
    }
    else {
      db_query("INSERT INTO {profile_values} (fid, uid, value) VALUES (%d, %d, '%s')", $fids['state'], $user->uid, $state_name);
    }

  }
}
