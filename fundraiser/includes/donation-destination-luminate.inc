<?php

/**
 * @file
 * Migration support for Fundraiser donations.
 */

/**
 * Destination class for a Fundraiser Luminate donation.
 */
class FundraiserLuminateDonationDestination extends FundraiserDonationDestination {

  public function __construct() {
    parent::__construct();
  }

  public function complete($donation, $row) {
    parent::complete($donation, $row);

    // Create the fundraiser sustainers series entity.
    if ($donation->recurring->master_did == $donation->did) {
      $payment_timestamp = strtotime($row->PAYMENT_DATE);

      $fundraiser_sustainers_series_data = array(
        'did' => $donation->did,
        'uid' => $donation->uid,
        'amount' => $donation->donation['amount'],
        'installments' => _fundraiser_sustainers_calculate_installments($donation),
        'luminate_start_date' => date('Y-m-d', $payment_timestamp),
        // Hard-code campaign ID because it isn't available at this point.
        'luminate_campaign_id' => '701U0000000tCQJIA2',
        'luminate_recurring_amount_rounded' => round($donation->donation['amount']),
        'luminate_installment_period' => LUMINATE_INSTALLMENT_PERIOD,
        'luminate_day_of_month' => date('d', $payment_timestamp),
      );

      $fundraiser_sustainers_series = entity_create('fundraiser_sustainers_series', $fundraiser_sustainers_series_data);
      $fundraiser_sustainers_series->save();
    }
  }

  /**
   * Returns a list of fields available to be mapped.
   *
   * @param Migration $migration
   *  Optionally, the migration containing this destination.
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields($migration = NULL) {
    $fields = parent::fields();

    $fields['campaign_id'] = 'campaign_id';
    $fields['market_source'] = 'market_source';

    return $fields;
  }

}
