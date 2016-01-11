<?php

/**
 * Library for the PayWithMyBank REST API.
 */
class PayWithMyBank {

  // Transaction type constants.
  const TRANSACTION_TYPE_EXTERNAL = 0;
  const TRANSACTION_TYPE_AUTHORIZE = 1;
  const TRANSACTION_TYPE_PAY = 2;
  const TRANSACTION_TYPE_CAPTURE = 3;
  const TRANSACTION_TYPE_REFUND = 4;
  const TRANSACTION_TYPE_REVERSE = 5;
  const TRANSACTION_TYPE_CHALLENGE = 6;
  const TRANSACTION_TYPE_DEPOSIT = 7;
  const TRANSACTION_TYPE_RECLAIM = 8;
  // Transaction status constants.
  const TRANSACTION_STATUS_NEW = 0;
  const TRANSACTION_STATUS_PENDING = 1;
  const TRANSACTION_STATUS_AUTHORIZED = 2;
  const TRANSACTION_STATUS_PROCESSED = 3;
  const TRANSACTION_STATUS_COMPLETED = 4;
  const TRANSACTION_STATUS_FAILED = 5;
  const TRANSACTION_STATUS_EXPIRED = 6;
  const TRANSACTION_STATUS_CANCELED = 7;
  const TRANSACTION_STATUS_DENIED = 8;
  const TRANSACTION_STATUS_REVERSED = 10; // No status 9.
  const TRANSACTION_STATUS_PARTIALLY_REFUNDED = 11;
  const TRANSACTION_STATUS_REFUNDED = 12;
  const TRANSACTION_STATUS_VOIDED = 13;
  const TRANSACTION_STATUS_ON_HOLD = 14;
  // Payment type constants.
  const PAYMENT_TYPE_INSTANT = 1;
  const PAYMENT_TYPE_DEFERRED = 2;
  const PAYMENT_TYPE_RECURRING = 3;
  const PAYMENT_TYPE_DISBURSEMENT = 4;
  const PAYMENT_TYPE_VERIFICATION = 5;
  const PAYMENT_TYPE_RETRIEVAL = 6;
  // Authorization status constants.
  const AUTHORIZATION_STATUS_PENDING = 1;
  const AUTHORIZATION_STATUS_AUTHORIZED = 2;
  const AUTHORIZATION_STATUS_FINISHED = 3;
  const AUTHORIZATION_STATUS_EXPIRED = 4;
  const AUTHORIZATION_STATUS_FAILED = 5;
  const AUTHORIZATION_STATUS_CANCELED = 6;
  const AUTHORIZATION_STATUS_VOIDED = 7;
  // Event type constants.
  const EVENT_TYPE_UPDATE = 'Update';
  const EVENT_TYPE_ESTABLISH = 'Establish';
  const EVENT_TYPE_AUTHORIZE = 'Authorize';
  const EVENT_TYPE_PROCESS = 'Process';
  const EVENT_TYPE_COMPLETE = 'Complete';
  const EVENT_TYPE_CANCEL = 'Cancel';
  const EVENT_TYPE_FAIL = 'Fail';
  const EVENT_TYPE_EXPIRE = 'Expire';
  const EVENT_TYPE_DENY = 'Deny';
  const EVENT_TYPE_REFUND = 'Refund';
  const EVENT_TYPE_DISPUTE = 'Dispute';
  const EVENT_TYPE_RECONCILE = 'Reconcile';
  const EVENT_TYPE_REVERSE = 'Reverse';
  const EVENT_TYPE_HOLD = 'Hold';
  const EVENT_TYPE_FEEDBACK = 'Feedback';
  // Recurrence frequency unit types.
  const NULL_RECURRENCE_FREQUENCY_UNIT_TYPE = '_NULL_RECURRENCE_FREQUENCYUNITTYPE';

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
    $data = $this->call('/api/v1/transactions/{transactionId}', array('transactionId' => $transactionId));
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
   * Gets an array of event objects associated with a transaction.
   * 
   * @param string $transactionId
   *   The ID of the transaction.
   * 
   * @return array
   *   Array of event objects.
   * @see https://paywithmybank.com/docs/api-ref.html#events
   */
  public function getTransactionEvents($transactionId) {
    $data = $this->call('/api/v1/transactions/{transactionId}/events', array('transactionId' => $transactionId));
    return (isset($data->events)) ? $data->events : NULL;
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

    $data = $this->call('/api/v1/transactions/{transactionId}/capture', $tokens, $parameters);
    return (isset($data->transaction)) ? $data->transaction : NULL;
  }

  /**
   * Refunds a given amount of a processed transaction.
   *
   * @param type $transactionId
   *   The ID of the authorizing transaction.
   * @param type $merchantReference
   *   The initial merchant reference value sent to establish the payment.
   * @param type $amount
   *   The amount of money to refund.
   *
   * @return object
   *   The transaction object.
   * @see https://paywithmybank.com/docs/api-ref.html#transactions
   */
  public function refundTransaction($transactionId, $merchantReference, $amount) {
    $tokens = array('transactionId' => $transactionId);
    $parameters = array(
      'merchantReference' => $merchantReference,
      'amount' => $amount,
    );

    $data = $this->call('/api/v1/transactions/{transactionId}/refund', $tokens, $parameters);
    return (isset($data->transaction)) ? $data->transaction : NULL;
  }

  /**
   * Updates a recurring payment schedule.
   *
   * @param type $transactionId
   *   The ID of the authorizing transaction.
   * @param type $merchantReference
   *   The initial merchant reference value sent to establish the payment.
   * @param type $recurrence
   *   The Recurrence object.
   *   @see https://www.paywithmybank.com/beta/docs/api-ref.html#recurrence
   *
   * @return object
   *   The transaction object.
   * @see https://paywithmybank.com/docs/api-ref.html#transactions
   */
  public function updateRecurrence($transactionId, $merchantReference, $recurrence) {
    $tokens = array('transactionId' => $transactionId);
    $parameters = array(
      'merchantReference' => $merchantReference,
      'recurrence' => $recurrence,
    );

    $data = $this->call('/api/v1/transactions/{transactionId}/updateRecurrence', $tokens, $parameters);
    return (isset($data->transaction)) ? $data->transaction : NULL;
  }

  /**
   * Sets the verification status of a transaction.
   *
   * @param string $transactionId
   *   The ID of the authorizing transaction.
   * @param string $merchantReference
   *   The initial merchant reference value sent to establish the payment.
   * @param bool $approve
   *   The transaction approval status.
   *
   * @return object
   *   The transaction object.
   * @see https://paywithmybank.com/docs/api-ref.html#transactions
   */
  public function setVerificationStatus($transactionId, $merchantReference, $approve) {
    $tokens = array('transactionId' => $transactionId);
    $parameters = array(
      'merchantReference' => $merchantReference,
      'approve' => $approve,
    );

    $data = $this->call('/api/v1/transactions/{transactionId}/setVerificationStatus', $tokens, $parameters);
    return (isset($data->transaction)) ? $data->transaction : NULL;
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
