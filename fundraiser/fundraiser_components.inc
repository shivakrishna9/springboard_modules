<?php
function fundraiser_mapping_form($form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'mapping' => array(),
  );
  

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  
  $form['info'] = array(
  	'#value' => 'This form allows you to map the submitted form values to Drupal user and profile fields. When the donation form is submitted, the user\'s profile will be updated with the values from the form.',
  	'#weight' => 0, 
  );

	foreach ($node->webform['components']  as $cid =>$component){
		if($component['type'] != 'fieldset'){
		
			$form['mapping']['cid'][$cid] = array(
			'#type' => 'item',
			'#description' => t($component['name']." - (".$component['type'].")"),
			);
			
			$options = array(''=>'--Select One--');
			
			if(module_exists('profile')){
				$cat_array = profile_categories();
				foreach ($cat_array as $cat){
					$result = _profile_get_fields($cat['name']);
					for($i=0;$row = db_fetch_array($result); $i++){
						$options['Drupal Profile Fields'][$row['name']] = $row['title'];
					}
				}				
			}
			
			$options['User Fields'] = array(
				'email' => t("Email Address"),
			);
			
			$form['mapping']['type'][$cid] = array(
			'#type' => 'select',
			'#options' => $options,
			'#default_value' => $node->map[$cid],
			);
			
			$form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
					
		}
		 
	}

  return $form;
}

function fundraiser_mapping_form_submit($form, &$form_state){
	//dprint_r($form); die();
	$sql = "delete from fundraiser_component_map where nid=".$form['#node']->nid;
	db_query($sql);

	foreach($form_state['values']['mapping']['type'] as $key => $value) {
		if(!empty($value)){
			
			$sql = "insert into fundraiser_component_map set nid=".$form['#node']->nid.", cid=$key,map_id='$value'";
			db_query($sql);
		}
	}
	
	 drupal_set_message(t('Profile mapping has been saved'));
}

function fundraiser_recurring_admin(){
	
	$sql = "select fundraiser_recurring.*,users.name 
	from fundraiser_recurring
	inner join uc_orders on fundraiser_recurring.order_id=uc_orders.order_id
	inner join users on uc_orders.uid=users.uid 
	where fundraiser_recurring.order_id=".arg(3)."
	order by uc_orders.created desc";
	$resource = db_query($sql);
	
	$output = "<h3><a href='/admin/fundraiser/recurring/logs'>View Logs</a></h3><table border=1>
	<tr>
	
	<th>OrderID</th>
	<th>User Name</th>
	<th>Amount</th>

	<th>Last Charged</th>
	</tr>
	";
	
	for($i=1;$row = db_fetch_array($resource);$i++){
		$row['fee_amount'] = number_format($row['fee_amount'],2,'.','');
		$last_charge = date("Y-m-d H:i:s",strtotime('-'. '1 months', $row['next_charge']));
		$output.= "<tr>
		
		<td>$row[order_id]</td>
		<td>$row[name]</td>
		<td>$row[fee_amount]</td>
		<td>$last_charge</td>
		</tr>";
	}
	
	$output .= "</table>";
	
	return $output;

}

