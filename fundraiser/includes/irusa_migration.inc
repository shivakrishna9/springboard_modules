<?php

class IrusaMigration extends FundraiserMigration {
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup.
    parent::__construct($arguments);
  }

  public function csvColumnNames() {
    $cols = array(
      // A unique id will usually be required for matching the rows to the payment reference token.
      'UNIQUE_ID',
      'LCRM_RECURRING_DONATION_ID',
      'NPSP_RECURRING_DONATION_ID',
      'CONTACT_ACCOUNT_ID',
      'CONTACT_ID',
      'PAYMENT_METHOD',
      'PAYMENT_DATE',
      'FREQUENCY',
      'PAYMENT_AMOUNT',
      'FIRST_NAME',
      'LAST_NAME',
      'EMAIL_ADDRESS',
      'PRIMARY_ADDRESS_LINE_1',
      'PRIMARY_ADDRESS_LINE_2',
      'PRIMARY_ADDRESS_CITY',
      'PRIMARY_ADDRESS_STATE',
      'PRIMARY_ADDRESS_ZIP_CODE',
      'PRIMARY_ADDRESS_COUNTRY',
      'PAYMENT_STATUS', // active
      'CREDIT_CARD_TYPE',
      'CREDIT_CARD_NUMBER',
      'EXPIRE_MONTH',
      'EXPIRE_YEAR',
      // This is usually provided separately from the main data dump by the payment processor.
      'PAYMENT_REFERENCE_TOKEN',
    );

    foreach ($cols as $col) {
      $csv_cols[] = array($col, $col);
    }

    return $csv_cols;
  }

}

/**
 * An example migration class.
 *
 * You probably want to copy this instead of extending this class as it
 * makes assumptions on the data and mapping options.
 */
class IrusaDonationMigration extends IrusaMigration {
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup
    parent::__construct($arguments);

