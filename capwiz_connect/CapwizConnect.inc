<?php
/**
 * @file
 * CapwizConnect class. Encapsualtes functions to interact with Capwiz Connect API
 */
 
class CapizConnect {
  public static $base_url = 'https://connect.capwiz.com/api/';

  private $connected = FALSE;
  private $account_details;
  private $last_error;
  
  private $client_id;
  private $session_id;
  
  private $last_user_offset = 0;
  private $last_action_offset = 0;

  // Status values when saving users
  public static $STAT_UPDATE_OK = 1;
  public static $STAT_UPDATE_FAIL = 2;
  public static $STAT_CREATE_OK = 4;
  public static $STAT_CREATE_FAIL = 8;
  
  // Status value when saving actions
  public static $STAT_DUPLICATE_MESSAGE = 16;
  
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
    return NULL;
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
    if (!empty($cached_session_id->data)) {
      $this->session_id = $cached_session_id->data;
      $this->client_id = $cached_client_id->data;
      $this->connected = TRUE;
    }
    // No Session ID cached, so authenticate now
    elseif ($this->account_details->user_name && $this->account_details->user_password) {
      $query_params = array(
        'loginName' => decrypt($this->account_details->user_name),
        'password' => decrypt($this->account_details->user_password),
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
  public function queryUsers($updated_since = 0, $start = 0, $batch_size = 100) {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    $query_params['sessionId'] = $this->session_id;
    
    // Specify that what officials we want in the result set.
    // From https://connect.capwiz.com/documentation/lookup/v1/index.html:
    //   PR  President
    //   NS  National/Federal Senator
    //   NH  National/Federal Representative
    //   G   Governor
    //   SL  State Legislator
    //   SE  State Executive (including the Governor, Lt. Governor, etc...)
    $query_params['officialTypes'] = 'NS,NH,G';
      
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
        
        // Sanity check each user to ensure data will not cause errors
        foreach ($users as $user) {
          if (empty($user->id) || empty($user->emails[0]->address)) {
            watchdog('capwiz_connect', 'There was an error querying users with Capwiz Connect for account %account_name. Incorrect data passed for user: %user.', array('%account_name' => $this->account_details->display_name, '%user' => print_r($user, TRUE)), WATCHDOG_ERROR);
            return FALSE;
          }
        }

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
   * Return whether or not there are more users to collect from the API
   */
  public function hasMoreUsers() {
    return ($this->last_user_offset > 0);
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
        foreach ($fields as $field) {
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
    $batch_start_time = time() - 1800;
    
    // Query for users that have been updated since the last time
    $sync_since = $this->account_details->user_sync_since;
    $start_offset = $this->account_details->user_sync_offset;
    $batch_size = variable_get('capwiz_connect_user_batch_size', 100);
    $users = $this->queryUsers($sync_since, $start_offset, $batch_size);

    // If $users is FALSE, there was an error connecting or querying users, and
    // a message will be logged to the watchdog
    if ($users === FALSE) {
      return;
    }

    if (is_array($users) && !empty($users)) {
      // Prep an array to keep track of added/updated users
      $save_counts = array(
        self::$STAT_CREATE_OK => 0,
        self::$STAT_UPDATE_OK => 0,
        self::$STAT_CREATE_FAIL => 0,
        self::$STAT_UPDATE_FAIL => 0,
      );   

      // Loop over the returned users and save them to the DB
      foreach ($users as $user) {
        $save_stat = $this->saveUser($user);
        $save_counts[$save_stat]++;
      }

      // Log stats for users collected for this account
      $severity = WATCHDOG_INFO;
      $message = "Updated users for %account_name. Created: %created. Updated: %updated.";
      if ($save_counts[self::$STAT_CREATE_FAIL] > 0) {
        $message .= " Failed to create: %create_fail.";
        $severity = WATCHDOG_ERROR;
      }
      if ($save_counts[self::$STAT_UPDATE_FAIL]> 0) {
        $message .= " Failed to update: %update_fail.";
        $severity = WATCHDOG_ERROR;
      }
      $message_vars = array(
        '%account_name' => $this->account_details->display_name,
        '%created' => $save_counts[self::$STAT_CREATE_OK],
        '%updated' => $save_counts[self::$STAT_UPDATE_OK],
        '%create_fail' => $save_counts[self::$STAT_CREATE_FAIL],
        '%update_fail' => $save_counts[self::$STAT_UPDATE_FAIL],
      );
      watchdog('capwiz_connect', $message, $message_vars, $severity);
    }
    else {
      // $users was an empty array, so there were no new users
      $message = "No users to add or update for %account_name.";
      $message_vars = array(
        '%account_name' => $this->account_details->display_name,
      );
      watchdog('capwiz_connect', $message, $message_vars);
    }
    
    // If this is the last batch of users, set the last sync time to right now
    // so that the next batch will only query users who are updated after right
    // now.  Also, reset the offset.
    if (!$this->hasMoreUsers()) {
      $this->account_details->user_sync_since = $batch_start_time;
      db_query('UPDATE {capwiz_connect_accounts} SET user_sync_since=%d, user_sync_offset=%d WHERE id=%d', $this->account_details->user_sync_since, 0, $this->account_details->id);
    }
    else {
      // Save the new user offset so we know where to start the next batch
      db_query('UPDATE {capwiz_connect_accounts} SET user_sync_offset=%d WHERE id=%d', $this->last_user_offset, $this->account_details->id);
    }
    
  }
  
  
  
  /**
   * Local method to save a Capwiz user as a Drupal user and add the user to
   * the Salesforce processing queue
   * 
   * @param $user
   *   Object representing a user that was pulled over from Capwiz
   * 
   * @return int
   *   Returns a status matching a static $STAT_* variable from this class
   */
  private function saveUser($user) {
    $return_value = 0;
    
    $capwiz_user_id = $user->id;
    
    $email = $user->emails[0]->address;
    $existing_profile_values = array();
    
    if (!empty($email)) {
      $drupal_account = user_load(array('mail' => $email));
      if ($drupal_account) {
        $return_value = self::$STAT_UPDATE_OK;
        // Load profile values
        if (module_exists('profile')) {
          $result = db_query('SELECT fid, value FROM {profile_values} WHERE uid=%d', $drupal_account->uid);
          while ($row = db_fetch_object($result)) {
            $existing_profile_values[$row->fid] = $row->value;
          }
        }
      }
      else {
        $drupal_account_password = user_password();
        $drupal_account = array(
          'name'    => $email,
          'pass'    => md5($drupal_account_password),
          'mail'    => $email,
          'created' => $time = time(),
          'access'  => $time,
          'login'   => 0,
          'status'  => 1,
        );

        db_query('INSERT INTO {users} (name, pass, mail, created, access, login, status) VALUES ("%s", "%s", "%s", %d, %d, %d, %d)', $drupal_account);
        $drupal_account = (object)$drupal_account;
        $drupal_account->uid  = db_last_insert_id('users', 'uid');
        $return_value = self::$STAT_CREATE_OK;
      }
    }
    
    // Add the new/updated user to the SF queue
    if (module_exists('sf_user') && $drupal_account->uid > 0) {
      sf_queue_insert($drupal_account->uid, 'user', 'upsert');
    }
    
    // Round up the details on any legislators for this user
    $legislators = array();
    if (!empty($user->officials)) {
      foreach ($user->officials as $o) {
        $level = $o->level->label;
        $chamber = $o->position->label;
        $label = $o->title . ' ' . $o->firstName . ' ' . $o->lastName;
        $phone = is_object($o->offices[0]) ? $o->offices[0]->phone : '';
        // @todo: Phone and email will be relocated into another data structure
        $legislators[$level][$chamber][] = array(
          'name' => $label,
          'phone' => $phone,
          'email' => $o->emailAddress,
        );
      }
    }
    
    // Make sure the map field is an array
    if (!is_array($this->account_details->mapping_user['map'])) {
      $this->account_details->mapping_user['map'] = array();
    }
    
    
    

    foreach ($this->account_details->mapping_user['map'] as $field_key => $target_field) {
      $business_rule = $this->account_details->mapping_user['business'][$field_key];
      $value = '';

      // Handle the mapping from Capwiz field to Drupal user field
      switch ($field_key) {
        case 'capwiz_user_id':
          $value = $capwiz_user_id;
          break;
        
        // Salutation
        case 'honorificId':
          if ($user->honorific) {
            $value = $user->honorific->name;
          }
          break;
          
        // Street address
        case 'street1':
        case 'street2':
        case 'city':
        case 'state':
        case 'postCode':
        case 'country':
          if ($user->addresses[0]) {
            $value = $user->addresses[0]->$field_key;
          }
          break;

        // Zip+4 appears as postPlus in the field info, but postCodePlus in the
        // user's address details
        case 'postPlus':
        case 'postCodePlus':
          if ($user->addresses[0]) {
            $value = $user->addresses[0]->postCodePlus;
          }
          break;
          
        // Type of address
        case 'addressType':
          $address_field = $this->getSpecificCustomUserField('addressType');
          if ($user->addresses[0] && $address_field) {
            $address_type_id = $user->addresses[0]->addressTypeId;
            $value = $address_field->options[$address_type_id]->label;
          }
          break;
        
        // Affiliation of form
        case 'form_affiliation':
          $value = $this->account_details->form_affiliation;
          break;
      
        // National District
        case 'nationalDistrict':
          $value = $user->districtInfo->national;
          break;
        
        // State districts
        case 'stateDistrictUpper':
          if (is_array($user->districtInfo->stateUpper)) {
            $value = implode(',', $user->districtInfo->stateUpper);
          }
          break;
        case 'stateDistrictLower':
          if (is_array($user->districtInfo->stateLower)) {
            $value = implode(',', $user->districtInfo->stateLower);
          }
          break;
        
        // Federal senators
        case 'fed_senator_1':
          $value = $legislators['National']['Senate'][0]['name'];
          break;          
        case 'fed_senator_2':
          $value = $legislators['National']['Senate'][1]['name'];
          break;
      
        // Federal senator phone numbers
        case 'fed_senator_1_phone':
          $value = $legislators['National']['Senate'][0]['phone'];
          break;
        case 'fed_senator_2_phone':
          $value = $legislators['National']['Senate'][1]['phone'];
          break;
        
        // Federal senator email
        case 'fed_senator_1_email':
          $value = $legislators['National']['Senate'][0]['email'];
          break;
        case 'fed_senator_2_email':
          $value = $legislators['National']['Senate'][1]['email'];
          break;
        
        // Federal rep
        case 'fed_rep':
          $value = $legislators['National']['House'][0]['name'];
          break;        
        // Federal rep phone number
        case 'fed_rep_phone':
          $value = $legislators['National']['House'][0]['phone'];
          break;
        // Federal rep email
        case 'fed_rep_email':
          $value = $legislators['National']['House'][0]['email'];
          break;
          
        // Governor fields
        case 'governor':
          $value = $legislators['State']['Governor'][0]['name'];
          break;
        case 'governor_phone':
          $value = $legislators['State']['Governor'][0]['phone'];
          break;
        case 'governor_email':
          $value = $legislators['State']['Governor'][0]['email'];
          break;
          
      
        // Every other field. Just load the field from the object
        default:
          if ($user->{$field_key}) {
            $value = $user->{$field_key};
          }
      }
      
      // Save the value
      // If it's a profile field
      if ((int)$target_field > 0) {
        // Always overwrite, or overwrite when blank
        if (empty($business_rule) || $business_rule == 'always' || ($business_rule == 'blank' && empty($existing_profile_values[$target_field]))) {
          db_query('INSERT INTO {profile_values} (uid, fid, value) VALUES (%d, %d, "%s") ON DUPLICATE KEY UPDATE value = "%s"', $drupal_account->uid, $target_field, $value, $value);
        }
        // Append
        elseif ($business_rule == 'append') {
          if ($existing_profile_values[$target_field]) {
            $value = $existing_profile_values[$target_field] . ' ' . $value;
          }
          db_query('INSERT INTO {profile_values} (uid, fid, value) VALUES (%d, %d, "%s") ON DUPLICATE KEY UPDATE value = "%s"', $drupal_account->uid, $target_field, $value, $value);
        }
      }
      // Not a profile field, but a regular user field
      else {
      }      
    }

    module_invoke_all('capwiz_connect_save_user', $user, $drupal_account);

    return $return_value;
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
  public function queryActions($updated_since = 0, $start = 0, $batch_size = 100) {
    if (!$this->connected) {
      return FALSE;
    }
    
    $query_params = array();
    // Only for dev testing
    $query_params['sessionId'] = $this->session_id;
    
    // If requesting actions updated since some timestamp
    if ($updated_since > 0) {
      // Date must be in ISO 8601 format
      $query_params['minDate'] = date('c', $updated_since);
    }
    
    // If requesting a second (or third or fourth or...) batch of actions,
    // start at some point in the result set.
    $query_params['start'] = (int)$start;
    // If a batch size is given, only get that many items
    if ($batch_size > 0) {
      $query_params['end'] = $query_params['start'] + $batch_size;
    }

    $url = url(self::$base_url . 'activity/v1/client/' . $this->client_id . '/events', array('query' => $query_params));
    
    $action_response = drupal_http_request($url);
    
    // If the query was successfull
    if ($action_response->code == 200) {
      $response_data = json_decode($action_response->data);
      if ($response_data) {
        $this->last_error = t('Actions queried successfully');
        $actions = $response_data->events;
        
        // Sanity check each action to ensure data will not cause errors
        foreach ($actions as $action) {
          if (empty($action->alertId) || empty($action->messageId) || empty($action->emailAddress)) {
            watchdog('capwiz_connect', 'There was an error querying actions with Capwiz Connect for account %account_name. Incorrect data was passed for this action: %action.', array('%account_name' => $this->account_details->display_name, '%action' => print_r($action, TRUE)), WATCHDOG_ERROR);
            return FALSE;
          }
        }

        // Determine the offset of the last item
        $last_action = end($actions);
        $this->last_action_offset = !empty($last_action->offset) ? $last_action->offset : 0;
        reset($actions);
        
        $this->more_data = ($response_data->moreData == 'true') ? TRUE : FALSE; 

        return $actions;      
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
  
  
  /**
   * Return whether or not there are more actions to collect from the API
   */
  public function hasMoreActions() {
    return $this->more_data;
  }
  
  
  /**
   * Sync a batch of action from Capwiz
   */
  public function syncActionsBatch() {
    // Note when we started this batch, to update action_sync_since if this
    // batch completes the full action sync. Subtract five minutes to ensure
    // that no actions falls through the cracks between batches
    $batch_start_time = time() - 1800;
    
    // Query for actions that have been updated since the last time
    $sync_since = $this->account_details->action_sync_since;
    // Add one to the offset to keep from importing the last action
    $start_offset = $this->account_details->action_sync_offset+1;
    $batch_size = variable_get('capwiz_connect_action_batch_size', 100);
    $actions = $this->queryActions($sync_since, $start_offset, $batch_size);
    
    // If $actions is FALSE, there was an error connecting or querying actions,
    // and a message will be logged to the watchdo
    if ($actions === FALSE) {
      return;
    }
    

    if (is_array($actions) && !empty($actions)) {
      // Prep an array to keep track of added/updated users
      $save_counts = array(
        self::$STAT_CREATE_OK => 0,
        self::$STAT_UPDATE_OK => 0,
        self::$STAT_CREATE_FAIL => 0,
        self::$STAT_UPDATE_FAIL => 0,
        self::$STAT_DUPLICATE_MESSAGE => 0,
      );
        
      // Loop over the returned users and save them to the DB
      foreach ($actions as $action) {
        $save_stat = $this->saveAction($action);
        $save_counts[$save_stat]++;
      }
      
      // Log stats for users collected for this account
      $severity = WATCHDOG_INFO;
      $message = "Updated actions for %account_name. Created: %created. Ignored existing: %existing.";
      if ($save_counts[self::$STAT_CREATE_FAIL] > 0) {
        $message .= " Failed to create: %create_fail.";
        $severity = WATCHDOG_ERROR;
      }
      if ($save_counts[self::$STAT_UPDATE_FAIL]) {
        $message .= " Failed to update: %update_fail.";
        $severity = WATCHDOG_ERROR;
      }
      if ($save_counts[self::$STAT_DUPLICATE_MESSAGE]) {
        $message .= " Ignored duplicate messages: %duplicate_message.";
      }
      $message_vars = array(
        '%account_name' => $this->account_details->display_name,
        '%created' => $save_counts[self::$STAT_CREATE_OK],
        '%existing' => $save_counts[self::$STAT_UPDATE_OK],
        '%create_fail' => $save_counts[self::$STAT_CREATE_FAIL],
        '%update_fail' => $save_counts[self::$STAT_UPDATE_FAIL],
        '%duplicate_message' => $save_counts[self::$STAT_DUPLICATE_MESSAGE],
      );
      watchdog('capwiz_connect', $message, $message_vars, $severity);
    }
    else {
      // $actions was an empty array, so there were no new users
      $message = "No actions to add or update for %account_name.";
      $message_vars = array(
        '%account_name' => $this->account_details->display_name,
      );
      watchdog('capwiz_connect', $message, $message_vars);
    }

    // We have no more items to import, reset the settings
    if (!$this->hasMoreActions()) {
      $time_set = $batch_start_time;
      $offset_set = 0;
    }
    // We have more items the time remains the same and the offset gets updated
    else {
      $time_set = $this->account_details->action_sync_since;
      $offset_set = $this->last_action_offset;      
    }

    // Update the settings
    db_query('UPDATE {capwiz_connect_accounts} SET action_sync_since=%d, action_sync_offset=%d WHERE id=%d', $time_set, $offset_set, $this->account_details->id);

  }
  
  /**
   * Local method to save an action
   * 
   * @param $action
   *   Object representing an action that was pulled over from Capwiz
   * 
   * @return int
   *   Returns a status matching a static $STAT_* variable from this class
   */
  private function saveAction($action) {
    $return_value = 0;
    
    $message_id = $action->messageId;
    $record = array(
      'message_id' => $message_id,
      'capwiz_account_id' => $this->account_details->id,
      'capwiz_alert_id' => $action->alertId,
      'capwiz_user_id' => $action->userId,
      'data' => serialize($action),
      'action_timestamp' => strtotime($action->dtAction),
    );

    // Check if this is a duplicate message. Capwiz creates an event for each message recipient selected by the user, we only need to record one of them
    $duplicate_message = db_result(db_query("SELECT id FROM {capwiz_connect_actions} WHERE capwiz_alert_id='%s' AND capwiz_user_id='%s' AND action_timestamp=%d AND message_id != %d", $record['capwiz_alert_id'], $record['capwiz_user_id'], $record['action_timestamp'], $message_id));
    if ($duplicate_message > 0) {
      return self::$STAT_DUPLICATE_MESSAGE;
    }
        
    // Check if this action (identified by message ID) is already in our DB
    $existing_record_id = db_result(db_query("SELECT id FROM {capwiz_connect_actions} WHERE message_id='%s'", $message_id));
      
    if ($existing_record_id > 0) {
      // If its an existing message do nothing
      return self::$STAT_UPDATE_OK;
    }
    else {
      // New-to-us user
      if (drupal_write_record('capwiz_connect_actions', $record)) {
        $return_value = self::$STAT_CREATE_OK;
      }
      else {
        $return_value = self::$STAT_CREATE_FAIL;
      }
    }
    
    // Insert the action into the Salesforce processing queue
    if ($record['id']) {
      sf_queue_insert($record['id'], 'capwiz_connect_action', 'upsert');
    }
    
    return $return_value;
  }
  
  /***********************************************
   * Alert functions
   */
   
  /**
   * Query for alerts
   */
  public function queryAlerts() {
    if (!$this->connected) {
      return FALSE;
    }

    $query_params['sessionId'] = $this->session_id;

    $url = url(self::$base_url . 'interests/v1/client/' . $this->client_id . '/alerts', array('query' => $query_params));

    $response = drupal_http_request($url);

    $response_data = NULL;
    // If the query was successfull
    if ($response->code == 200) {
      $response_data = json_decode($response->data);
    }
    return $response_data;
  }
  
  /**
   * Query an alert
   */
  public function queryAlert($id) {
    if (!$this->connected) {
      return FALSE;
    }

    $query_params['sessionId'] = $this->session_id;

    $url = url(self::$base_url . 'messagedelivery/v1/client/' . $this->client_id . '/messageForm/alert/' . $id, array('query' => $query_params));

    $response = drupal_http_request($url);

    $response_data = NULL;
    // If the query was successfull
    if ($response->code == 200) {
      $response_data = json_decode($response->data);
    }
    return $response_data;
  }
}