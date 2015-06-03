<?php

/**
 * @file
 * API calls used by springboard advocacy submodules.
 */

class AdvocacyApiCall {

  /**
   * Load the API client.
   */
  public function __construct() {
    $loader = new AdvocacyApiClientLoader();
    $this->client = $loader->getClient();
  }

  /**
   * Call a php-sdk API method.
   *
   * @param $method
   * @return mixed
   */
  public function invokeClientMethod($method) {

    $args = func_get_args();
    unset($args[0]);

    $response = call_user_func_array(array($this->client, $method), $args);

    if ($this->client->isDebugMode()) {
      $info = $this->client->getDebugInfo();
      $this->setDebug($info);
    }

    if (!isset($response->data)) {
      $this->buildMessage($response, $method);
    }

    return $response;
  }

  /**
   * Log a debug message to watchdog.
   *
   * @param array $info
   *    The API response in an array.
   */
  private function setDebug(array $info) {
    watchdog('advocacy', 'Advocacy API Call details: !message', array('!message' => '<pre>' . check_plain(print_r($info, TRUE)) . '</pre>'), WATCHDOG_DEBUG);
  }

  /**
   * Log a non-debug error to watchdog and display message.
   *
   * @param string $message
   *   An error message.
   */
  public function setMessage($message) {
    drupal_set_message($message, 'error');
    watchdog('advocacy', $message, array(), WATCHDOG_ERROR);
  }

  /**
   * Get subscription details from API server and stick 'em in variables table.
   *
   * @return array|bool|null
   *   An array of subscription data or FALSE.
   */
  public function getSubscription() {

    $subscription = variable_get('sba_subscription', array());

    if (empty($subscription)) {
      $response = $this->client->getSubscription();

      if ($this->client->isDebugMode()) {
        $info = $this->client->getDebugInfo();
        $this->setDebug($info);
      }

      if (isset($response->data)) {
        $subscription = (array) $response->data;
        variable_set('sba_subscription', array('subscription_type' => $response->data->subscription_type, 'states_custom' => isset($response->data->states_custom) ? $response->data->states_custom : NULL));
      }
      else {
        $this->buildMessage($response, 'getSubscription');
        return FALSE;
      }

    }
    return $subscription;
  }

  /**
   * Get a list of available committees, from static cache, DB cache or API.
   *
   * Called from the views committee field's autocomplete callback.
   *
   * @return array
   *    A committee list array,
   */
  public function getCommittees() {

    $committee_list = &drupal_static(__FUNCTION__);
    if (!isset($committee_list)) {
      if ($cache = cache_get('committee_list')) {
        $committee_list = $cache->data;
      }
      else {
        $response = $this->client->getCommitteeList();
        if ($this->client->isDebugMode()) {
          $info = $this->client->getDebugInfo();
          $this->setDebug($info);
        }
        if (isset($response->data)) {
          $committee_list = (array) $response->data;
          $expire = time() + 3600;
          cache_set('committee_list', $committee_list, 'cache', $expire);
        }
      }
    }
    return $committee_list;
  }

  /**
   * Get a list of districts for a state.
   *
   * Called from the view filter state dropdown's ajax callback.
   *
   * @param string $state
   *    A state abbreviation.
   *
   * @return array
   *    An array formatted for selector option use.
   */
  public function getDistricts($state) {
    $options = array('All' => '- Any -');
    $response = $this->client->getDistrictsByState($state);

    if ($this->client->isDebugMode()) {
      $info = $this->client->getDebugInfo();
      $this->setDebug($info);
    }

    if (isset($response->data)) {
      foreach ($response->data as $district) {
        $options[$district->DistrictDesc] = $district->DistrictDesc;
      }
    }
    else {
      $this->buildMessage($response, 'getDistrictsByState');
    }
    return $options;
  }

  private function buildMessage($response, $method) {
    if (user_access('administer springboard advocacy')) {
      if (isset($response->error)) {
        $error = isset($response->error->message) ? $response->error->message : $response->error;
        $message = t('An error occurred when attempting to call @method: @message:', array('@method' => $method, '@message' => $error));
      }
      else {
        $message = t('A response was not received from the server when attempting to call @method', array('@method' => $method));
      }
      $this->setMessage($message);
    }
  }
}
