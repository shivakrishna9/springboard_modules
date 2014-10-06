<?php
/**
 * @file
 * Class for the Account Updater functionality of Litle.
 */

class CommerceLitleAccountUpdater {

  protected $methodInstance;

  /**
   * Save the method instance for later.
   *
   * @param array $method_instance
   *   The payment gateway's method instance data.
   *   Sometimes also called gateway_details.
   *   Needs to contain at least the settings for the payment method.
   */
  public function __construct(array $method_instance) {
    $this->methodInstance = $method_instance;
  }

  /**
   * Checks the payment method instance settings to see if we should extend it.
   *
   * @return bool
   *   TRUE if the sustainer series should be extended by one.
   */
  public function shouldExtendSustainers() {
    return
      isset($this->methodInstance['base'])
      && $this->methodInstance['base'] == 'commerce_litle_cc'
      && isset($this->methodInstance['settings']['accountupdater_extend_sustainers'])
      && $this->methodInstance['settings']['accountupdater_extend_sustainers'];
  }

  /**
   * Add a month to the given end date.
   *
   * @param int $year
   *  The year.
   * @param int $month
   *  The month.
   *
   * @return array
   *   An array with year and month keys.
   */
  public function getNewEndDate($year, $month) {
    $exp_datetime = new DateTime($year . '-' . $month);
    $exp_datetime->add(new DateInterval('P1M'));

    return array(
      'month' => $exp_datetime->format('n'),
      'year' => $exp_datetime->format('Y'),
    );
  }

  /**
   * Take the response XML from Litle and does the account updater stuff.
   *
   * @param DOMDocument $response
   *   The XML response document.
   *
   * @param object $order
   *   The order object that has just been charged.
   *   This will be used as the template to create additional donations
   *   if needed.
   */
  public function processResponseXML(DOMDocument $response, $order) {
    $list = $response->getElementsByTagName('accountUpdater');

    if ($list->length == 0) {
      return;
    }

    $data = $this->getDataFromXML($list->item(0));

    // Do some error checking.
    if (!isset($data['originalCardTokenInfo']) || !isset($data['newCardTokenInfo'])) {
      return;
    }

    $old_exp_date = $this->convertExpDateString($data['originalCardTokenInfo']['expDate']);

    $card = $this->findMatchingCardonfile($data['originalCardTokenInfo']['litleToken'], $old_exp_date);

    if (is_null($card)) {
      // No card with a matching remote id or exp date.
      // Bail for now.
      // @todo Do something? Create a new cardonfile?
      return;
    }

    $changed = FALSE;

    // If the Litle Account Updater returns a new token,
    // we save that new token with the series/scheduled payments.
    // A new token implies a new last 4 digits of the card number.
    if ($data['originalCardTokenInfo']['litleToken'] != $data['newCardTokenInfo']['litleToken']) {

      $new_token = $data['newCardTokenInfo']['litleToken'];
      $new_number = substr($new_token, -4);
      $new_type = _commerce_litle_litle_card_type_to_cc($data['newCardTokenInfo']['type']);

      $card->remote_id = $new_token;
      // The long form of the card_type.
      $card->type = $new_type;
      // The card_number, which is the last 4 of the token for credit cards.
      $card->card_number = $new_number;

      $changed = TRUE;
    }

    // If the Litle Account Updater returns a new expiration date,
    // we create recurring donations through that new expiration date (+1 month,
    // see #703 for reference).
    if ($data['originalCardTokenInfo']['expDate'] != $data['newCardTokenInfo']['expData']) {
      // Call create future orders with new exp date.
      // Edit card_exp_month and card_exp_year.
      $new_exp_date = $this->convertExpDateString($data['newCardTokenInfo']['expDate']);
      $card->card_exp_month = $new_exp_date['month'];
      $card->card_exp_year = $new_exp_date['year'];

      $donation = fundraiser_donation_get_donation($order->order_id);
      if (isset($donation->recurring['master_did'])) {
        $master_donation = fundraiser_donation_get_donation($order->recurring['master_did']);
        $this->createFutureOrders($master_donation, $new_exp_date['month'], $new_exp_date['year'], $donation);
      }

      $changed = TRUE;
    }

    if ($changed) {
      $card->changed = REQUEST_TIME;
      commerce_cardonfile_save($card);
    }

  }

  /**
   * Converts the date format from Litle to an array that Springboard likes.
   *
   * @param string $date_string
   *   The expiration date string from Litle.
   *   For example, 1114 for November 2014.
   *
   * @return array
   *   An array with month and year keys.
   */
  public function convertExpDateString($date_string) {
    $two_digit_year = (int) substr($date_string, -2);
    // Split the expDate into month and week.
    $exp_data = array(
      'month' => substr($date_string, 0, 2),
      'year' => 2000 + $two_digit_year,
    );

    return $exp_data;
  }

  /**
   * Extracts the important information from the accountUpdater XML.
   *
   * @param DOMNode $updater_node
   *   The DOM node for accountUpdater
   *
   * @return array
   *   The original and new card token info.
   */
  public function getDataFromXML(DOMNode $updater_node) {
    $card_info_keys = array('originalCardTokenInfo', 'newCardTokenInfo');
    $card_keys = array('litleToken', 'expDate', 'type');
    $data = array();
    foreach ($updater_node->childNodes as $node) {
      if (in_array($node->nodeName, $card_info_keys)) {
        foreach ($node->childNodes as $thing) {
          if (in_array($thing->nodeName, $card_keys)) {
            $data[$node->nodeName][$thing->nodeName] = $thing->nodeValue;
          }
        }
      }
    }

    return $data;
  }


  /**
   * Get a matching cardonfile for the given token and expiration date.
   *
   * @param string $remote_id
   *   The Litle token, usually from litleToken.
   * @param array $exp_date
   *   An array with month and year keys.
   *
   * @return null|object
   *   The card entity, or NULL if nothing's found.
   */
  public function findMatchingCardonfile($remote_id, array $exp_date) {
    $card = NULL;
    $cards = commerce_cardonfile_load_multiple_by_remote_id($remote_id);
    // Could be an array or FALSE.
    if (!$cards || empty($cards)) {
      return $card;
    }

    foreach ($cards as $potential_card) {
      // Check the card on file agains the old expiration date.
      if ($potential_card->card_exp_month == $exp_date['month'] && $potential_card->card_exp_year == $exp_date['year']) {
        $card = $potential_card;
        break;
      }
    }

    return $card;
  }

  protected function createFutureOrders() {
    _fundraiser_sustainers_create_future_orders($donation, $month, $year, $source_donation = NULL, $start = NULL);
  }
}
