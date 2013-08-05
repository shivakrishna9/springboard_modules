<?php

/**
 * @file
 * Classes to represent a Springboard donation
 *
 * @author Phillip Cave <phillip.cave@jacksonriver.com>
 */

class DonationFactory {
  public static function Create($id, $order_id, $sid) {
    switch ($id) {
      case 'npsp':
        return new NPSPDonation($order_id, $sid);

      case 'common_ground':
        return new CommonGroundDonation($order_id, $sid);
    }
  }
}

/**
 * Class to define maps for the different donation types
 */
class DonationMapper {
  public static function GetDefaultMap($id) {
    switch ($id) {
      case 'npsp':
        return NPSPDonation::default_map();

      case 'common_ground':
        return CommonGroundDonation::default_map();
    }
  }
}

/**
 *  Represents a Non Profit Starter Pack specfic donation
 */
class NPSPDonation extends Donation {
  public function _get_stages() {
    return array(
      'payment_received' => 'Posted',
      'canceled' => 'Withdrawn',
      'pending_future_payment' => 'Pledged',
      'partially_refunded' => 'Partially Refunded',
      'refunded' => 'Refunded',
      'failed' => 'Declined',
    );
  }

  static function default_map() {
    return array(
      'donor_salesforce_account_id' => 'AccountId',
      'name' => 'Name',
      'amount' => 'Amount',
      'cc_last_4' => 'CC_Last_4__c',
      'cc_expiration_month' => 'CC_Exp_Month__c',
      'cc_expiration_year' => 'CC_Exp_Year__c',
      'cc_type' => 'CC_Type__c',
      'donation_form_name' => 'Donation_Form_Name__c',
      'donation_form_url' => 'Donation_Form_URL__c',
      'order_id' => 'Order_ID__c',
      'close_date' => 'CloseDate',
      'transaction_date_gm' => 'Transaction_Date_Time__c',
      'probability' => 'Probability',
      'stage' => 'StageName',
      'billing_street1' => 'Billing_Street__c',
      'billing_street2' => 'Billing_Street_Line_2__c',
      'billing_city' => 'Billing_City__c',
      'billing_zone' => 'Billing_State__c',
      'billing_postal_code' => 'Billing_Zip__c',
      'billing_country' => 'Billing_Country__c',
      'cid' => 'CampaignId',
      'referrer' => 'Referrer__c',
      'initial_referrer' => 'Initial_Referrer__c',
      'ms' => 'Market_Source__c',
      'payment_gateway' => 'Payment_Gateway__c',
      'payment_transaction_id' => 'Gateway_Reference__c',
      'payment_authorization_code' => 'Payment_Gateway_Auth_Code__c',
      'created_by' => 'Donation_Created_By__c',
      'offline' => 'Offline_Donation__c',
    );
  }
}

/**
 * Represents a Common Ground donation
 */
class CommonGroundDonation extends Donation {
  public function _get_stages() {
    return array(
      'payment_received' => 'Received',
      'payment_withdrawn' => 'Withdrawn',
      'pending_future_payment' => 'Not Received',
      'failed' => 'Declined',
      'canceled' => 'Withdrawn',
    );
  }

  static function default_map() {
    return array(
      'donor_salesforce_account_id' => 'AccountId',
      'name' => 'Name',
      'amount' => 'Amount',
      'cc_last_4' => 'CC_Last_4__c',
      'cc_expiration_month' => 'CC_Exp_Month__c',
      'cc_expiration_year' => 'CC_Exp_Year__c',
      'cc_type' => 'CC_Type__c',
      'donor_salesforce_contact_id' => 'cv__Contact__c',
      'order_id' => 'cv__ExternalId__c',
      'payment_type' => 'cv__Payment_Type__c',
      'donation_form_name' => 'Donation_Form_Name__c',
      'donation_form_url' => 'Donation_Form_URL__c',
      'close_date' => 'CloseDate',
      'transaction_date_gm' => 'Transaction_Date_Time__c',
      'probability' => 'Probability',
      'stage' => 'StageName',
      'billing_first_name' => 'Billing_First_Name__c',
      'billing_last_name' => 'Billing_Last_Name__c',
      'billing_street1' => 'Billing_Street__c',
      'billing_street2' => 'Billing_Street_Line_2__c',
      'billing_city' => 'Billing_City__c',
      'billing_zone' => 'Billing_State__c',
      'billing_postal_code' => 'Billing_Zip__c',
      'billing_country' => 'Billing_Country__c',
      'cid' => 'CampaignId',
      'referrer' => 'Referrer__c',
      'initial_referrer' => 'Initial_Referrer__c',
      'ms' => 'Market_Source__c',
      'payment_gateway' => 'Payment_Gateway__c',
      'payment_transaction_id' => 'Gateway_Reference__c',
    );
  }
}

/**
 * Generic donation class
 */
class Donation {
  private $_stage_posted = 'Posted';
  private $_stage_pledged = 'Pledged';

