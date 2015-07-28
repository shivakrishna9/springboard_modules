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
   * @var string $endpoint
   *   The API endpoint to call.
   */
  private $endpoint = '';

  /**
   * @var string $accessId
   *   The API access ID to authenticate with.
   */
  private $accessId = '';

  /**
   * @var string $accessKey
   *   The API access key to authenticate with.
   */
  private $accessKey = '';

  public function __construct($accessId, $accessKey, $sandbox = FALSE) {
    $this->accessId = $accessId;
    $this->accessKey = $accessKey;
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
    return $this->call('transactions/{transactionId}', array('transactionId' => $transactionId));
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

  private function call($path, $tokens, $parameters = NULL) {
    if (!empty($tokens)) {
      foreach ($tokens as $key => $value) {
        $path = str_replace('{' . $key . '}', $value, $url);
      }
    }

    $url = $this->endpoint . '/' . $path;
  }

}
