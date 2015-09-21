<?php

class FundraiserLuminateDonation extends FundraiserDonationMigration {
  protected $dependencies = array('FundraiserLuminateDonor');

  public function __construct($arguments) {
    parent::__construct($arguments);

    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'fundraiser') . '/includes/csv/luminate-import-full.csv';

    $this->source = new MigrateSourceCSV($file, $this->csvColumnNames(), array('enclosure' => '"', 'header_rows' => 1), $this->fields());

    $this->destination = new FundraiserLuminateDonationDestination();

    // Add extra field mappings.
    $this->removeFieldMapping('payment_method');
    $this->addFieldMapping('payment_method', 'PAYMENT_METHOD');
    $this->addFieldMapping('campaign_id');
    $this->addFieldMapping('market_source');
  }

  public function csvColumnNames() {
    $csv_cols = parent::csvColumnNames();

    $csv_cols[] = array('PAYMENT_METHOD', 'PAYMENT_METHOD');

    return $csv_cols;
  }

  /**
   * Called before each record is migrated for validation and value
   * manipulation.
   *
   * @return bool
   *   FALSE if the row is to be skipped.
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    /**
     * Below are important and unique variables for the import process.
     *
     * These are used in the donation destination class to control
     * some of the standard donation processing.
     */
    $row->recurring_start_date = strtotime($row->{'PAYMENT_DATE'});
    $row->create_sustainer_series = TRUE;

    // We don't want to fire the success hook.
    $row->donation_success = FALSE;

    // We do want to sync this donation.
    $row->send_to_queue = TRUE;
  }
}

class FundraiserLuminateDonor extends FundraiserUserMigration {
  public function __construct($arguments) {
     parent::__construct($arguments);
 
    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'fundraiser') . '/includes/csv/luminate-import-full.csv';

    $this->source = new MigrateSourceCSV($file, $this->csvColumnNames(), array('enclosure' => '"', 'header_rows' => 1));

    // Add extra field mappings.
    $this->addFieldMapping('sbp_phone', 'PHONE_NUMBER');
  }

  public function csvColumnNames() {
    $csv_cols = parent::csvColumnNames();

    $csv_cols[] = array('PHONE_NUMBER', 'PHONE_NUMBER');

    return $csv_cols;
  }
}
