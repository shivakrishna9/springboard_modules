<?php

/**
 * @file
 * Additional administration pages provided by Fundraiser module.
 */

/**
 * User mapping form.
 */
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
        'mail' => t("Email Address"),
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

  $resouce = db_query("
          SELECT
            fr.*,
            u.name
    FROM {fundraiser_recurring} fr
    INNER JOIN {uc_orders} uo
            ON fr.order_id = uo.order_id
    INNER JOIN {users} u
            ON o.uid = u.uid
    WHERE fr.order_id='%s'
    ORDER BY uo.created desc", arg(3));

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

  $resource = db_query('
    SELECT
      fr.*,
      u.name
    FROM {fundraiser_recurring} fr
    INNER JOIN {uc_orders} uo
      ON fr.order_id = uo.order_id
    INNER JOIN {users} u
      ON u.uid = uo.uid
    WHERE  fr.master_order_id = %d
    ORDER BY fr.order_id asc', arg(3));

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
    $resource = db_query('
      SELECT
        fr.*,
        u.name,
        uo.order_total
      FROM {fundraiser_recurring} fr
      INNER JOIN {uc_orders} uo
        ON fr.order_id = uo.order_id
      INNER JOIN {users} u
        ON u.uid = uo.uid
      WHERE fr.master_order_id = (SELECT master_order_id FROM {fundraiser_recurring} WHERE order_id=%d)
      ORDER BY fr.order_id asc', arg(3));
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
    $output.= "
      <tr class='$class'>
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

  $resource = db_query('
    SELECT
      fr.master_order_id,
      fr.order_id,
      uo.order_total,
      fr.next_charge,
      uo.uid,
      fr.gateway_resp
  FROM {fundraiser_recurring}
  INNER JOIN {uc_orders}
    ON uo.order_id = fr.order_id
  WHERE uo.uid=%d
  ORDER BY fr.master_order_id DESC, fr.order_id asc',arg(1));

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
    </thead>";

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
    $output.= "<tr class='$class'><td>" . $rows[$i][order_id]."</td>
    <td>".number_format($rows[$i][order_total], 2, '.', '')."</td>
    <td>$next_charge</td>
    <td>$processed</td>
    <td><a href='/user/".$rows[$i]['uid']."/recurring/edit/".$rows[$i][master_order_id]."'>Edit Payment Info</a></td>
    </tr>";
  }

  $output .= "</table>";
  return $output;
}

function _processed_recurring_charge_count($order_id) {
  return db_result(db_query("SELECT count(order_id) FROM {fundraiser_recurring} WHERE master_order_id = %d AND gateway_resp = 'success'", $order_id));
}

function _remaining_recurring_charge_count($order_id) {
  return db_result(db_query("select count(order_id) from {fundraiser_recurring} where master_order_id = %d
    and (gateway_resp IS NULL or gateway_resp = 'failed')", $order_id));
}

function fundraiser_billing_update_form(&$form_state, $master_order_id, &$order) {
  $form = array(
    '#tree' => TRUE,
      '#node' => $node,
      'fundraiser_recurring' => array(),
  );

  $form['uid'] = array(
     '#type' => 'hidden',
     '#value' => $order->uid,
  );

  $form['master_order_id'] = array(
    '#type' => 'hidden',
    '#value' => $master_order_id,
  );

   $form['billing_info'] = array(
     '#type' => "fieldset",
     '#title' => "Billing Information",
     '#description' => t('Use this form to update your billing information. Changes will apply to all future payments from today\'s date.'),
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
     '#weight' => -5,
   );

   $form['billing_info']['first_name'] = array(
     '#type' => 'textfield',
     '#title' => "First Name",
    '#required' => TRUE,
     '#default_value' => $order->billing_first_name,
   );
   $form['billing_info']['last_name'] = array(
     '#type' => 'textfield',
     '#title' => "Last Name",
    '#required' => TRUE,
     '#default_value' => $order->billing_last_name,
   );

    $form['billing_info']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email Address'),
      '#required' => TRUE,
      '#default_value' => $order->primary_email,
    );

   $form['billing_info']['address_1'] = array(
     '#type' => 'textfield',
     '#title' => 'Address Line 1',
     '#size' => 30,
    '#required' => TRUE,
     '#default_value' => $order->billing_street1,
   );

   $form['billing_info']['address_2'] = array(
     '#type' => 'textfield',
     '#title' => 'Address Line 2',
     '#size' => 30,
     '#default_value' => $order->billing_street2,
   );

   $form['billing_info']['city'] = array(
     '#type' => 'textfield',
     '#title' => 'City',
     '#size' => 30,
    '#required' => TRUE,
     '#default_value' => $order->billing_city,
   );

  $countries = uc_country_select(uc_get_field_name('country'), 840, NULL, 'name', uc_address_field_required('country'));
  $form['billing_info']['country'] = array(
     '#type' => 'select',
     '#title' => 'Country',
    '#options' => $countries['#options'],
    '#required' => TRUE,
     '#default_value' => $order->billing_country,
   );

   $zones = uc_zone_select(uc_get_field_name('zone'), $arg1->billing_zone, NULL, 840, 'name', uc_address_field_required('zone'));
   $form['billing_info']['state'] = array(
     '#type' => 'select',
     '#title' => 'State',
    '#options' => $zones['#options'],
    '#required' => TRUE,
     '#default_value' => $order->billing_zone,
   );

   $form['billing_info']['zipcode'] = array(
     '#type' => 'textfield',
     '#title' => 'Postal Code',
     '#size' => 5,
     '#maxlength' => 11,
    '#required' => TRUE,
     '#default_value' => $order->billing_postal_code,
   );

   $form['billing_info']['cc_info'] = array(
     '#type' => 'fieldset',
     '#title' => 'Credit Card Information',
   );

   $form['billing_info']['cc_info']['card_num'] = array(
     '#type' => 'textfield',
     '#title' => 'Card Number',
     '#size' => 20,
     '#required' => TRUE,
     '#maxlength' => 20,
   );

   $form['billing_info']['cc_info']['card_exp_date'] = array(
     '#type' => 'date',
     '#required' => TRUE,
     '#title' => 'Expiration Date',

   );

   $form['billing_info']['cc_info']['card_cvv'] = array(
     '#type' => 'textfield',
     '#title' => 'Security Code',
     '#required' => TRUE,
     '#size' => 5,
     '#maxlength' => 5,
   );

   $form['billing_info']['submit'] = array(
     '#type' =>'submit',
     "#value" => t('Save changes'),
   );

   return $form;
}

function fundraiser_billing_update_form_validate($form, &$form_state) {
  // validate credit card information
  $cc_number = $form_state['values']['billing_info']['cc_info']['card_num'];
  $cc_expiration_month =  $form_state['values']['billing_info']['cc_info']['card_exp_date']['month'];
  $cc_expiration_year = $form_state['values']['billing_info']['cc_info']['card_exp_date']['year'];
  $cc_cvv = $form_state['values']['billing_info']['cc_info']['card_cvv'];

  //validate cc number
  if ((variable_get('uc_credit_validate_numbers', TRUE) && !_valid_card_number($cc_number))
    || !ctype_digit($cc_number)) {
      form_set_error('billing_info][cc_info][card_num',"You have entered an invalid credit card number.");
  }

  // Validate the card expiration date.
  if (!_valid_card_expiration($cc_expiration_month, $cc_expiration_year)) {
    form_set_error('billing_info][cc_info][card_exp_date',t('The credit card you entered has expired.'));
  }

  // Validate the CVV Number
  if (variable_get('uc_credit_cvv_enabled', TRUE) && !_valid_cvv($cc_cvv)) {
    form_set_error('billing_info][cc_info][card_cvv',t('You have entered an invalid CVV number.'));
  }
}

function fundraiser_billing_update_form_submit($form, &$form_state) {
  global $user;

  $master_order_id = $form_state['values']['master_order_id'];
  $orders = _get_pending_recurring_orders($master_order_id);
  $orders_changed = array();
  $vault_guid = '';

  while($row = db_fetch_object($orders)) {
    // update ubercart orders first
    $order = uc_order_load($row->order_id);

    $vault_guid = $order->data['ref_id'];
    $orders_changed[] = $order->order_id;

    $order->primary_email = $form_state['values']['billing_info']['email'];
      $order->billing_first_name =  $form_state['values']['billing_info']['first_name'];
      $order->billing_last_name = $form_state['values']['billing_info']['last_name'];
      $order->billing_city = $form_state['values']['billing_info']['city'];
      $order->billing_street1 = $form_state['values']['billing_info']['address_1'];
      $order->billing_street2 = $form_state['values']['billing_info']['address_2'];
      $order->billing_postal_code = $form_state['values']['billing_info']['zipcode'];
      $order->billing_zone = $form_state['values']['billing_info']['state'];
      $order->billing_country = $form_state['values']['billing_info']['country'];
      $order->payment_method = 'credit';
      $order->payment_details = array(
        'cc_type' => '',
        'cc_owner' => '',
        'cc_number' => $form_state['values']['billing_info']['cc_info']['card_num'],
        'cc_start_month' => '',
        'cc_start_year' => '',
        'cc_exp_month' => $form_state['values']['billing_info']['cc_info']['card_exp_date']['month'],
        'cc_exp_year' => $form_state['values']['billing_info']['cc_info']['card_exp_date']['year'],
        'cc_issue' => '',
        'cc_cvv' => $form_state['values']['billing_info']['cc_info']['card_cvv'],
        'cc_bank' => '',
      );

    uc_order_save($order);
    uc_order_comment_save($order->order_id, $user->uid, "Billing information updated.");
  }

  // if the order contains a vault_guid. update the vault
  if (!empty($vault_guid)) {
    $vault_id = db_result(db_query("select vault_id from {uc_sage_vault} where guid = '%s'", $vault_guid));
    $data = array(
      'uid' => $order->uid,
      'billing_name' => $order->billing_first_name . ' ' . $order->billing_last_name,
      'billing_street1' => $order->billing_street1,
      'billing_street2' => $order->billing_street2,
      'billing_city' => $order->billing_city,
      'billing_zone' => $order->billing_zone,
      'billing_postal_code' => $order->billing_postal_code,
      'billing_country' => $order->billing_country,
      'cc_number' => $order->payment_details['cc_number'],
      'cc_exp_month' => $order->payment_details['cc_exp_month'],
      'cc_exp_year' => $order->payment_details['cc_exp_year'],
    );

    uc_sage_vault_update($vault_id, $data);
  }

  // Check for Salesforce Fundraiser module and update billing information
  if (module_exists('sf_donation')) {
    sf_donation_update_donation_billing($orders_changed);
  }

  drupal_set_message('Billing information for all pending orders has been updated.');
}

function fundraiser_donation_amount_form(&$form_state, $master_order_id, $nid) {
  // get the amount of a pending donation because the older ones may be out of date
  $donation_amount = db_result(db_query("select order_total from {uc_orders}
    where order_id = %d", $master_order_id));

  $form = array(
    '#tree' => TRUE,
      '#node' => $node,
      'fundraiser_recurring' => array(),
  );

   $form['amount'] = array(
     '#type' => "fieldset",
     '#title' => "Donation Amount",
     '#description' => 'Use this form to update the donation amount of future payments.',
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
     '#weight' => -5,
   );

  $form['amount']['master_order_id'] = array(
    '#type' => 'hidden',
    '#value' => $master_order_id,
  );

  $form['amount']['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid,
  );

  $amount = money_format('%i', $donation_amount);
  $form['amount']['fee_amount'] = array(
    '#type' => 'textfield',
    '#title' => "Donation Amount",
    '#default_value' => $amount,
    '#size' => 10,
    '#required' => TRUE,
    '#description' => "Your current donation amount is $$amount",
    '#field_prefix' => '$',
  );

  $form['amount']['submit'] = array(
    '#type' => 'submit',
     '#value' => t('Update donation amount'),
  );

  return $form;
}

function fundraiser_donation_amount_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['amount']['fee_amount'])) {
    form_set_error('amount', t('Donation amount must be a valid number.'));
  }

  // get minimum amount
  $minimum = db_result(db_query("select minimum_donation_amount from {fundraiser} where nid = %d", $form_state['values']['amount']['nid']));
  if ($form_state['values']['amount']['fee_amount'] <= $minimum) {
    $minimum = money_format('%i', $minimum);
    form_set_error('amount', t("Donation amount must be greater than $$minimum"));
  }
}

function fundraiser_donation_amount_form_submit($form, &$form_state) {
  global $user;
  $amount = $form_state['values']['amount']['fee_amount'];
  $master_order_id = $form_state['values']['amount']['master_order_id'];

  $orders = _get_pending_recurring_orders($master_order_id);
  $orders_changed = array();

  while($row = db_fetch_object($orders)) {
    // update ubercart orders first
    $order = uc_order_load($row->order_id);
    $old_amount = $order->order_total;
    $orders_changed[] = $row->order_id;

    // update the order total
    $order->order_total = $amount;
    // update the product price
    $order->products[0]->price = $amount;
    // save the order
    uc_order_save($order);
    uc_order_comment_save($order->order_id, $user->uid, "Donation amount changed from
      $old_amount to $amount by an administrator.", 'admin');
  }

  // call out to saleforce module if installed
  if (module_exists('sf_donation')) {
    sf_donation_update_donation_amount($orders_changed, $amount);
  }

  drupal_set_message("The amount of all future donations has been updated to $amount.");
}

function fundraiser_cancel_form(&$form_state, $master_order_id) {
  $form = array(
    '#tree' => TRUE,
      '#node' => $node,
      'fundraiser_recurring' => array(),
  );

  $form['cancel'] = array(
     '#type' => 'fieldset',
     '#title' => 'Cancel Future Donations',
     '#description' => 'Click the button to cancel all future payments.',
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
     '#weight' => -5,
   );

  $form['cancel']['reason'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason'),
    '#description' => t('Please enter a brief description of why you are cancelling. This will help us better serve you in the future.'),
    '#maxlength' => 500,
    '#cols' => 70,
    '#rows' => 3,
    '#resizable' => FALSE,
  );

  $form['cancel']['master_order_id'] = array(
    '#type' => 'hidden',
    '#value' => $master_order_id,
  );
  $form['cancel']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel all future payments'),
  );

  return $form;
}

function fundraiser_cancel_form_submit($form, &$form_state) {
  global $user;

  $master_order_id = $form_state['values']['cancel']['master_order_id'];
  $reason = $form_state['values']['cancel']['reason'];

  // get the ids of the child orders
  $result = _get_pending_recurring_orders($master_order_id);

  // cancel all child orders that are still pen
  $sql = "UPDATE {uc_orders} u inner join {fundraiser_recurring} f on f.order_id = u.order_id
  SET u.order_status = 'canceled' where f.master_order_id = %d and u.order_status = 'pending_future_payment'";
   db_query($sql, $master_order_id);

  // delete recurring information
  $sql = "update {fundraiser_recurring} set gateway_resp = 'canceled', cancellation_reason = '%s' where master_order_id = %d AND (gateway_resp IS NULL OR gateway_resp = 'failed')";
  db_query($sql, $reason, $master_order_id);

  $order_ids = array();
  // add a comment to the canceled orders
  while ($data = db_fetch_object($result)) {
    $order_ids[] = $data->order_id;
    uc_order_comment_save($data->order_id, $user->uid, 'Order canceled by an administrator. Reason: ' . $reason, 'order', 'canceled', FALSE);
  }

  // call out to saleforce module if installed
  if (module_exists('sf_donation')) {
    sf_donation_canel_recurring_donations($master_order_id);
  }

  drupal_set_message("All future payments cancelled.");

  //drupal_goto("user/$user->uid/recurring_overview");
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
  //             CIM profiles at this time
  // $output .= drupal_get_form('fundraiser_billing_update_form', $master_order_id, $order);

  $output .= drupal_get_form('fundraiser_cancel_form', $master_order_id);

  return $output;
}

function _get_billing_information($master_order_id, $order) {
  $state = uc_get_zone_code($order->billing_zone);
  $orders_remaining = _remaining_recurring_charge_count($master_order_id);
  $orders_processed = _processed_recurring_charge_count($master_order_id);
  $donation_amount = '$' . money_format('%i', $order->order_total);

  $billing_str = "";
  $billing_str .= "<div class='order-pane pos-left'>";
  $billing_str .= "<div class='order-pane-title'>Bill to:</div>";
  $billing_str .= "$order->billing_first_name $order->billing_last_name<br />";
  $billing_str .= "$order->billing_street1<br />";
  if (!empty($order->billing_street2)) {
    $billing_str .= "$order->billing_street2<br />";
  }
   $billing_str .= "$order->billing_city, $state $order->billing_postal_code</div>";
  $billing_str .= "<div class='order-pane pos-left'>";
  $billing_str .= "<div class='order-pane-title'>Customer Info:</div>";
  $billing_str .= "Customer Number: <a href='/user/$order->uid'>$order->uid</a><br />";
  $billing_str .= "Primary email:<br />$order->primary_email</div>";
  $billing_str .= "<div class='order-pane pos-left'>";
  $billing_str .= "<div class='order-pane-title'>Recurring Payment Info:</div>";
  $billing_str .= "Recurring Donation Amount: $donation_amount<br />";
  $billing_str .= "Recurring Charges Processed: $orders_processed <br />";
  $billing_str .= "Recurring Charges Remaining: $orders_remaining <br /></div>";
  $billing_str .= "<div style='clear:both;'>&nbsp;</div>";
  return $billing_str;
}

function _get_pending_recurring_orders($order_id) {
  // get the ids of the child orders
  $sql = "SELECT order_id FROM {fundraiser_recurring} WHERE master_order_id = %d AND
    (gateway_resp IS NULL OR gateway_resp = 'failed')";

   return db_query($sql, $order_id);
}

