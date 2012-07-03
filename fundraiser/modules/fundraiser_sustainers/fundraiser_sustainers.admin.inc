<?php

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

/**
 * Display a table of recurring donations for admin users.
 */
function fundraiser_sustainers_admin() {
  $headers = array('Master Donation ID', 'User', 'Amount', 'Charge Date', 'Status');
  $rows = array();
  // Get the data.
  $donation_sets = _fundraiser_sustainers_get_donation_sets();
  if ($donation_sets) {
    foreach ($donation_sets as $donation_set) {
      $this_donation = fundraiser_donation_get_donation($donation_set->master_order_id); //TODO rename.
      $next_charge = _fundraiser_sustainers_get_donations_next_charge($donation_set->master_order_id);
      $next_charge = fundraiser_donation_get_donation($next_charge->order_id);
      $row = array();
      $row[] = l($donation_set->master_order_id, 'admin/config/system/fundraiser/recurring/' . $donation_set->master_order_id . '/view');
      $row[] = $this_donation->user->name;
      $row[] = '$' . money_format('%i', $next_charge->donation['amount']);
      $row[] = date('m/d/y', $donation_set->next_charge_date);
      $row[] = $donation_set->status;
      $rows[] = $row;
    }
  }
  $output = '<h2>All Recurring Orders</h2>';
  $output .= theme('table', array('header' => $headers, 'rows' => $rows));
  return $output;
}

/**
 * Display a table of recurring donations for admin users by master donation id.
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
      $row[] = '$' . money_format('%i', $this_donation->donation['amount']);
      $row[] = date('m/d/y', $this_donation->recurring->next_charge);
      $row[] = $this_donation->recurring->gateway_resp;
      $rows[] = $row;
    }
  }
  $output = '<h2>Recurring Orders For #' . $donation_set[0]->master_order_id . '</h2>';
  $output .= theme('table', array('header' => $headers, 'rows' => $rows));
  return $output;
}

/**
 * Menu callback, provide way for admins to edit a recurring donation info directly.
 */
function fundraiser_sustainers_admin_edit($did) {
  if (is_numeric($did)) {
    return fundraiser_sustainers_edit_form($did);
  }
}