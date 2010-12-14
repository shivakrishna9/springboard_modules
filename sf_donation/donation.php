<?php

/**
 * @file donation.php
 * Classes to represent a Springboard donation
 *
 * @author Phillip Cave <phillip.cave@jacksonriver.com>
 */

class DonationFactory 
{
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
class DonationMapper
{
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
class NPSPDonation extends Donation
{
  public function _get_stages() {
    return array(
      'payment_received' => 'Posted',
      'payment_withdrawn' => 'Withdrawn',
      'payment_pending' => 'Pledged',
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
    );
  }
}

/**
 * Represents a Common Ground donation
 */
class CommonGroundDonation extends Donation
{
  public function _get_stages() {
    return array(
      'payment_received' => 'Received',
      'payment_withdrawn' => 'Withdrawn',
      'payment_pending' => 'Not Received',
    );
  }
  
  static function default_map() {
    return array();
  }
}

/**
 * Generic donation class
 */
class Donation
{
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
  public $transaction_date = null;
  public $close_date = null;
  public $transaction_date_gm = null;
  public $probability = 50.00;
  public $stage;
  
  function __construct($order_id, $sid) {
  
    global $base_url;
    
    // set the order id
    $this->order_id = $order_id;
    
    $stages = $this->_get_stages();
    
    // load items required to populate the object
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
    $this->donation_form_url = $base_url . '/node/' . $order->products[0]->nid;
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
    
    // check for payments
    if ($payments) {
  		// deal with sf date handling
  		$this->transaction_date = strtotime(date('H:i:s d-M-Y T', $payments[0]->received));
      $this->close_date = date('Y-m-d', $this->transaction_date);
      $this->transaction_date_gm = gmdate('c', $this->transaction_date);
      $this->probability = 100.00;
      $this->stage = $stages['payment_received'];
  	}
    
    uc_credit_cache('clear');
    
    // if this is a recurring donation, make sure we get the right close date  	
  	$close_date = db_result(db_query("SELECT next_charge FROM {fundraiser_recurring} WHERE order_id = %d", $this->order_id));
  	if ($close_date) {
  		$this->close_date = date('Y-m-d', $close_date);
  	}
  	
  	$this->_load_webform_values($this->donation_form_nid, $sid);

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
    
    foreach($map['fields'] as $salesforce => $drupal) {
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
   * @return    null
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
      'payment_withdrawn' => 'Withdrawn',
      'payment_pending' => 'Pledged',
    );
  }
  
}