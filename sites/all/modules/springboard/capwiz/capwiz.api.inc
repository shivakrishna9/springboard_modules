<?php
// $Id$

/**
 * @file capwiz.api.inc
 * Communicate with Capwiz service.
 *
 * @author Derek Dunagan <derek.dunagan@jacksonriver.com>
 */


class CapwizApi
{


  private static $_soapClient;
  private static $_soapClientFunctions;

  private static $_authenticationArguments;

  public static $accounts;
  public static $actions;

  private static $_uninitialized = TRUE;


  private function __construct() {}


  /**
   * Initialize the SoapClient and caching mechanism.
   *
   * @return void
   */
  private static function _init() {

    if (self::$_uninitialized) {

      try {
        self::$_soapClient = new SoapClient(dirname(__FILE__) . '/capwizxc.wsdl');

        self::$_authenticationArguments = array(
          'client_id' => variable_get('capwiz_client_id', ''),
          'authuser'  => variable_get('capwiz_authuser',  ''),
          'authpass'  => variable_get('capwiz_authpass',  ''),
        );

        $cache  = cache_get('capwiz');
        $cached = !empty($cache) && is_array($cache->data);

        self::$accounts = $cached && isset($cache->data['accounts']) ? $cache->data['accounts'] : array();
        self::$actions  = $cached && isset($cache->data['actions'])  ? $cache->data['actions']  : array();

        // Register a shutdown function to cache any unsaved Capwiz data.
        register_shutdown_function(array('CapwizApi', 'saveCache'));

        self::$_uninitialized = FALSE;
      }
      catch (SoapFault $e) {
        self::_logSoapFault($e);
        self::$_soapClient = FALSE;
      }
      catch (Exception $e) {
        self::_logException($e);
        self::$_soapClient = FALSE;
      }

    }
  }


  /**
   * Call a Capwiz API function.
   *
   * @param string $function
   * @param array|StdClass $arguments (optional)
   * @param bool $authenticate (optional)
   *   Whether or not this function must authenticate in order to run.
   * @param bool $throttle (optional)
   *   Whether or not this function needs the throttle argument.
   * @return mixed
   *   Return value varies depending on the function that was called.
   *   Errors return FALSE.
   */
  private static function _callFunction($function, $arguments = array(), $authenticate = FALSE, $throttle = FALSE) {

    self::_init();
    if (!empty(self::$_soapClient)) {

      $real_function = FALSE;

      // Statically cache the available functions.
      if (empty(self::$_soapClientFunctions)) {
        self::$_soapClientFunctions = self::$_soapClient->__getFunctions();
      }

      foreach (self::$_soapClientFunctions as $function_info) {
        if (strpos($function_info, $function) === 0) {
          $real_function = $function;
          break;
        }
      }
    }

    $function = $real_function;

    if (!empty($function)) {

      try {

        $function_arguments = array();

        if ($authenticate) {
          $function_arguments += self::$_authenticationArguments;
        }

        if ($throttle) {
          $function_arguments += array('throttle' => 'true');
        }

        $function_arguments += (array)$arguments;

        $response = self::$_soapClient->$function((object)$function_arguments);

        if (!empty($response)) {
          self::_logCapwizMessage($response);
          return $response;
        }
        else {
          self::_logNoCapwizResponse($function);
        }
      }
      catch (SoapFault $e) {
        self::_logSoapFault($e);
      }
      catch (Exception $e) {
        self::_logException($e);
      }
    }

    return FALSE;
  }


  /**
   * Reset the "date from" for getting Capwiz accounts.
   *
   * This method ensures *all* accounts are retrieved
   * on the next Capwiz::getAccounts() call.
   *
   * @return void
   *
   * @see _callFunction()
   */
  private static function _resetApiTimestamp() {

    $arguments = array('reset_date' => NULL);
    self::_callFunction('reset_sync_window', $arguments, TRUE);
  }


  /**
   * Helper method: Fetch a Capwiz object.
   * 
   * @param StdClass $response
   *   The response of a Capwiz API call
   * @param string $parent
   *   The parent property in the response (e.g. 'actions', 'people')
   * @param string $child
   *   The child property in the response (e.g. 'action', 'person')
   * @return array|bool
   *   An array of objects, FALSE on error
   */
  private static function _fetchObjects($response, $parent, $child) {

    if (!empty($response) && is_object($response) && property_exists($response, $parent)) {

      $objects = is_object($response->$parent) && property_exists($response->$parent, $child) ? $response->$parent->$child : array();
      
      if (is_object($objects)) {
        $objects = array($objects);
      }

      $objects = array_merge(self::$$parent, $objects);

      if (!empty($objects)) {
        self::$$parent = $objects;
      }

      return $objects;
    }

    return FALSE;
  }



  /*********************\
  ** Public-facing API **
  \*********************/


