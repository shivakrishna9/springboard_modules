<?php
// $Id: fundraiser.fields.inc $

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
 */
function fundraiser_create_webform_components($nid, $donation_amounts, $show_other_amount, $min_donation_amount) {
	// market source id
	$market_source = array();
	$market_source['nid'] = $nid;
	$market_source['pid'] = 0; 
	$market_source['value'] = 'default_ms';
	$market_source['form_key'] = 'ms'; 
	$market_source['name'] = 'Market Source';
	$market_source['type'] = 'hidden';  
	$market_source['extra'] = array(
		'description' => '',
	);
	$market_source['mandatory'] = 1; 
	$market_source['weight'] = 20; 
	$market_source['email'] = 1;
	
	webform_component_insert($market_source);
	
	// if the sf_donation is installed, try to get the default campaign
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
	
	webform_component_insert($campaign);

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
	
	webform_component_insert($referrer);
	
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
	
	webform_component_insert($initial_referrer);
	
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
	
	$id = webform_component_insert($donation);

	// determine the type of amount field to output
	if ($donation_amounts) {
		
		$formatted_amounts = _create_amount_options($donation_amounts, $show_other_amount);
		
		if ($show_other_amount) {		
			// other amount
			$other_amount = array();
			$other_amount['nid'] = $nid;
			$other_amount['pid'] = $id; 
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

			webform_component_insert($other_amount);
		}

		// ask amounts
		$amount = array();
		$amount['nid'] = $nid;
		$amount['pid'] = $id; 
		$amount['form_key'] = 'amount'; 
		$amount['name'] = 'Please select your tax-deductable gift amount below';
		$amount['type'] = 'select';  
		$amount['extra'] = array(
			'description' => '',
			'items' => $formatted_amounts,
			'multiple' => 0,
		);
		$amount['mandatory'] = 1; 
		$amount['weight'] = 0; 
		$amount['email'] = 1;
		$amounts = explode(',', $donation_amounts);
		$amount['value'] = array_shift($amounts); // select first value

		webform_component_insert($amount);
	}
	else { // plain old textbox for donation amount
		$amount = array();
		$amount['nid'] = $nid;
		$amount['pid'] = $id; 
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

		webform_component_insert($amount);
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
	
	$id = webform_component_insert($donor_information);
	
	// first name
	$first_name = array();
	$first_name['nid'] = $nid;
	$first_name['pid'] = $id; 
	$first_name['form_key'] = 'first_name'; 
	$first_name['name'] = 'First Name';
	$first_name['type'] = 'textfield';  
	$first_name['extra'] = array(
		'description' => '',
	);
	$first_name['mandatory'] = 1; 
	$first_name['weight'] = 0; 
	$first_name['email'] = 1;
	
	webform_component_insert($first_name);
	
	// last name
	$last_name = array();
	$last_name['nid'] = $nid;
	$last_name['pid'] = $id; 
	$last_name['form_key'] = 'last_name'; 
	$last_name['name'] = 'Last Name';
	$last_name['type'] = 'textfield';  
	$last_name['extra'] = array(
		'description' => '',
	);
	$last_name['mandatory'] = 1; 
	$last_name['weight'] = 1; 
	$last_name['email'] = 1;
	
	webform_component_insert($last_name);

	// email
	$email = array();
	$email['nid'] = $nid;
	$email['pid'] = $id; 
	$email['form_key'] = 'email'; 
	$email['name'] = 'Email';
	$email['type'] = 'textfield';  
	$email['extra'] = array(
		'description' => '',
	);
	$email['mandatory'] = 1; 
	$email['weight'] = 2; 
	$email['email'] = 1;
	
	webform_component_insert($email);
	
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
	
	$id = webform_component_insert($billing_information);
	
	// billing address
	$billing_address = array();
	$billing_address['nid'] = $nid;
	$billing_address['pid'] = $id; 
	$billing_address['form_key'] = 'billing_address'; 
	$billing_address['name'] = 'Address';
	$billing_address['type'] = 'textfield';  
	$billing_address['extra'] = array(
		'description' => '',
	);
	$billing_address['mandatory'] = 1; 
	$billing_address['weight'] = 3; 
	$billing_address['email'] = 1;
	
	webform_component_insert($billing_address);

	// billing address 2
	$billing_address_2 = array();
	$billing_address_2['nid'] = $nid;
	$billing_address_2['pid'] = $id; 
	$billing_address_2['form_key'] = 'billing_address_2'; 
	$billing_address_2['name'] = 'Address Line 2';
	$billing_address_2['type'] = 'textfield';  
	$billing_address_2['extra'] = array(
		'description' => '',
	);
	$billing_address_2['mandatory'] = 0; 
	$billing_address_2['weight'] = 4; 
	$billing_address_2['email'] = 1;
	
	webform_component_insert($billing_address_2);

	// city
	$billing_city = array();
	$billing_city['nid'] = $nid;
	$billing_city['pid'] = $id; 
	$billing_city['form_key'] = 'billing_city'; 
	$billing_city['name'] = 'City';
	$billing_city['type'] = 'textfield';  
	$billing_city['extra'] = array(
		'description' => '',
	);
	$billing_city['mandatory'] = 1; 
	$billing_city['weight'] = 5; 
	$billing_city['email'] = 1;
	
	webform_component_insert($billing_city);

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
	$billing_country['pid'] = $id; 
	$billing_country['form_key'] = 'billing_country'; 
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
	
	webform_component_insert($billing_country);

	// state
	$zones = uc_zone_select(uc_get_field_name('zone'), $arg1->billing_zone, NULL, $us, 'name', uc_address_field_required('zone'));
	
	// build option string for states
	$options = '';
	foreach($zones['#options'] as $key => $value) {
		$options .= $key .'|'. $value . "\n";
	}
	
	$billing_state = array();
	$billing_state['nid'] = $nid;
	$billing_state['pid'] = $id; 
	$billing_state['form_key'] = 'billing_state'; 
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
	
	webform_component_insert($billing_state);
	
	// zip code
	$billing_zipcode = array();
	$billing_zipcode['nid'] = $nid;
	$billing_zipcode['pid']= $id; 
	$billing_zipcode['form_key'] = 'billing_zipcode'; 
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
	
	webform_component_insert($billing_zipcode);
	
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
	
	$id = webform_component_insert($credit_card_information);

	// credit card number
	$card_number = array();
	$card_number['nid'] = $nid;
	$card_number['pid'] = $id; 
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
	
	webform_component_insert($card_number);
	
	// expiration date
	$expiration_date = array();
	$expiration_date['nid'] = $nid;
	$expiration_date['pid'] = $id; 
	$expiration_date['form_key'] = 'card_expiration_date'; 
	$expiration_date['name'] = 'Expiration Date';
	$expiration_date['type'] = 'date';  
	$expiration_date['value'] = '1 January';
	$expiration_date['extra'] = array(
		'description' => '',
		'year_textfield' => 0,
		'year_start' => date("Y"),
		'year_end' => date("Y") + 6,
		'timezone' => 'site',
		'check_daylight_savings' => 0,
	);
	$expiration_date['mandatory'] = 1; 
	$expiration_date['weight'] = 1; 
	$expiration_date['email'] = 0;
	
	webform_component_insert($expiration_date);

	// cvv
	$card_cvv = array();
	$card_cvv['nid'] = $nid;
	$card_cvv['pid']= $id; 
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
	
	webform_component_insert($card_cvv);

	// recurring donation
	$recurs = array();
	$recurs['nid'] = $nid;
	$recurs['pid']= $id; 
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
	
	webform_component_insert($recurs);
}