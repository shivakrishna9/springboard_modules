<?php

class CapizConnect {
  public static $base_url = 'https://connect.capwiz.com/api/';

  private $connected = FALSE;
  private $account_details;
  private $last_error;
  
  private $client_id;
  private $session_id;
  
  /**
   * Create a single instance of CapizConnect.
   */
  public static function instance($account_details) {
    static $instances = array();
    
    if ($account_details->id > 0) {
      if (!isset($instances[$account_details->id])) {
        $instances[$account_details->id] = new CapizConnect($account_details);
      }
      return $instances[$account_details->id];
    }
    return null;
  }
  
  
  /**
   * Protect constructor.
   */
  protected function __construct($account_details) {
    $this->account_details = $account_details;
    $this->authenticate();
  }
  
  public function isConnected() {
    return $this->connected;
  }
  
  
  /**
   * Get the text of the last error, so that it can be shown to the user
   */
  public function getLastError() {
    return $this->last_error;
  }
  
  
  /**
   * Authenticate with the API. Called from the constructor
   */
  public function authenticate() {
    $this->connected = FALSE;
    
    if ($this->account_details->user_name && $this->account_details->user_password) {
      $query_params = array(
        'loginName' => $this->account_details->user_name,
        'password' => $this->account_details->user_password,
      );
      $url = url(self::$base_url . 'auth/v1/authenticate', array('query' => $query_params));
      
      $auth_response = drupal_http_request($url);
      
      // If the authentication was successfull
      if ($auth_response->code == 200) {
        $response_data = json_decode($auth_response->data);
        if ($response_data) {
          $this->client_id = $response_data->application->clientId;
          $this->session_id = $response_data->id;
          $this->connected = TRUE;
          $this->last_error = t('Connected successfully');
        }
        else {
          watchdog('capwiz_connect', 'There was an error authenticating with Capwiz Connect for account %account_name. Response: %error.', array('%account_name' => $this->account_details->display_name, '%error' => print_r($auth_response->data, TRUE)), WATCHDOG_ERROR);
        }
      }
      // Response code was NOT 200. Report the error
      else {
        switch ($auth_response->code) {
          case 400:
            $this->last_error = 'Required arguments missing: loginName or password';
            break;
          case 401:
            $this->last_error = 'Invalid login credentials';
            break;
          case 500:
            $this->last_error = 'A system error was encountered in processing the request';
            break;
          default:
            $this->last_error = 'Unknown error during authentication. Response code: ' . $auth_response->code;
            break;            
        }
        watchdog('capwiz_connect', 'There was an error authenticating with Capwiz Connect for account %account_name: %error.', array('%account_name' => $this->account_details->display_name, '%error' => $this->last_error), WATCHDOG_ERROR);
      }
    }
    
    return $this->connected;
  }
  
  
  /**
   * Query users. If a timestamp is given, only return users updated since then.
   * 
   * @param $updated_since
   *   Timestamp. If given, only users updated since that time will be returned.
   */
  public function queryUsers($updated_since = 0) {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    $query_params['sessionId'] = $this->session_id;
    
    // If requesting users updated since some timestamp
    if ($updated_since > 0) {
      $query_params['updatedSince'] = date('c', $updated_since);
    }
    
    $url = url(self::$base_url . 'users/v1/client/' . $this->client_id . '/users', array('query' => $query_params));
    
    $user_response = drupal_http_request($url);
    
    // If the query was successfull
    if ($user_response->code == 200) {
      $response_data = json_decode($user_response->data);
      if ($response_data) {
        $this->last_error = t('Users queried successfully');
        $users = $response_data->users;
        return $users;
      }
      else {
        watchdog('capwiz_connect', 'There was an error querying users with Capwiz Connect for account %account_name. Response: %error.', array('%account_name' => $this->account_details->display_name, '%error' => print_r($auth_response->data, TRUE)), WATCHDOG_ERROR);
      }
    }
    // Error with user query
    else {
      switch ($user_response->code) {
        case 500:
          $this->last_error = 'A system error was encountered in processing the request';
          break;
        default:
          $this->last_error = 'Unknown error while attempting to query users. Response code: ' . $user_response->code;
          break;            
      }
      watchdog('capwiz_connect', 'There was an error querying users with Capwiz Connect for account %account_name: %error.', array('%account_name' => $this->account_details->display_name, '%error' => $this->last_error), WATCHDOG_ERROR);
    }
    
    return FALSE;
  }
  
  
  /**
   * Get a single user by email
   */
  public function getUserByEmail($email) {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    $query_params['sessionId'] = $this->session_id;
    
    
    $url = url(self::$base_url . 'users/v1/client/' . $this->client_id . '/user/email/' . $email, array('query' => $query_params));
    
    $user_response = drupal_http_request($url);
    
    // If the query was successfull
    if ($user_response->code == 200) {
      $response_data = json_decode($user_response->data);
      if ($response_data) {
        $this->last_error = t('User queried successfully');
        return $response_data;
      }
      else {
        watchdog('capwiz_connect', 'User could not be found by email (%email) with Capwiz Connect for account %account_name. Response: %error.', array('%email' => $email, '%account_name' => $this->account_details->display_name, '%error' => print_r($auth_response->data, TRUE)), WATCHDOG_ERROR);
      }
    }
    // Error with user query
    else {
      switch ($user_response->code) {
        case 404:
          $this->last_error = 'The user identified by email address was not found.';
          break;
        case 500:
          $this->last_error = 'A system error was encountered in processing the request';
          break;
        default:
          $this->last_error = 'Unknown error while attempting to query users. Response code: ' . $user_response->code;
          break;            
      }
      watchdog('capwiz_connect', 'There was an error finding user by email (%email) with Capwiz Connect for account %account_name: %error.', array('%email' => $email, '%account_name' => $this->account_details->display_name, '%error' => $this->last_error), WATCHDOG_ERROR);
    }
    
    return FALSE;
  }
}