<?php
/**
 * Class FundraiserTestHelper includes common code needed by fundraiser.test and submission.test
 */

class fundraiserTestHelper extends DrupalWebTestCase {

  protected $_fundraiser_node;
  protected $_user;

  function setUp() {
    parent::setUp(
      'webform',
      'profile',
      'token',
      'uc_store',
      'uc_order',
      'uc_cart',
      'uc_product',
      'uc_payment',
      'uc_credit',
      'ca',
      'test_gateway',
      'fundraiser',
      'fundraiser_uc_gateway',
      'market_source'
    );

    // Install any additional modules specified by helper modules
    $modules = func_get_args();
    foreach ($modules as $module) {
      drupal_install_modules(array($module));
    }

    $permissions = array(
      'administer blocks',
      'access content',
      'administer nodes',
      'create webform content',
      'edit any webform content',
      'access all webform results',
      'edit all webform submissions',
      'delete all webform submissions',
      'create donation form',
    );

    // Create and log in our privileged user.
    $this->_user =  $this->drupalCreateUser($permissions);

    // Create ubercart settings
    $this->setupPayment();
    $this->setupProfileFields();
    $this->setProfileFields();
  }

  function tearDown() {
    // Delete nodes
    $result = db_query('SELECT nid FROM {node}');
    while ($node = db_fetch_array($result)) {
      node_delete($node['nid']);
    }

    parent::tearDown();
  }


  /**
   * Performs the installation and configuration of payment methods
   */
  function setupPayment() {
    variable_set('uc_pg_test_gateway_enabled', 1);
    variable_set('uc_pg_test_gateway_cc_txn_type', 'auth_capture');
    variable_set('uc_payment_credit_gateway', 'test_gateway');
    variable_set('uc_credit_debug', 1);
    variable_set('fundraiser_development_mode', 1);
  }

