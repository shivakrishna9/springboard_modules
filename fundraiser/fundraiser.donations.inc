<?php

/**
 * @file
 * Administration page callbacks for the fundraiser module.
 */

/**
 * Display a table of recurring donations for admin users.
 */
function fundraiser_donations_admin() {
  $headers = array('Donation ID', 'User', 'Amount', 'Last Changed', 'Status');
  $rows = array();
  // Get the data.
  // Paginate donation sets. TODO
  $donations = _fundraiser_get_donations();
  if ($donations) {
    foreach ($donations as $donation) {
      $this_donation = fundraiser_donation_get_donation($donation->did);
      $row = array();
      $row[] = l($this_donation->did, 'admin/config/system/fundraiser/donations/' . $this_donation->did);
      $row[] = $this_donation->user->name;
      $row[] = '$' . money_format('%i', $this_donation->donation['amount']);
      $date = isset($this_donation->payment['receipt_date']) && !empty($this_donation->payment['receipt_date']) ?
        $this_donation->payment['receipt_date'] : $this_donation->last_changed;
      $row[] = date('m/d/y', $date);
      $row[] = $this_donation->status;
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
function fundraiser_donations_admin_view($did) {
  $headers = array('Master ID', 'Donation ID', 'User', 'Amount', 'Last Changed', 'Status');
  $rows = array();
  // Get the data.
  $donation = fundraiser_donation_get_donation($did);
  $output = '';
  //$output = 'VIEW A DONATION AS ADMIN'.print_r(array_keys($donation),1); //TODO set up view of donation.
  if (user_access('refund donations')) {
    $output .= fundraiser_show_refund_form($did);
  }
  return $output;
}