function fundraiser_recurring_master_order_overview(){
	
	$resource = db_query("select fundraiser_recurring.*, name  from {fundraiser_recurring}  
	inner join {uc_orders} on {fundraiser_recurring}.order_id={uc_orders}.order_id
	inner join {users} on {users}.uid={uc_orders}.uid
	where master_order_id=%d order by order_id asc",arg(3));
	
	$output = "<h2>Recurring Orders</h2>";
	$output .= "<table><thead>
	<tr>
	<th>Master Order ID</th>
	<th>OrderID</th>
	<th>User Name</th>
	<th>Amount</th>

	<th>Charge Date</th>
	</tr></thead>
	";
	

	
	if(empty($rows[1])){
		//we're actually looking at the order id and not the master_order_id
		$resource = db_query("select fundraiser_recurring.*, name, uc_orders.order_total  from fundraiser_recurring
		inner join uc_orders on fundraiser_recurring.order_id=uc_orders.order_id
		inner join users on users.uid=uc_orders.uid
		where master_order_id=(select master_order_id from fundraiser_recurring where order_id=%d)
		order by order_id asc",arg(3));
		unset($rows);
		for($i=1;$rows[$i] = db_fetch_array($resource);$i++){
			;
		}
	}
	
	for($i=1;$i<count($rows);$i++){
		if (($i % 2) == 0) {
			$class = 'even';
		}
		else {
			$class = 'odd';
		}
		$rows[$i]['fee_amount'] = number_format($rows[$i]['order_total'],2,'.','');
		if ($rows[$i]['gateway_resp'] == 'canceled') {
			$next_charge = 'Canceled';
		}
		else {
			$next_charge = date("Y-m-d H:i:s",$rows[$i]['next_charge']);
		}
		$output.= "<tr class='$class'>
		<td><a href='/admin/store/orders/".$rows[$i][master_order_id]."/recurring/edit'>".$rows[$i][master_order_id]."</a></td>
		<td>".$rows[$i][order_id]."</td>
		<td>".$rows[$i][name]."</td>
		<td>".$rows[$i][fee_amount]."</td>
		<td>$next_charge</td>
		</tr>";
	}
	
	$output .= "</table>";
	
	return $output;

}

function fundraiser_recurring_user_overview(){
	
	$resource = db_query("select master_order_id,fundraiser_recurring.order_id,uc_orders.order_total, next_charge,uid, gateway_resp
		from {fundraiser_recurring}
		inner join uc_orders on uc_orders.order_id=fundraiser_recurring.order_id
		where uc_orders.uid=%d
		order by master_order_id desc, fundraiser_recurring.order_id asc",arg(1));
	
	$output = "<h2>Recurring Payment Schedule</h2>";
	$output .= "<table>
	<thead>
	<tr>
	<th>OrderID</th>
	<th>Amount</th>
	<th>Charge Date</th>
	<th>Processed</th>
	<th>&nbsp;</th>
	</tr>
	</thead>
	";
	
	for($i=1;$rows[$i] = db_fetch_array($resource);$i++){
		;
	}
	
	for($i=1;$i<count($rows);$i++) {
		if (($i % 2) == 0) {
			$class = 'even';
		}
		else {
			$class = 'odd';
		}

		$next_charge = date("Y-m-d H:i:s",$rows[$i]['next_charge']);
		$processed = $rows[$i]['gateway_resp'] == "success" ? "Yes" : "No";
		$output.= "<tr class='$class'>
		<td>".$rows[$i][order_id]."</td>
		<td>".number_format($rows[$i][order_total], 2, '.', '')."</td>
		<td>$next_charge</td>
		<td>$processed</td>
		<td><a href='/user/".$rows[$i]['uid']."/recurring/edit/".$rows[$i][master_order_id]."'>Edit Payment Info</a></td>
		</tr>";
	}
	
	$output .= "</table>";
	
	return $output;

}

function fundraiser_recurring_edit_forms() {
	if (is_numeric(arg(3)))  {
		$master_order_id = arg(3);
		return _recurring_donation_edit_forms($master_order_id);
	}
}

function fundraiser_recurring_user_edit_forms() {
	if (is_numeric(arg(4)))  {
		$master_order_id = arg(4);
		return _recurring_donation_edit_forms($master_order_id);
	}
}

function _recurring_donation_edit_forms($master_order_id) {
	// we need an unprocessed child order for billing information
	$child_order_id = db_result(db_query("select order_id from {fundraiser_recurring} 
		where master_order_id = %d and gateway_resp is null limit 1", $master_order_id));
	$order = uc_order_load($child_order_id);
	$nid = $order->products[0]->nid;
	
	$output = '<div>' . _get_billing_information($master_order_id, $order) . '</div>';
	// render 3 forms
	$output .= drupal_get_form('fundraiser_donation_amount_form', $master_order_id, $nid);
	
	// 4/5/2010 - commented out for time being because authorize.net does not support updating
	// 						CIM profiles at this time
	$output .= drupal_get_form('fundraiser_billing_update_form', $master_order_id, $order);
	
	$output .= drupal_get_form('fundraiser_cancel_form', $master_order_id);
	
	return $output;
}