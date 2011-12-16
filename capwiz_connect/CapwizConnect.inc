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
    // If this function gets called again after connecting, don't do anything
    if ($this->connected) {
      return $this->connected;
    }
    
    $session_id_cache_id = 'capwiz_session_id_' . $this->account_details->id;
    $client_id_cache_id = 'capwiz_cache_id_' . $this->account_details->id;
    $this->connected = FALSE;
    
    // Check if a session ID has already been retrieved in a previous authentication
    // call. Session IDs are good for four hours.
    $cached_session_id = cache_get($session_id_cache_id);
    $cached_client_id = cache_get($client_id_cache_id);
    if ($cached_session_id) {
      $this->session_id = $cached_session_id->data;
      $this->client_id = $cached_client_id->data;
      $this->connected = TRUE;
    }
    // No Session ID cached, so authenticate now
    elseif ($this->account_details->user_name && $this->account_details->user_password) {
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
          
          // Cache the session ID. Session IDs are good for a few hours, but to
          // make sure that we don't try to use a session ID after it has expired
          // (at this writing, there is a bug on their end related to this), only
          // cache the session ID for one hour.
          $expire_at = time() + 3600;
          cache_set($session_id_cache_id, $this->session_id, 'cache', $expire_at);
          cache_set($client_id_cache_id, $this->client_id, 'cache', $expire_at);
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
  
  
  /**
   * Get custom user fields from Capwiz
   */
  public function getCustomUserFields() {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    $query_params['sessionId'] = $this->session_id;
    
    $url = url(self::$base_url . 'users/v1/client/' . $this->client_id . '/customFields', array('query' => $query_params));
    
    $fields_response = drupal_http_request($url);
    
    // If the query was successfull
    if ($fields_response->code == 200) {
      $fields = json_decode($fields_response->data);
      if ($fields) {
        $this->last_error = t('Custom fields gathered successfully');
        return $fields;
      }
      else {
        watchdog('capwiz_connect', 'There was an error getting custom fields for users with Capwiz Connect for account %account_name. Response: %error.', array('%account_name' => $this->account_details->display_name, '%error' => print_r($auth_response->data, TRUE)), WATCHDOG_ERROR);
      }
    }
    // Error with user query
    else {
      switch ($fields_response->code) {
        case 500:
          $this->last_error = 'A system error was encountered in processing the request';
          break;
        default:
          $this->last_error = 'Unknown error while attempting to get custom fields for users. Response code: ' . $fields_response->code;
          break;
      }
      watchdog('capwiz_connect', 'There was an error while trying to get custom fields for users with Capwiz Connect for account %account_name: %error.', array('%account_name' => $this->account_details->display_name, '%error' => $this->last_error), WATCHDOG_ERROR);
    }
    
    return FALSE;
  }
}