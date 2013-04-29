<?php

/**
 * @file
 * Defines the class for connecting to the PSI payment service
 */

class PsiClient {
  /**
   * Constructor.
   */
  public function __construct($account) {
    $this->authenticated = FALSE;

    // Determine the correct WSDL URL
    if ($account['txn_mode'] == COMMERCE_PSI_MODE_LIVE) {
      $this->wsdl = 'https://client.paymentsolutionsinc.net/Services/Payment/PsiWcfService.svc?wsdl';
    }
    else {
      $this->wsdl = 'http://68.163.70.218/Services/Payment/PsiWcfService.svc?wsdl';
    }

    // Create a new soapclient
    try {
      $this->soapclient = new SoapClient($this->wsdl);
    }
    catch (Exception $ex) {
      watchdog('psi_commerce', 'Payment Services wsdl is not available.',
        array(),
        WATCHDOG_ERROR);
      return;
    }

    $this->account = $account;
    $this->authenticate();
  }

  /**
   * Verify authenticated
   */
  public function isAuthenticated() {
    return $this->authenticated;
  }

  /**
   * Return the authcode for this connection
   */
  public function getAuthcode() {
    return $this->authcode;
  }

  /**
   * Return a setting for the account
   */
  public function getAccountSetting($name) {
    return !empty($this->account[$name]) ? $this->account[$name] : NULL;
  }

  /**
   * Returns the authentication array for the current connection
   *
   * This must be included with every request
   */
  public function currentAuthentication() {
    return array(
      'authentication' => array(
        'AuthenticationCode' => $this->getAuthcode(),
        'SourceIpAddress' => $this->getAccountSetting('account_ip'),
      )
    );
  }

  /**
   * Authenticate with the API. Called from the constructor
   */
  public function authenticate() {
    // If this function gets called again after connecting, don't do anything
    if (!empty($this->authenticated)) {
      return $this->authenticated;
    }

    $credentials = array();
    $credentials['UserName'] = $this->getAccountSetting('account_username');
    $credentials['Password'] = $this->getAccountSetting('account_password');
    $credentials['SourceIpAddress'] = $this->getAccountSetting('account_ip');

    try {
      $response = $this->soapclient->Authenticate(array('credentials' => $credentials));
    }
    catch (Exception $ex) {
      watchdog('psi_commerce', 'The PSI Commerce account @title did not authenticate: @error',
        array(
          '@title' => $this->getAccountSetting('title'),
          '@error' => $ex->getMessage(),
        ),
        WATCHDOG_ERROR);
      return;
    }

    $this->authcode = $response->AuthenticateResult;
    $this->authenticated = TRUE;
  }

  /**
   * Send a credit card request
   *
   * @param $payment_details Array
   *   An array of values to pass to the PSI API
   *
   * @return Object
   *   The resulting data from the request
   */
  public function creditRequest($payment_details) {
    if (!$this->isAuthenticated()) {
      return;
    }

    $params = $this->currentAuthentication() + array('paymentInfo' => $payment_details);
    try {
      $response = $this->soapclient->SendCreditCardPayment($params);
    }
    catch (Exception $ex) {
      watchdog('psi_commerce', 'The PSI Commerce account @title failed to process an order: @error',
        array(
          '@title' => $this->getAccountSetting('title'),
          '@error' => $ex->getMessage(),
        ),
        WATCHDOG_ERROR);
    }

    if (!empty($response)) {
      return $response->SendCreditCardPaymentResult;
    }
  }

  /**
   * Send a credit card request
   *
   * @param $payment_details Array
   *   An array of values to pass to the PSI API
   *
   * @return Object
   *   The resulting data from the request
   */
  public function checkRequest($payment_details) {
    if (!$this->isAuthenticated()) {
      return;
    }

    $params = $this->currentAuthentication() + array('paymentInfo' => $payment_details);
    try {
      $response = $this->soapclient->SendEftPayment($params);
    }
    catch (Exception $ex) {
      watchdog('psi_commerce', 'The PSI Commerce account @title failed to process an order: @error',
        array(
          '@title' => $this->getAccountSetting('title'),
          '@error' => $ex->getMessage(),
        ),
        WATCHDOG_ERROR);
    }

    if (!empty($response)) {
      return $response->SendEftPaymentResult;
    }
  }
}