  public $donor_uid;
  public $donor_email;
  public $donor_name;
  public $donor_salesforce_account_id;
  public $donor_salesforce_contact_id;
  public $name;
  public $amount;
  public $cc_last_4;
  public $cc_expiration_month;
  public $cc_expiration_year;
  public $cc_type;
  public $donation_form_name;
  public $donation_form_nid;
  public $donation_form_url;
  public $order_id;
  public $order_status;
  public $billing_first_name;
  public $billing_last_name;
  public $billing_street1;
  public $billing_street2;
  public $billing_city;
  public $billing_zone;
  public $billing_postal_code;
  public $billing_country;
  public $transaction_date = NULL;
  public $close_date = NULL;
  public $transaction_date_gm = NULL;
  public $probability = 50.00;
  public $stage;
  public $payment_gateway;
  public $payment_transaction_id;
  public $created_by;
  public $offline;
  public $payment_authorization_code;

  function __construct($order_id, $sid) {

    global $base_url;

    // set the order id
    $this->order_id = $order_id;

    $stages = $this->_get_stages();

    // load items required to populate the object
    uc_credit_cache('clear');
    $order = uc_order_load($this->order_id);
    $payments = uc_payment_load_payments($this->order_id);
    $user = user_load($order->uid);

    // set properties
    $this->donor_uid = $order->uid;
    $this->donor_email = $user->mail;
    $this->donor_name = $user->name;
    $this->donor_salesforce_account_id = $user->salesforce_account_id;
    $this->donor_salesforce_contact_id = $user->salesforce_contact_id;
    $this->name = t('Donation - !first !last (!date)', array('!first' => $order->billing_first_name, '!last' => $order->billing_last_name, '!date' => gmdate('Y-m-d H:i:s\Z', $order->created)));
    $this->amount = $order->order_total;
    $this->cc_last_4 = !empty($order->payment_details['cc_number']) ? substr($order->payment_details['cc_number'], -4) : '';
    $this->cc_expiration_month = $order->payment_details['cc_exp_month'];
    $this->cc_expiration_year = $order->payment_details['cc_exp_year'];
    $this->cc_type = $order->payment_details['cc_type'];
    $this->donation_form_name = $order->products[0]->title;
    $this->donation_form_nid = $order->products[0]->nid;
    $this->order_status = $order->order_status;
    $this->billing_first_name = $order->billing_first_name;
    $this->billing_last_name = $order->billing_last_name;
    $this->billing_street1 = $order->billing_street1;
    $this->billing_street2 = $order->billing_street2;
    $this->billing_city = $order->billing_city;
    $this->billing_zone = uc_get_zone_code($order->billing_zone);
    $this->billing_country = $this->_convert_country($order->billing_country);
    $this->billing_postal_code = $order->billing_postal_code;
    $this->stage = $stages['payment_pending'];
    $this->created_by = $order->data['created_by'];
    $this->offline = $order->data['offline'];

    // Only set these fields if it's paid
    if ($order->order_status == 'payment_received') {
      // deal with sf date handling
      $this->transaction_date = strtotime(date('H:i:s d-M-Y T', $payments[0]->received));
      $this->close_date = date('Y-m-d', $this->transaction_date);
      $this->transaction_date_gm = gmdate('c', $this->transaction_date);
      $this->probability = 100.00;
    }

    // Set close date to the original date for refunds otherwise SF will default to current date
    if ($order->order_status == 'refunded' || $order->order_status == 'partially_refunded') {
      $this->transaction_date = strtotime(date('H:i:s d-M-Y T', $payments[0]->received));
      $this->close_date = date('Y-m-d', $this->transaction_date);
    }

    $this->stage = $stages[$order->order_status];

    // add gateway and transaction id
    $txn_details = $this->_load_transaction_details($this->order_id);
    $this->payment_gateway = $txn_details['gateway'];
    $this->payment_transaction_id = $txn_details['txn_id'];
    $this->payment_authorization_code = $txn_details['auth_code'];
    $this->donation_form_url = $txn_details['form_url'];

    // if this is a recurring donation, make sure we get the right close date
    $close_date = db_result(db_query("SELECT next_charge FROM {fundraiser_recurring} WHERE order_id = %d", $this->order_id));
    if ($close_date) {
      $this->close_date = date('Y-m-d', $close_date);
    }

    $this->_load_webform_values($this->donation_form_nid, $sid);

    // Allow modules to alter the object
    drupal_alter('sf_donation_object', $this);
  }

  /**
   *
   * Retrieves the fieldmap for a specific donation form.
   *
   */
  private function _get_donation_map($nid) {
    $sql = "SELECT f.single_recordtype_id, f.recurring_recordtype_id, f.fields, f.salesforce FROM {fundraiser_salesforce_map} f WHERE f.nid = %d";
    $result = db_query($sql, $nid);

    $data = db_fetch_object($result);
    return array(
      'single_recordtype_id' => $data->single_recordtype_id,
      'recurring_recordtype_id' => $data->recurring_recordtype_id,
      'fields' => unserialize($data->fields),
    );
  }