    $this->description = t('Migrate donations');

    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'fundraiser') . '/includes/irusa_sustainers.csv';

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceCSV($file, $this->csvColumnNames(), array('enclosure' => '"', 'header_rows' => 1), $this->fields());

    // The source ID here is the one retrieved from each data item in the XML file, and
    // used to identify specific items
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'csvrownum' => array(
          'type' => 'int',
          'not null' => TRUE,
        )
      ),
      FundraiserDonationDestination::getKeySchema()
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->destination = new FundraiserDonationDestination();

    $this->addFieldMapping('recurs_monthly')->defaultValue(FALSE);

    // Add mappings.
    $this->addFieldMapping('mail', 'EMAIL_ADDRESS');
    $this->addFieldMapping('created')->defaultValue(date('c'));
    $this->addFieldMapping('amount', 'PAYMENT_AMOUNT');
    $this->addFieldMapping('payment_method', 'payment_method');
    $this->addFieldMapping('payment_fields', 'payment_fields');
    $this->addFieldMapping('cardonfile', 'cardonfile');
    $this->addFieldMapping('first_name', 'FIRST_NAME');
    $this->addFieldMapping('last_name', 'LAST_NAME');
    $this->addFieldMapping('state', 'PRIMARY_ADDRESS_STATE');
    $this->addFieldMapping('address', 'PRIMARY_ADDRESS_LINE_1');
    $this->addFieldMapping('address_line_2', 'PRIMARY_ADDRESS_LINE_2');
    $this->addFieldMapping('city', 'PRIMARY_ADDRESS_CITY');
    $this->addFieldMapping('zip', 'PRIMARY_ADDRESS_ZIP_CODE');
    $this->addFieldMapping('country', 'PRIMARY_ADDRESS_COUNTRY');
    $this->addFieldMapping('data', 'data');
    $this->addFieldMapping('currency')->defaultValue('USD');
    $this->addFieldMapping('status')->defaultValue(1);
 
    // Required defaults
    $this->addFieldMapping('nid')
      ->description(t('Enter the NID of the target donation form.'))
      ->issuePriority(MigrateFieldMapping::ISSUE_PRIORITY_BLOCKER);

    $this->addFieldMapping('gateway') 
      ->description(t('Enter the ID of the commerce gateway to process these donations.'))
      ->issuePriority(MigrateFieldMapping::ISSUE_PRIORITY_BLOCKER);
  }

  public function fields() {
    return array(
      'payment_method' => 'Payment method set in prepareRow()',
      'payment_fields' => 'An array of payment field data made during prepareRow().',
      'cardonfile' => 'An array of card data for a cardonfile record.',
      'data' => 'Data for the donation data array constructed during prepareRow().',
    );
  }

  /**
   * Give the calling migration a shot at manipulating, and possibly rejecting,
   * the source row.
   *
   * @return bool
   *  FALSE if the row is to be skipped.
   */
  public function prepareRow($row) {
    // Slow down migration
    sleep(1);

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // TODO: Some with been ACH so this should not be hardcoded to 'credit'
    $row->payment_method = 'credit'; 
    $row->payment_fields = $this->paymentFields($row);

    // Prepare data for card on file.
    $row->cardonfile = $this->cardOnFile($row);

    // Set the start date of the series to the payment date in the import file.
    $start = strtotime($row->{'PAYMENT_DATE'});

    $day = date('d', $start);
    $month = date('m');
    $year = date('Y');

    $start = strtotime(date("$month/$day/$year"));
    $row->recurring_start_date = $start;
    $row->create_sustainer_series = TRUE;

    // We don't want to fire the success hook.
    $row->donation_success = FALSE;

    // We do want to sync this donation.
    $row->send_to_queue = TRUE;

    /**
     * Add an array of submission values.
     *
     * This would be the area to add additional submission values, like market source fields.
     */
    $row->submission = array();
    $row->submission['first_name'] = $row->{'FIRST_NAME'};
    $row->submission['last_name'] = $row->{'LAST_NAME'};
    $row->submission['address'] = $row->{'PRIMARY_ADDRESS_LINE_1'};
    $row->submission['address_line_2'] = $row->{'PRIMARY_ADDRESS_LINE_2'};
    $row->submission['city'] = $row->{'PRIMARY_ADDRESS_CITY'};
    $row->submission['state'] = $row->{'PRIMARY_ADDRESS_STATE'};
    $row->submission['zip'] = $row->{'PRIMARY_ADDRESS_ZIP_CODE'};
    $row->submission['country'] = $row->{'PRIMARY_ADDRESS_COUNTRY'};

    return TRUE;
  }

  public function paymentFields($row) {
    if ($row->payment_method == 'credit') {
      module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
      $fields['credit'] = array();
      // Card number.
      $fields['credit']['card_number'] = $row->{'CREDIT_CARD_NUMBER'};

      // Card exp date.
      $dates = explode('/', $row->{'Card Expiration Date'});
      $fields['credit']['card_expiration_month'] = $row->{'EXPIRE_MONTH'};
      $fields['credit']['card_expiration_year'] = $row->{'EXPIRE_YEAR'};

      // Card types, payflow records Discover.
      $types = array('discover' => 'Discover') + commerce_payment_credit_card_types();
      $type = array_search($row->{'CREDIT_CARD_TYPE'}, $types);
      $fields['credit']['card_type'] = !empty($type) ? $type : '';
    }

    return $fields;
  }

  public function cardOnFile($row) {
    $cardonfile = array(
      'remote_id' => $row->{'PAYMENT_REFERENCE_TOKEN'},
      'card_type' => $row->payment_fields['credit']['card_type'],
      'card_name' => substr($row->{'FIRST_NAME'}, 0, 120) . ' ' . substr($row->{'LAST_NAME'}, 0, 120),
      'card_number' => $row->payment_fields['credit']['card_number'],
      'card_exp_month' => $row->payment_fields['credit']['card_expiration_month'],
      'card_exp_year' => $row->payment_fields['credit']['card_expiration_year'],
      'status' => 1,
      'instance_default' => 1,
    );

    return $cardonfile;
  }

  public function complete($entity, stdClass $row) {
    $did = $entity->did;
    
    // Contact record.
    $recurring_donation_record = array(
      'sfid' => $row->{'NPSP_RECURRING_DONATION_ID'},
      'module' => 'fundraiser_sustainers_series',
      'delta' => 'fundraiser_sustainers_series',
      'drupal_id' => $did,
      'object_type' => 'npe03__Recurring_Donation__c',
    );
    salesforce_sync_save_map($recurring_donation_record);

    // Queue up the individual donations
    $recurring = _fundraiser_sustainers_get_donations_recurr_by_masterdid($did);
    if (!empty($recurring)) {
      foreach ($recurring as $recur) {
        drush_print_r($recur);
        salesforce_donation_fundraiser_donation_success($recur);
      }
    }
  }

  public function completeRollback($entity_ids) {
    // Remove sync map entries after a rollback.
    foreach ($entity_ids as $entity_id) {
      db_query("DELETE FROM {salesforce_sync_map} WHERE drupal_id = :drupal_id AND module = 'fundraiser_sustainers_series' AND delta = 'fundraiser_sustainers_series'", array(':drupal_id' => $entity_id));
    }
  }

}

