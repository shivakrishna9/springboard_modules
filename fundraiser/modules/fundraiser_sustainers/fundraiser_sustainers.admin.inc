<?php

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

function fundraiser_sustainers_admin() {
  $headers = array('Master Donation ID', 'User', 'Amount', 'Charge Date', 'Status');
  $rows = array();
  // Get the data.
  $donation_sets = _fundraiser_sustainers_get_donation_sets();
  if ($donation_sets) {
    foreach ($donation_sets as $donation_set) {
      $this_donation = fundraiser_donation_get_donation($donation_set->master_order_id);
      $next_charge = _fundraiser_sustainers_get_donations_next_charge($donation_set->master_order_id);
      $next_charge = fundraiser_donation_get_donation($next_charge->order_id);
      $row = array();
      $row[] = l($donation_set->master_order_id, 'admin/config/system/fundraiser/recurring/' . $donation_set->master_order_id . '/view');
      $row[] = $this_donation->user->name;
      $row[] = '$' . money_format('%i', $next_charge->donation->donation_amount);
      $row[] = date('m/d/y', $donation_set->next_charge_date);
      $row[] = $donation_set->status;
      $rows[] = $row;
    }
  }
  $output = '<h2>Recurring Orders</h2>';
  $output .= theme('table', array('header' => $headers, 'rows' => $rows));
  return $output;
}

/**
 * Menu callback, provides administrators view into recurring donation information.
 */
function fundraiser_sustainers_admin_view($did) {
  $headers = array('Master ID', 'Donation ID', 'User', 'Amount', 'Charge Date', 'Status');
  $rows = array();
  // Get the data.
  $donation = fundraiser_donation_get_donation($did);
  $donation_set = _fundraiser_sustainers_get_donations_recurr_by_member($did);
  if ($donation_set) {
    $donation_set = _fundraiser_sustainers_get_donations_recurr_by_masterdid($donation_set[0]->master_order_id);
  }
  if ($donation_set) {
    foreach ($donation_set as $this_donation) {
      $this_donation = fundraiser_donation_get_donation($this_donation->order_id);
      $row = array();
      $row[] = $this_donation->recurring->master_order_id;
      $row[] = $this_donation->did;
      $row[] = $this_donation->user->name;
      $row[] = '$' . money_format('%i', $this_donation->donation->donation_amount);
      $row[] = date('m/d/y', $this_donation->recurring->next_charge);
      $row[] = $this_donation->recurring->gateway_resp;
      $rows[] = $row;
    }
  }
  $output = '<h2>Recurring Orders</h2>';
  $output .= theme('table', array('header' => $headers, 'rows' => $rows));
  return $output;
}

/**
 * Menu callback, provide way for admins to edit a recurring donation info directly.
 */
function fundraiser_sustainers_admin_edit($did) {
  if (is_numeric($did)) {
    $donation = fundraiser_donation_get_donation($did);
    $donation_set = _fundraiser_sustainers_get_donations_recurr_by_member($did);
    $variables = fundraiser_sustainers_edit_form($donation->user, $donation_set[0]->master_order_id);
    // TODO revise some of the theming here to account for this wierdness. It's too complex for sanity as is.
    // Add additional info about the donator for the admin.
    //$variables['billing_info'] .= 'Customer Number: <a href=\'/user/'.$donation->uid.'\'>'.$donation->user->name .' ('.$donation->uid.')</a><br />';
    //$variables['billing_info'] .= 'Primary email: '. $donation->user->name .'</div>';
    // Remove things we don't need the admin to use.
    $variables['orders'] = '';
    $variables['billing_form'] = '';
    return theme('sustainer_management_page', $variables);
  }
}