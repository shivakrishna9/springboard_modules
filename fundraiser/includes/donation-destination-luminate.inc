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