  /**
   * Creates the stanard profile fields used by the fundraiser module.
   */
  function setupProfileFields() {
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('First Name', 'profile_first_name', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Last Name', 'profile_last_name', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Address', 'profile_address', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Address Line 2', 'profile_address_line_2', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('City', 'profile_city', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('State', 'profile_state', 'Personal Information', 'selection', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Zip', 'profile_zip', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Country', 'profile_country', 'Personal Information', 'textfield', 2)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Campaign Id', 'profile_cid', 'System', 'textfield', 4)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Market Source', 'profile_ms', 'System', 'textfield', 4)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Referrer', 'profile_referrer', 'System', 'textfield', 4)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Initial Referrer', 'profile_initial_referrer', 'System', 'textfield', 4)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Salesforce Account Id', 'profile_salesforce_account_id', 'System', 'textfield', 4)");
    db_query("INSERT INTO {profile_fields} (title, name, category, type, visibility) VALUES ('Salesforce Contact Id', 'profile_salesforce_contact_id', 'System', 'textfield', 4)");
  }

  function setProfileFields() {
    $personal = array();
    $personal['profile_first_name'] = 'John';
    $personal['profile_last_name'] = 'Doe';
    $personal['profile_address'] = '100 Elm Street';
    $personal['profile_address_line_2'] = '#5';
    $personal['profile_city'] = 'Washington';
    $personal['profile_state'] = 'DC';
    $personal['profile_zip'] = 20009;
    $personal['profile_country'] = 'US';
    _fundraiser_profile_save_profile($personal, $this->_user, 'Personal Information');
    $system = array();
    $system['profile_cid'] = '';
    $system['profile_ms'] = 'market-source';
    $system['profile_referrer'] = '';
  }

  /**
   *
   */
  function fundraiserReset() {
    $this->_fundraiser_node = NULL;
  }


  function getFundraiserForm($type = 'donation_form') {
    $args = func_get_args();
    if (isset($this->_fundraiser_node)) {
      return $this->_fundraiser_node;
    }

    $settings = array(
      'type' => $type,
      'language' => '',
      'uid' => '1',
      'status' => '1',
      'promote' => '1',
      'moderate' => '0',
      'sticky' => '0',
      'tnid' => '0',
      'translate' => '0',
      'title' => 'Test Donation Form',
      'body' => 'Donec placerat. Nullam nibh dolor, blandit sed, fermentum id, imperdiet sit amet, neque. Nam mollis ultrices justo. Sed tempor. Sed vitae tellus. Etiam sem arcu, eleifend sit amet, gravida eget, porta at, wisi. Nam non lacus vitae ipsum viverra pretium. Phasellus massa. Fusce magna sem, gravida in, feugiat ac, molestie eget, wisi. Fusce consectetuer luctus ipsum. Vestibulum nunc. Suspendisse dignissim adipiscing libero. Integer leo. Sed pharetra ligula a dui. Quisque ipsum nibh, ullamcorper eget, pulvinar sed, posuere vitae, nulla. Sed varius nibh ut lacus. Curabitur fringilla. Nunc est ipsum, pretium quis, dapibus sed, varius non, lectus. Proin a quam. Praesent lacinia, eros quis aliquam porttitor, urna lacus volutpat urna, ut fermentum neque mi egestas dolor.',
      'teaser' => 'Donec placerat. Nullam nibh dolor, blandit sed, fermentum id, imperdiet sit amet, neque. Nam mollis ultrices justo. Sed tempor. Sed vitae tellus. Etiam sem arcu, eleifend sit amet, gravida eget, porta at, wisi. Nam non lacus vitae ipsum viverra pretium. Phasellus massa. Fusce magna sem, gravida in, feugiat ac, molestie eget, wisi. Fusce consectetuer luctus ipsum. Vestibulum nunc. Suspendisse dignissim adipiscing libero. Integer leo. Sed pharetra ligula a dui. Quisque ipsum nibh, ullamcorper eget, pulvinar sed, posuere vitae, nulla. Sed varius nibh ut lacus. Curabitur fringilla.',
      'log' => '',
      'format' => '1',
      'is_donation_form' => '1',
      'donation_amounts' => '10,20,30', //array(10,20,30), // legacy code that can be removed from fundraiser.module
      'gateway' => 'test_gateway',
      'receipt_email_from' => 'Test',
      'receipt_email_address' => 'test@jacksonriver.com',
      'receipt_email_subject' => 'Thanks',
      'receipt_email_message' => 'Thanks',
      'amount_delta' => 4,
      'amount_0' => 10,
      'label_0' => '$10',
      'amount_1' => 20,
      'label_1' => '$20',
      'amount_2' => 50,
      'label_2' => '$50',
      'amount_3' => 100,
      'label_3' => '$100',
      'show_other_amount' => '1',
      'minimum_donation_amount' => '10',
      'internal_name' => 'Test Donation Form',
      'redirect_url' => '<confirmation>',
      'is_being_cloned' => '0',
      // expected by market source?
      'clone_from_original_nid' => '0',
      'confirmation_page_title' => 'Thanks!',
      'confirmation_page_body' => 'Thanks thanks.',
      'confirmation_page_format' => FILTER_FORMAT_DEFAULT,
      'webform' => array(
        'confirmation' => 'Thanks!',
        'confirmation_format' => FILTER_FORMAT_DEFAULT,
        'teaser' => '0',
        'allow_draft' => '0',
        'status' => '1',
        'record_exists' => '1',
        'submit_text' => '',
        'submit_limit' => '-1',
        'submit_interval' => '-1',
        'submit_notice' => '1',
        'roles' => array('1', '2'),
        'components' => array(),
        'emails' => array(),
      ),
    );

    if (isset($args[0]['settings'])) {
      $settings = array_merge($settings, $args[0]['settings']);
    }

    $node = $this->drupalCreateNode($settings);
    $node = node_load($node->nid, TRUE);

    $this->_fundraiser_node = $node;

    return $this->_fundraiser_node;
  }

  /**
   * Returns some test data.
   *
   * @return array
   */
  function createTestData() {
    $tomorrow = strtotime("+1 day");
    $first_name = $this->randomName(7);
    $last_name = $this->randomName(5);
    return array(
      'amount' => 10,
      'first_name' => $first_name,
      'last_name' => $last_name,
      'mail' => $first_name . '.' . $last_name . '@example.com',
      'address' => $this->randomName(25),
      'city' => $this->randomName(15),
      'state' => 44,
      'country' => 840,
      'zipcode' => '55555',
      'cc_number' => '4111111111111111',
      'cc_cvv' => '111',
      'cc_exp_month' => date('n', $tomorrow),
      'cc_exp_year' => date('Y', $tomorrow),
    );
  }

  /**
   * Sets up the form API array with the given data.
   *
   * @param array $data
   *
   * @return array
   */
  function createSubmission($data) {
    $edit1["submitted[donation][amount]"] = $data['amount'];
    $edit1["submitted[donor_information][first_name]"] = $data['first_name'];
    $edit1["submitted[donor_information][last_name]"] = $data['last_name'];
    $edit1["submitted[donor_information][email]"] = $data['mail'];
    $edit1["submitted[billing_information][address]"] = $data['address'];
    $edit1["submitted[billing_information][city]"] = $data['city'];
    $edit1["submitted[billing_information][country]"] = $data['country'];
    // state can't get filled out because it's dynamically populated based on country
//    $edit1["submitted[billing_information][state]"] = $data['state'];
    $edit1["submitted[billing_information][zip]"] = $data['zipcode'];
    $edit1["submitted[credit_card_information][card_number]"] = $data['cc_number'];
    $edit1["submitted[credit_card_information][expiration_date][card_expiration_month]"] = $data['cc_exp_month'];
    $edit1["submitted[credit_card_information][expiration_date][card_expiration_year]"] = $data['cc_exp_year'];
    $edit1["submitted[credit_card_information][card_cvv]"] = $data['cc_cvv'];
    return $edit1;
  }

}