/**
 * Base class for creating user accounts from the list of donations.
 */
class IrusaDonorMigration extends IrusaMigration {
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup
    parent::__construct($arguments);

    $this->description = t('Migrate Donation Users');
 
    $file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'fundraiser') . '/includes/irusa_sustainers.csv';

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceCSV($file, $this->csvColumnNames(), array('enclosure' => '"', 'header_rows' => 1));

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->destination = new MigrateDestinationUser();

    // Add the map
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'csvrownum' => array(
          'type' => 'int',
          'not null' => TRUE,
        )
      ),
      MigrateDestinationUser::getKeySchema()
    );

    // This is a shortcut you can use to mark several destination fields as DNM
    // at once.
    $this->addUnmigratedDestinations(array(
      'access',
      'data',
      'language',
      'login',
      'picture',
      'role_names',
      'signature',
      'signature_format',
      'theme',
      'timezone',
    ));

    $this->addFieldMapping('roles')->defaultValue(DRUPAL_AUTHENTICATED_RID);

    // Add mappings
    $this->addFieldMapping('mail', 'EMAIL_ADDRESS');
    $this->addFieldMapping('name', 'EMAIL_ADDRESS');
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('sbp_first_name', 'FIRST_NAME');
    $this->addFieldMapping('sbp_last_name', 'LAST_NAME');
    $this->addFieldMapping('sbp_state', 'PRIMARY_ADDRESS_STATE');
    $this->addFieldMapping('sbp_address', 'PRIMARY_ADDRESS_LINE_1');
    $this->addFieldMapping('sbp_address_line_2', 'PRIMARY_ADDRESS_LINE_2');
    $this->addFieldMapping('sbp_city', 'PRIMARY_ADDRESS_CITY');
    $this->addFieldMapping('sbp_zip', 'PRIMARY_ADDRESS_ZIP_CODE');
    $this->addFieldMapping('sbp_country', 'PRIMARY_ADDRESS_COUNTRY');

    $this->addFieldMapping('sbp_salesforce_contact_id', 'CONTACT_ID')
      ->description(t('The Salesforce Id of the contact associated with the recurring donation'));
    $this->addFieldMapping('sbp_salesforce_account_id', 'CONTACT_ACCOUNT_ID')
      ->description(t('The Salesforce Id of the account associated with the recurring donation'));

  }

  /**
   * Give the calling migration a shot at manipulating, and possibly rejecting,
   * the source row.
   *
   * @return bool
   *  FALSE if the row is to be skipped.
   */
  public function prepareRow($row) {
    // $migration = Migration::currentMigration();
    // $migration->saveMessage($row->{'EMAIL_ADDRESS'});

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $account = user_load_by_mail($row->{'EMAIL_ADDRESS'});

    drush_print_r($row);

    // We do not want to mess with existing users.
    if (!empty($account)) {
      return FALSE;
    }

    return TRUE;
  }

  public function prepare(stdClass $account, stdClass $row) {
    $account->is_new = TRUE;

    // If the email registration module is enabled, clean up
    // the username based on that module's rules.
    if (module_exists('email_registration')) {
      $username = preg_replace('/@.*$/', '', trim($account->name));
      $username = email_registration_cleanup_username($username);
    } 
    else {
      $username = preg_replace('/[^\x{80}-\x{F7} a-z0-9@_.\'-]/i', '-', trim($account->name));
      $username = substr($username, 0, USERNAME_MAX_LENGTH);
    }
    
    $account->name = $username;
  }

  public function complete($entity, stdClass $row) {
    // Since we'll have the user's contact and account ids during the import
    // use this opportunity to save them to the salesforce_sync_map table.
    $uid = $entity->uid;
    
    // Contact record.
    $contact_record = array(
      'sfid' => $row->{'CONTACT_ID'},
      'module' => 'user',
      'delta' => 'user',
      'drupal_id' => $uid,
      'object_type' => 'Contact',
    );
    salesforce_sync_save_map($contact_record);

    // Account record.
    $account_record = array(
      'sfid' => $row->{'CONTACT_ACCOUNT_ID'},
      'module' => 'user',
      'delta' => 'user',
      'drupal_id' => $uid,
      'object_type' => 'Account',
    );
    salesforce_sync_save_map($account_record);
  }

  public function completeRollback($entity_ids) {
    // Remove sync map entries after a rollback.
    foreach ($entity_ids as $entity_id) {
      db_query("DELETE FROM {salesforce_sync_map} WHERE drupal_id = :drupal_id AND module = 'user' AND delta = 'user'", array(':drupal_id' => $entity_id));

    }
  }
  
}