  /**
   *
   * Transforms a donation into a Salesforce object using it's assigned map
   *
   * @return    An array that represents a Salesforce opportunity
   */
  public function map($type = 'single') {
    $map = $this->_get_donation_map($this->donation_form_nid);
    $object = array();

    foreach ($map['fields'] as $salesforce => $drupal) {
      $object[$salesforce] = $this->{$drupal};
    }

    // add single recordtype id if available
    if ($type == 'single' && !empty($map['single_recordtype_id'])) {
      $object['RecordTypeId'] = $map['single_recordtype_id'];
    }

    // add recurring recordtype id if available
    if ($type == 'recurring' && !empty($map['recurring_recordtype_id'])) {
      $object['RecordTypeId'] = $map['recurring_recordtype_id'];
    }

    return $object;
  }

  /**
   *
   * Loads webform submitted data as properties of the object. This will only load unique properites that haven't
   * already been exposed by another object.
   *
   * @param     $nid The id of the donation form node
   * @param     $sid The id of the webform submission
   *
   * @return    NULL
   */
  private function _load_webform_values($nid, $sid) {
    $sql = "SELECT c.form_key, s.data FROM {webform_submitted_data} s INNER JOIN {webform_component} c on c.cid = s.cid and c.nid = s.nid WHERE s.sid = %d";
    //print "<pre>".print_r($map, TRUE)."</pre>";
    $results = db_query($sql, $sid);
    $exclude = Donation::get_fundraiser_fields();
    while ($data = db_fetch_object($results)) {
      // do not include fundraiser form items that have already been accounted for by the order
      if (!in_array($data->form_key, $exclude)) {
        $this->{$data->form_key} = $data->data;
      }
    }
  }

  /**
   * Gets the transaction details for a specific order.
   */
  function _load_transaction_details($order_id) {
    $details = array();
    $result = db_query(
      "
        SELECT order_id, gateway, txn_id, auth_code, form_url
        FROM {fundraiser_webform_order}
        WHERE order_id = %d
        UNION
        SELECT order_id, gateway, txn_id, auth_code, form_url
        FROM {fundraiser_recurring}
        WHERE order_id = %d
      ",
      $order_id, $order_id
    );

    while ($data = db_fetch_object($result)) {
      $details['gateway'] = $data->gateway;
      $details['txn_id'] = $data->txn_id;
      $details['auth_code'] = $data->auth_code;
      $details['form_url'] = $data->form_url;
    }

    // allow other modules to alter the details
    drupal_alter('donation_transaction_details', $details);
    return $details;
  }

  private function _convert_country($id) {
    return db_result(db_query("SELECT country_iso_code_2 FROM {uc_countries} WHERE country_id = %d", $id));
  }

  static function get_mappable_properties() {
    return array(
      'donor_uid' => 'Donor\'s User Id',
      'donor_email' => 'Donor\'s Email Address',
      'donor_name' => 'Donor\'s Drupal Username',
      'donor_salesforce_account_id' => 'Donor\'s Salesforce Account ID',
      'donor_salesforce_contact_id' => 'Donor\'s Salesforce Contact ID',
      'name' => 'Donation Name',
      'amount' => 'Donation Amount',
      'currency' => 'Currency',
      'cc_last_4' => 'Credit Card Last 4',
      'cc_expiration_month' => 'Credit Card Expiration Month',
      'cc_expiration_year' => 'Credit Card Expiration Year',
      'cc_type' => 'Credit Card Type',
      'donation_form_name' => 'Donation Form Name',
      'donation_form_nid' => 'Donation Form Node Id',
      'donation_form_url' => 'Donation Form Url',
      'order_id' => 'Order ID',
      'order_status' => 'Order Status',
      'billing_first_name' => 'Billing First Name',
      'billing_last_name' => 'Billing Last Name',
      'billing_street1' => 'Billing Street',
      'billing_street2' => 'Billing Street 2',
      'billing_city' => 'Billing City',
      'billing_zone' => 'Billing Zone',
      'billing_postal_code' => 'Billing Postal Code',
      'billing_country' => 'Billing Country',
      'close_date' => 'Close Date',
      'transaction_date_gm' => 'Transaction Date/Time',
      'probability' => 'Probability',
      'stage' => 'Stage',
      'payment_gateway' => 'Payment Gateway',
      'payment_transaction_id' => 'Payment Transaction ID',
      'created_by' => 'Donation Created By',
      'offline' => 'Offline Donation',
      'payment_authorization_code' => 'Payment Authorization Code',
    );
  }

  static function get_fundraiser_fields() {
    return array(
      'other_amount',
      'amount',
      'first_name',
      'last_name',
      'email',
      'billing_address',
      'billing_address_2',
      'billing_city',
      'billing_country',
      'billing_state',
      'billing_zipcode',
      'card_number',
      'card_expiration_date',
      'card_cvv',
      'recurs_monthly',
    );
  }

  public function _get_stages() {
    return array(
      'payment_received' => 'Posted',
      'canceled' => 'Withdrawn',
      'pending_future_payment' => 'Pledged',
      'partially_refunded' => 'Partially Refunded',
      'refunded' => 'Refunded',
    );
  }

}
