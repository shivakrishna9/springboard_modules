<?php

class CapizConnect {
  public static $base_url = 'https://connect.capwiz.com/api/';

  private $connected = FALSE;
  private $account_details;
  private $last_error;
  
  private $client_id;
  private $session_id;
  
  private $last_user_offset = 0;
  
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
  
  
  
  /***********************************************
   * User functions
   */
   
  /**
   * Query users. If a timestamp is given, only return users updated since then.
   * 
   * @param $updated_since
   *   Timestamp. If given, only users updated since that time will be returned.
   * @param $start
   *   User to start at. If given, this will be used as the start value for the
   *   query, since some queries will result in multiple batches of users.
   * @param $batch_size
   *   Number of items to include in a given batch
   */
  public function queryUsers($updated_since = 0, $start = 0, $batch_size = 500) {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    $query_params['sessionId'] = $this->session_id;
    
    // If requesting users updated since some timestamp
    if ($updated_since > 0) {
      // Date must be in ISO 8601 format
      $query_params['updatedSince'] = date('c', $updated_since);
    }
    
    // If requesting a second (or third or fourth or...) batch of users, start at
    // some point in the result set.
    $query_params['start'] = (int)$start;
    // If a batch size is given, only get that many items
    if ($batch_size > 0) {
      $query_params['end'] = $query_params['start'] + $batch_size;
    }
    
    $url = url(self::$base_url . 'users/v1/client/' . $this->client_id . '/users', array('query' => $query_params));
    
    $user_response = drupal_http_request($url);
    
    // If the query was successfull
    if ($user_response->code == 200) {
      $response_data = json_decode($user_response->data);
      if ($response_data) {
        $this->last_error = t('Users queried successfully');
        $users = $response_data->users;

        // Check if there are more users to get
        if ($response_data->moreData == 'true') {
          // Determine the offset of the last item
          $last_user = end($users);
          $this->last_user_offset = $last_user->offset;
          reset($users);
        }
        else {
          // No more users to sync down. Reset the offset for the next run to 0 so
          // it starts over
          $this->last_user_offset = 0;
        }
        
        // Save the new user offset so we know where to start the next batch
        db_query('UPDATE {capwiz_connect_accounts} SET user_sync_offset=%d WHERE id=%d', $this->last_user_offset, $this->account_details->id);
        
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
   * Return whether or not there are more users to collect from the 
   */
  function hasMoreUsers() {
    return ($this->last_user_offset > 0);
  }
  
  /**
   * Return the offset of the last user that was returned by a query
   */
  function lastUserOffset() {
    return $this->last_user_offset;
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
        $return_fields = array();
        foreach($fields as $field) {
          $return_fields[$field->name] = $field;
        }
        return $return_fields;
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
  
  
  /**
   * Get the definition of a specific user field
   * 
   * @param $field_name
   *   Key of the custom field, matching to the field->name value
   */
  public function getSpecificCustomUserField($field_name) {
    if ($this->connected) {
      $fields = $this->getCustomUserFields();
      if ($fields) {
        return $fields[$field_name];
      }
    }
    return NULL;
  }
  
  
  
  /**
   * Sync a batch of users down from Capwiz
   */
  public function syncUsersBatch() {
    // Note when we started this batch, to update user_sync_since if this batch
    // completes the full user sync. Subtract five minutes to ensure that no one
    // falls through the cracks between batches
    $batch_start_time = time() - 300;
    
    // Query for users that have been updated since the last time
    $sync_since = $this->account_details->user_sync_since;
    $start_offset = $this->account_details->user_sync_offset;
    $users = $this->queryUsers($sync_since, $start_offset);
    
    if (!$this->hasMoreUsers()) {
      $this->account_details->user_sync_since = $batch_start_time;
      db_query('UPDATE {capwiz_connect_accounts} SET user_sync_since=%d WHERE id=%d', $this->account_details->user_sync_since, $this->account_details->id);
    }

    // If $users is FALSE, there was an error connecting or querying users, and a
    // message will be logged to the watchdog
    if ($users === FALSE) {
      return;
    }
    
    if (is_array($users) && !empty($users)) {
      // Loop over the returned users and save them to the DB
      foreach ($users as $user) {
        $capwiz_user_id = $user->id;
        $record = array(
          'capwiz_account_id' => $account_id,
          'capwiz_user_id' => $capwiz_user_id,
          'local_uid' => 0,
          'data' => serialize($user),
        );
        
        // Check if this user (identified by Capwiz ID) is already in our DB
        $existing_record = db_result(db_query("SELECT 1 FROM {capwiz_connect_users} WHERE capwiz_user_id='%s'", $capwiz_user_id));
        
        if ($existing_record) {
          // User is already in our DB: update the record
          if (drupal_write_record('capwiz_connect_users', $record, 'capwiz_user_id')) {
            $updated++;
          }
          else {
            $update_fail++;
          }
        }
        else {
          // New-to-us user
          if(drupal_write_record('capwiz_connect_users', $record)) {
            $created++;
          }
          else {
            $create_fail++;
          }
        }
        // Insert the user into the Salesforce processing queue
        if ($record['capwiz_user_id']) {
          sf_queue_insert($record['capwiz_user_id'], 'capwiz_connect_user', 'upsert');
        }
      }
      
      // Log stats for users collected for this account
      $severity = WATCHDOG_INFO;
      $message = "Updated users for %account_name. Created: %created. Updated: %updated.";
      if ($create_fail > 0) {
        $message .= " Failed to create: %create_fail.";
        $severity = WATCHDOG_ERROR;
      }
      if ($update_fail > 0) {
        $message .= " Failed to update: %update_fail.";
        $severity = WATCHDOG_ERROR;
      }
      $message_vars = array(
        '%account_name' => $this->account_details->display_name,
        '%created' => $created,
        '%updated' => $updated,
        '%create_fail' => $create_fail,
        '%update_fail' => $update_fail,
      );
      watchdog('capwiz_connect', $message, $message_vars, $severity);
    }
    else {
      // $users was an empty array, so there were no new users
      $message = "No users to add or update for %account_name.";
      $message_vars = array(
        '%account_name' => $this->account_details->display_name,
        '%created' => $created,
        '%updated' => $updated,
        '%create_fail' => $create_fail,
        '%update_fail' => $update_fail,
      );
      watchdog('capwiz_connect', $message, $message_vars);
    }
  }
  
  
  /***********************************************
   * Action functions
   */
   
  /**
   * Query actions. If a timestamp is given, only return items updated since then
   * 
   * @param $updated_since
   *   Timestamp. If given, only actions updated since that time will be returned.
   */
  public function queryActions($updated_since = 0) {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    $query_params['sessionId'] = $this->session_id;
    
    // If requesting actions updated since some timestamp
    if ($updated_since > 0) {
      // Date must be in ISO 8601 format
      $query_params['minDate'] = date('c', $updated_since);
    }
    
    $url = url(self::$base_url . 'activity/v1/client/' . $this->client_id . '/events', array('query' => $query_params));
    
    $action_response = drupal_http_request($url);
    
    // If the query was successfull
    if ($action_response->code == 200) {
      $response_data = json_decode($action_response->data);
      if ($response_data) {
        $this->last_error = t('Actions queried successfully');
        return $response_data->events;
      }
      else {
        watchdog('capwiz_connect', 'There was an error querying actions with Capwiz Connect for account %account_name. Response: %error.', array('%account_name' => $this->account_details->display_name, '%error' => print_r($auth_response->data, TRUE)), WATCHDOG_ERROR);
      }
    }
    // Error with action query
    else {
      switch ($action_response->code) {
        case 500:
          $this->last_error = 'A system error was encountered in processing the request';
          break;
        default:
          $this->last_error = 'Unknown error while attempting to query actions. Response code: ' . $action_response->code;
          break;            
      }
      watchdog('capwiz_connect', 'There was an error querying actions with Capwiz Connect for account %account_name: %error.', array('%account_name' => $this->account_details->display_name, '%error' => $this->last_error), WATCHDOG_ERROR);
    }
    
    return FALSE;
  }
  
}