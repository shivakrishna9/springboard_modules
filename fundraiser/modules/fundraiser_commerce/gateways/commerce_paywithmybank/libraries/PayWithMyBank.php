<?php

/**
 * Library for the PayWithMyBank REST API.
 */
class PayWithMyBank {

  /**
   * @var array $endpoints
   *   API endpoints indexed by environment type.
   */
  private $endpoints = array(
    'sandbox' => 'https://uat.paywithmybank.com',
    'production' => 'https://paywithmybank.com',
  );

  /**
   * @var array $endpoints
   *   API endpoints indexed by environment type.
   */
  private $endpoint = '';

  public function __construct($sandbox = FALSE) {
    $this->endpoint = ($sandbox) ? $this->endpoints['sandbox'] : $this->endpoints['production'];
  }

  /**
   * Gets the details of an existing transaction.
   * 
   * @param string $transactionId
   *   The ID of the transaction.
   * 
   * @return object
   *   The transaction object.
   * @see https://www.paywithmybank.com/beta/docs/api-ref.html#transactions
   */
  public function getTransaction($transactionId) {
    return $this->call('transactions', array($transactionId));
  }

  /**
   * Gets a list of existing transactions.
   * 
   * @return array
   *   Array of transaction objects.
   * @see https://www.paywithmybank.com/beta/docs/api-ref.html#transactions
   */
  public function listTransactions() {
    return $this->call('transactions');
  }

  private function call($method, $arguments, $parameters = NULL) {
    $url = $this->endpoint . '/' . $method;

    if (!empty($arguments)) {
      foreach ($arguments as $argument) {
        $url .= '/' . $argument;
      }
    }
  }

}
