<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the fundraiser module
 */

/**
 * Clones a donation form and related fieldmaps
 */
function fundraiser_clone_donation_form($nid) {
	if (is_numeric($nid)) {
    global $user;

    $node = node_load($nid);
    if (isset($node->nid)) {
      $original_node = drupal_clone($node);

      $node->nid = NULL;
      $node->vid = NULL;
      $node->name = $user->name;
      $node->uid = $user->uid;
      $node->created = NULL;
      $node->menu = NULL;
      $node->book['mlid'] = NULL;
      $node->path = NULL;
      $node->files = array();
      $node->title = t('Clone of !title', array('!title' => $node->title));
			$node->internal_name = 'Clone of ' . $original_node->internal_name;
			$node->is_being_cloned = true;

      if (variable_get('clone_reset_'. $node->type, FALSE)) {
        $node_options = variable_get('node_options_'. $node->type, array('status', 'promote'));
        // Fill in the default values.
        foreach (array('status', 'moderate', 'promote', 'sticky', 'revision') as $key) {
          $node->$key = in_array($key, $node_options);
        }
      }
      node_save($node);
      $new_nid = $node->nid;

			// get profile map for the original donation form
			$result = db_query("select f.nid, f.cid, f.map_id, c.form_key from {fundraiser_component_map} f inner join
				{webform_component} c on c.cid = f.cid and c.nid = f.nid where f.nid = %d", $nid);
			
			// loop over mapped components and find new component in cloned donation form
			while ($data = db_fetch_object($result)) {
				$cid = db_result(db_query("SELECT cid FROM {webform_component} WHERE form_key = '%s' AND nid = %d", $data->form_key, $new_nid));
				if ($cid) {
					db_query("INSERT INTO {fundraiser_component_map} (nid, cid, map_id) VALUES (%d, %d, '%s')", $new_nid, $cid, $data->map_id);
				}
			}
			
			if (module_exists('sf_donation')) {
				sf_donation_clone_map($nid, $new_nid);
			}
			
			drupal_set_message("Donation form [$original_node->title] has been cloned succesfully.");
			drupal_goto('node/'. $new_nid .'/edit');
    }
  }
}


/** 
 * Form builder. Configure fundraiser.
 *
 */
function fundraiser_admin_settings() {
	
	$description = 'Use this option if you want donations to be exported to Salesforce.';
	$attributes = array();
	if (!module_exists('sf_donation')) {
		$description .= ' <span class="admin-disabled">The Salesforce Management Donations module must be enabled in order to use this option.</span>';
		$attributes['disabled'] = 'disabled';
	}
		
	$form['fundraiser_salesforce_integration'] = array(
		'#type' => 'checkbox',
		'#title' => t('Integrate with Salesforce'),
		'#default_value' => variable_get('fundraiser_salesforce_integration', 0),
		'#description' => t($description),
		'#attributes' => $attributes,
	);
	
	$form['fundraiser_all_countries'] = array(
		'#type' => 'checkbox',
		'#title' => t('Insert full list of countries into new donation forms'),
		'#default_value' => variable_get('fundraiser_all_countries', 0),
		'#description' => t('When checked, all countries will be inserted into the billing country field on new donations forms. When unchecked, on U.S and Canda will be inserted.'),
	);
	
	return system_settings_form($form);
}

/*
 * Creates a list of all donation forms on the site
 */
function fundraiser_donation_form_list() {
	$result = db_query("SELECT n.nid, n.title, f.internal_name, n.created, n.changed FROM {node} n INNER JOIN {fundraiser_gateway} f
		ON f.nid = n.nid ORDER BY n.changed DESC");
		
	$header = array('Title', 'Internal Name', 'Last Modified', '', '', '');
	$rows = array();
	
	while ($data = db_fetch_object($result)) {
		$edit_link = l('edit', 'node/' . $data->nid . '/edit');
		$view_link = l('view', drupal_get_path_alias('node/' . $data->nid));
		$clone_link = l('clone', 'admin/content/clone-form/'. $data->nid);
		$rows[] = array($data->title, $data->internal_name, date('m/d/Y g:i:s a', $data->changed), $edit_link, $view_link, $clone_link);
	}
	
	$output = theme('table', $header, $rows);
	return $output;
}

function fundraiser_monthly_report() {
	global $base_url;
	if ($_SESSION['fundraiser_report_filters']) {
		$start_date = $_SESSION['fundraiser_report_filters']['from_date'];
		$end_date = $_SESSION['fundraiser_report_filters']['to_date'];
		$sql = "select 
							u.order_id, 
							u.uid,
							u.order_status,
							s.sfid, 
							u.billing_first_name, 
							u.billing_last_name, 
							u.primary_email,
							u.billing_street1,
							u.billing_street2,
							u.billing_city,
							u.data,
							z.zone_code,
							u.billing_postal_code,
							c.country_iso_code_2,
							u.created as date, 
							u.order_total, 
							p.received,
							op.nid,
							op.title,
							fd.campaign_id,
							fd.market_source
						from 
							uc_orders u inner join fundraiser_webform_order s on s.order_id = u.order_id
							inner join {uc_payment_receipts} p on p.order_id = u.order_id
							inner join {uc_countries} c on c.country_id = u.billing_country
							inner join {uc_zones} z on z.zone_id = u.billing_zone 
							inner join {uc_order_products} op on op.order_id = u.order_id
							inner join {fundraiser_webform_order} wo on wo.order_id = u.order_id
							inner join {fundraiser_webform_data} fd on fd.sid = wo.sid and fd.nid = wo.webform_nid
							where u.order_status = 'payment_received' and u.created between %d and %d 
						union select 
							r.order_id,
							u.uid, 
							u.order_status,
							r.sf_opportunity_id, 
							u.billing_first_name,
							u.billing_last_name, 
							u.primary_email,
							u.billing_street1,
							u.billing_street2,
							u.billing_city,
							u.data,
							z.zone_code,
							u.billing_postal_code,
							c.country_iso_code_2,
							next_charge, 
							u.order_total, 
							p.received,
							op.nid,
							op.title,
							fd.campaign_id,
							fd.market_source
						from fundraiser_recurring r inner join uc_orders u on u.order_id = r.order_id 
							inner join {uc_payment_receipts} p on p.order_id = u.order_id
							inner join {uc_countries} c on c.country_id = u.billing_country
							inner join {uc_zones} z on z.zone_id = u.billing_zone
							inner join {uc_order_products} op on op.order_id = u.order_id
							inner join {fundraiser_webform_recurring_order} wo on wo.order_id = u.order_id
							inner join {fundraiser_webform_data} fd on fd.sid = wo.sid and fd.nid = wo.nid
							where u.order_status = 'payment_received' and r.next_charge between %d and %d order by order_id";
		$result = db_query($sql, $start_date, $end_date, $start_date, $end_date);
	}
	
	$header = array('Order Id', 'Opportunity Id', 'Uid', 'First Name', 'Last Name', 'Email', 'Address', 'Address 2', 'City', 'Country', 'State/Province', 'Postal Code', 'Payment Date', 'Amount', 'Donation Form', 'URL', 'Market Source', 'Campaign ID', 'CC Last Four', 'CC Type', 'CC Exp Month', 'CC Exp Year');
	$columns = array('order_id', 'sfid', 'billing_first_name', 'billing_last_name', 'primary_email', 'billing_street1', 'billing_street2', 'billing_city', 'country_iso_code_2', 'zone_code', 'received', 'order_total', 'title', 'nid');

	$rows = array();
	while ($data = db_fetch_object($result)) {
		$cc_data = decrypt_credit_card($data->data);
		$rows[] = array(
			$data->order_id, 
			$data->sfid ? $data->sfid : 'Not Exported', 
			$data->uid,
			$data->billing_first_name, 
			$data->billing_last_name,
			$data->primary_email,
			$data->billing_street1,
			$data->billing_street2,
			$data->billing_city,
			$data->country_iso_code_2,
			$data->zone_code,
			$data->billing_postal_code,
			date('m/d/Y g:i:s a', $data->received),
			'$'.number_format($data->order_total, 2),
			$data->title,
			$base_url . '/node/' . $data->nid,
			$data->market_source,
			$data->campaign_id,
			$cc_data['cc_number'],
			$cc_data['cc_type'],
			$cc_data['cc_exp_month'],
			$cc_data['cc_exp_year'],
		);
	}
	
	if ($_SESSION['fundraiser_report_filters']['export']) {
		$_SESSION['fundraiser_report_filters']['export'] = 0;
		
		foreach ($header as $i => $value) {
	  	$out .= '"' . $value . '",';
	  }

	  $out .="\n";

		// Add all values in the table
	 	foreach ($rows as $row) {
	 		foreach ($row as $i => $value) {
	 				$out .= '"' . $value . '",';
	 			}
	 		$out .="\n";
	 	}
	 	// Output to browser with appropriate mime type, you choose ;)
	 	header("Content-type: text/x-csv");
	 	header("Content-Disposition: attachment;filename=orders.csv");
	 	echo $out;
	 	exit;
	}
	else {	
		$output = drupal_get_form('fundraiser_report_form');
		if ($_SESSION['fundraiser_report_filters']) {
			$output .= '<h3>Date Range: ' . date('m/d/Y', $start_date) . ' - ' . date('m/d/Y', $end_date) . '</h3>';
			$output .= theme('table', $header, $rows);
		}
		else {
			$output .= '<h4>To get started, use the filter form to select the dates for your report.</h4>';
		}
		return $output;
	}
}

function fundraiser_report_form() {
	
	$from_date_default = $_SESSION['fundraiser_report_filters']['from_date'] ? $_SESSION['fundraiser_report_filters']['from_date'] : time();
	$to_date_default = $_SESSION['fundraiser_report_filters']['to_date'] ? $_SESSION['fundraiser_report_filters']['to_date'] : time();
	 
	$form['filters'] = array(
		'#type' => 'fieldset',
		'#title' => t('Filter'),
		'#collapsible' => TRUE,
	);
	
	$form['filters']['from_date'] = array(
		'#title' => t('From'),
		'#type' => 'date',
		'#description' => t('Select the beginning date of the report.'),
		'#weight' => 1,
		'#default_value' => array(
			'month' => date('n', $from_date_default),
			'day' => date('j', $from_date_default),
			'year' => date('Y', $from_date_default),
		),
	);
	
	$form['filters']['to_date'] = array(
		'#title' => t('To'),
		'#type' => 'date',
		'#description' => t('Select the end date of the report.'),
		'#weight' => 2,
		'#default_value' => array(
			'month' => date('n', $to_date_default),
			'day' => date('j', $to_date_default),
			'year' => date('Y', $to_date_default),
		),
	);
	
	$form['filters']['export'] = array(
		'#type' => 'checkboxes',
		'#options' => array(1 => 'Export to .csv file.'),
		'#weight' => 3,
	);
	
	$form['filters']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
		'#weight' => 4,
	);
	
	return $form;
}

function fundraiser_report_form_submit($form, &$form_state) {
	
	$export = array_shift($form_state['values']['export']);
	
	$from_year = $form_state['values']['from_date']['year'];
	$to_year = $form_state['values']['to_date']['year']; 
	
	$from_month = $form_state['values']['from_date']['month']; 
	$to_month = $form_state['values']['to_date']['month'];
		
	$from_day = $form_state['values']['from_date']['day'];	
	$to_day = $form_state['values']['to_date']['day'];

	$from_date = mktime(0, 0, 0, $from_month, $from_day, $from_year);
	$to_date = mktime(23, 59, 59, $to_month, $to_day, $to_year);	
	
	$_SESSION['fundraiser_report_filters'] = array(
		'from_date' => $from_date,
		'to_date' => $to_date,
		'export' => $export,
	);
}

// Utility function to decrypt stored credit card information
function decrypt_credit_card($data) {
	$key = uc_credit_encryption_key();
  $crypt = new uc_encryption_class;
	$cc = unserialize($data);
	$cc_data = unserialize($crypt->decrypt($key, $cc['cc_data']));
	$cc_data['cc_number'] = substr($cc_data['cc_number'], -4);
	return $cc_data;
}