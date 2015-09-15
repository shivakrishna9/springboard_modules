<?php

class FundraiserLuminateDonation extends FundraiserDonationMigration {
  protected $dependencies = array('FundraiserLuminateDonor');

  public function __construct($arguments) {
    parent::__construct($arguments);

    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'fundraiser') . '/includes/csv/luminate-import-sample.csv';

    $this->source = new MigrateSourceCSV($file, $this->csvColumnNames(), array('enclosure' => '"', 'header_rows' => 1), $this->fields());

    // Add extra field mappings.
    $this->addFieldMapping('payment_method', 'PAYMENT_METHOD');
  }

  /**
   * Called before each record is migrated for validation and value
   * manipulation.
   *
   * @return bool
   *   FALSE if the row is to be skipped.
   */
  public function prepareRow($row) {
    // Slow down migration.
    sleep(1);

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // TODO: Validate payment date.
    $start = strtotime($row->{'PAYMENT_DATE'});

    $day = date('d', $start);
    $month = date('m');
    $year = date('Y');

    $start = strtotime(date("$month/$day/$year"));

    /**
     * Below are important and unique variables for the import process.
     *
     * These are used in the donation destination class to control
     * some of the standard donation processing.
     */
    $row->recurring_start_date = $start;
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
 
    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'fundraiser') . '/includes/csv/luminate-import-sample.csv';

    $this->source = new MigrateSourceCSV($file, $this->csvColumnNames(), array('enclosure' => '"', 'header_rows' => 1));
  }
}