  /**
   * Get Capwiz user accounts and cache them.
   *
   * Capwiz API function: get_latest_people_plus
   *
   * get_latest_people_plus() returns custom_fields.
   * get_latest_people() does not.
   *
   * @param bool $get_all_accounts (optional)
   *   If FALSE, just gets the new/updated accounts
   *   since the last time this function ran
   * @return array|bool
   *   Returns FALSE if accounts could not be retrieved
   *
   * @see _callFunction()
   * @see _fetchObjects()
   */
  public static function fetchAccounts($get_all_accounts = FALSE) {

    if ((bool)$get_all_accounts) {
      self::_resetApiTimestamp();
    }

    $response = self::_callFunction('get_latest_people_plus', array(), TRUE, TRUE);

    return self::_fetchObjects($response, 'people', 'person');
  }


  /**
   * Get Capwiz advocacy actions and cache them.
   *
   * This method returns all the actions from the last time
   * it ran to the last time Capwiz::getAccounts() ran.
   *
   * Capwiz API function: get_latest_actions
   *
   * @return array|bool
   *   Returns FALSE if actions could not be retrieved
   *
   * @see _callFunction()
   * @see _fetchObjects()
   */
  public static function fetchActions() {

    $response = self::_callFunction('get_latest_actions', array(), TRUE, TRUE);

    return self::_fetchObjects($response, 'actions', 'action');
  }


  /**
   * Get the fields of a Capwiz personObject as an array.
   *
   * The Capwiz API is comprised of only *get* functions.
   * There are no describe object functions or the like.
   * Therefore, we have to hard code this information.
   *
   * @return array
   *
   * @todo Add support for customFields.
   */
  public static function getAccountFields() {

    return array(
      // 'external_id', Not used
      // 'capwiz_id', Managed automatically
      // 'email_address', Managed automatically
      'honorific',
      'first_name',
      'last_name',
      'subscription_status',
      'federal_district',
      'state_district_upper',
      'state_district_lower',
      'address',
      'city',
      'state',
      'zip',
      'country',
      'phone',
      'fax',
      // 'last_update', Not used
    );
  }


  /**
   * Get the fields of a Capwiz action as an array.
   *
   * The Capwiz API is comprised of only *get* functions.
   * There are no describe object functions or the like.
   * Therefore, we have to hard code this information.
   *
   * @return array
   */
  public static function getActionFields() {

    return array(
      // 'external_id', Not used
      // 'capwiz_id', Based on uid instead
      // 'email_address', Based on uid instead
      'event_id',
      'alert_id',
      'alert',
      'recipient_full',
      'action_date',
      'subject',
      'intended_delivery_method',
      'actual_delivery_method',
      'delivery_status',
      // 'message_id', Not used
    );
  }


  /**
   * Cache any unsaved data.
   *
   * @return void
   */
  public static function saveCache() {

    $data = array();

    if (!empty(self::$accounts)) {
      $data['accounts'] = self::$accounts;
      self::_logCacheMessage(count($data['accounts']), 'accounts');
    }

    if (!empty(self::$actions)) {
      $data['actions'] = self::$actions;
      self::_logCacheMessage(count($data['actions']), 'actions');
    }

    if (empty($data)) {
      cache_clear_all('capwiz', 'cache');
    }
    else {
      cache_set('capwiz', $data);
    }
  }


  /*******************\
  ** Message logging **
  \*******************/


  /**
   * Log a Capwiz message.
   *
   * @param StdClass $soap_response
   * @return void
   */
  private static function _logCapwizMessage($soap_response) {

    watchdog('capwiz', '(@id) @status (transaction time: @time)', array('@id' => $soap_response->transaction_id, '@status' => $soap_response->status_msg, '@time' => $soap_response->transaction_time));
  }


  /**
   * Log a Capwiz message.
   *
   * @param StdClass $soap_response
   * @return void
   */
  private static function _logCacheMessage($number, $thing) {

    watchdog('capwiz', '!number !thing were not saved. They will be saved on the next cron run.', array('!number' => $number, '!thing' => $thing));
  }


  /**
   * Log a message when Capwiz does not respond.
   *
   * @param string $function
   *   The API function that tried to communicate with Capwiz
   * @return void
   */
  private static function _logNoCapwizResponse($function) {

    watchdog('capwiz', 'The Capwiz service did not respond when calling the @function API function.', array('@function' => "$function()"), WATCHDOG_ALERT);
  }


  /**
   * Log a SoapFault exception.
   *
   * @param SoapFault $e
   * @return void
   */
  private static function _logSoapFault($e) {

    watchdog('capwiz', '(@faultcode) @faultstring @detail', array('@faultcode' => $e->faultcode, '@faultstring' => $e->faultstring, '@detail' => $e->detail), WATCHDOG_ERROR);
  }


  /**
   * Log an Exception.
   *
   * @param Exception $e
   * @return void
   */
  private static function _logException($e) {

    watchdog('capwiz', '(@code) @message (@filename: @line) !filepath', array('@code' => $e->getCode(), '@message' => $e->getMessage(), '@filename' => basename($e->getFile()), '@line' => $e->getLine(), '!filepath' => '<br /><em>' . check_plain($e->getFile()) . '</em>'), WATCHDOG_ERROR);
  }


}

