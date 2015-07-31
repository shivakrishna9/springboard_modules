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
    'sandbox' => 'https://uat.paywithmybank.com/api/v1',
    'production' => 'https://paywithmybank.com/api/v1',
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

  /**
   * @var array $lastErrors
   *   The last error objects received from the API.
   * @see https://paywithmybank.com/docs/api-ref.html#errors
   */
  private $lastErrors = NULL;

  public function __construct($accessId, $accessKey, $sandbox = FALSE) {
    $this->accessId = $accessId;
    $this->accessKey = $accessKey;
    $this->endpoint = ($sandbox) ? $this->endpoints['sandbox'] : $this->endpoints['production'];
  }

  /**
   * Gets the API endpoint for a given environment.
   */
  public function getEndpoint() {
    return $this->endpoint;
  }

  /**
   * Gets the last error objects received from the API.
   */
  public function getLastErrors() {
    return $this->lastErrors;
  }

  /**
   * Gets the details of an existing transaction.
   * 
   * @param string $transactionId
   *   The ID of the transaction.
   * 
   * @return object
   *   The transaction object.
   * @see https://paywithmybank.com/docs/api-ref.html#transactions
   */
  public function getTransaction($transactionId) {
    $data = $this->call('transactions/{transactionId}', array('transactionId' => $transactionId));
    return (isset($data->transaction)) ? $data->transaction : NULL;
  }

  /**
   * Gets a list of existing transactions.
   * 
   * @return array
   *   Array of transaction objects.
   * @see https://paywithmybank.com/docs/api-ref.html#transactions
   */
  public function listTransactions() {
    return $this->call('transactions');
  }

  /**
   * Collects money from the bank account of a previously authorized deferred
   * or recurring payment.
   *
   * @param string $transactionId
   *   The ID of the authorizing transaction.
   * @param string $merchantReference
   *   The initial merchant reference value sent to establish the payment.
   * @param string $amount
   *   The amount of money to collect.
   *
   * @return object
   *   The transaction object.
   * @see https://paywithmybank.com/docs/api-ref.html#transactions
   */
  public function captureTransaction($transactionId, $merchantReference, $amount) {
    $tokens = array('transactionId' => $transactionId);
    $parameters = array(
      'merchantReference' => $merchantReference,
      'amount' => $amount,
    );

    return $this->call('transactions/{transactionId}/capture', $tokens, $parameters);
  }

  private function call($path, $tokens, $parameters = NULL) {
    // Reset error messages.
    $this->lastErrors = NULL;

    // Add tokens to the API call path.
    if (!empty($tokens)) {
      foreach ($tokens as $key => $value) {
        $path = str_replace('{' . $key . '}', $value, $path);
      }
    }

    $url = $this->endpoint . '/' . $path;

    // Calculate basic auth string from the API credentials.
    $auth_string = base64_encode($this->accessId . ':' . $this->accessKey);

    $headers = array(
      'Authorization: Basic ' . $auth_string,
    );

    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);

    // If sending parameters, add them here and set request type to POST.
    if (!empty($parameters)) {
      $post_fields = '';
      foreach ($parameters as $key => $value) {
        $post_fields .= $key . '=' . $value . '&';
      }
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, $post_fields);
    }

    $response = curl_exec($ch);
    curl_close($ch);

    $data = json_decode($response);

    if (isset($data->errors)) {
      $this->lastErrors = $data->errors;
    }

    return $data;
  }

}
