<?php
/**
 * @file
 * Contains SpringboardAPIObject, an example API implemention.
 */

/**
 * Salesforce specific implementation of DrupalQueue.
 */
class SpringboardAPIObject {

  /**
   * The URL to execute queries against, of the format: http://example.com/springboard_api/.
   *
   * @var string
   */
  private $url;

  /**
   * The public key provided by Springboard API.
   *
   * @var string
   */
  private $public_key; // from module settings

  /**
   * The private key provided by Springboard API.
   *
   * @var string
   */
  private $private_key;

  /**
   * Debug mode.
   *
   * @var string
   */
  private $debug;

  /**
   * Construct object.
   *
   * @param string $url
   *   The URL to execute queries against, of the format: http://example.com/springboard_api.
   * @param string $public_key
   *   The public key provided by Springboard API.
   * @param string $private_key
   *   The private key provided by Springboard API.
   * @param boolean $debug
   *   Optional, debug mode.
   *
   * @return result
   *   Return true if successful connection or false if failure.
   */
  public function __construct($url, $public_key, $private_key, $debug = FALSE) {
    $this->url = $url;
    $this->public_key = $public_key;
    $this->private_key = $private_key;
    $this->debug = $debug;
    // Attempt base query to test settings.
    return $this->sb_api_test_base();
  }

  /**
   * Execute a base query against Springboard API.
   *
   * @param string $format
   *   Optional, return in json or xml format.
   *
   * @return result
   *   Return true if successful connection or false if failure.
   */
  public function sb_api_test_base($format = NULL) {
    if ($format == NULL) {
      $format = 'xml'; // or json
    }
    $url = $this->url;
    //URL: http://example.com/springboard_api
    $result = $this->_sb_api_test_make_call('index', $url);
    if ($result == FALSE) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Execute an index query against Springboard API.
   *
   * @param array $query
   *   Optional, a key=>value array with the following:
   *     page => integer  (optional), page number.
   *     pagesize => integer  (optional), records per page.
   * @param string $format
   *   Optional, return in json or xml format.
   *
   * @return result
   *   Return result in xml or json of query, or false if failure.
   */
  public function sb_api_test_index($query = array(), $format = NULL) {
    if ($format == NULL) {
      $format = 'xml'; // or json
    }
    $url = $this->url . '/webform' . '.' . $format;
    //URL: http://example.com/springboard_api/webform[.<response format>]?key=<pubkey hash>
    // &time=<timestamp>&checksum=<checksum hash>[&page=<page>][&pagesize=<number of records>]
    return $this->_sb_api_test_make_call('index', $url, $query);
  }

  /**
   * Execute a retrieve query against Springboard API.
   *
   * @param integer $nid
   *   The node id of the webform to retrieve.
   * @param string $format
   *   Optional, return in json or xml format.
   *
   * @return result
   *   Return result in xml or json of query, or false if failure.
   */
  public function sb_api_test_retrieve($nid, $format = NULL) {
    if ($format == NULL) {
      $format = 'xml'; // or json
    }
    $url = $this->url . '/webform/' . $nid . '.' . $format;
    //http://example.com/springboard_api/webform/<node id>[.<response format>]?key=<pubkey hash>
    //&time=<timestamp>&checksum=<checksum hash>
    return $this->_sb_api_test_make_call('retrieve', $url);
  }

  /**
   * Execute a submit query against Springboard API.
   *
   * @param integer $nid
   *   The node id of the webform to retrieve.
   * @param array $query
   *   A key=>value array matching the webform fields to values.
   * @param string $format
   *   Optional, return in json or xml format.
   *
   * @return result
   *   Return result in xml or json of query, or false if failure.
   */
  public function sb_api_test_submit($nid, $query, $format = NULL) {
    if ($format == NULL) {
      $format = 'xml'; // or json
    }
    // Put nid on the query set.
    $query['webform_id'] = $nid;
    $query['webform_nid'] = $nid;
    $url = $this->url . '/webform/submit';//.' . $format;
    //http://example.com/springboard_api/webform/submit[.<response format>]
    return $this->_sb_api_test_make_call('submit', $url, $query, 'POST');
  }

  /**
   * Execute a query against Springboard API.
   *
   * @param string $resource
   *   The type of query.
   * @param string $url
   *   The constructed URL to execute against.
   * @param array $query
   *   Optional, a key=>value array where needed.
   * @param string $method
   *   Optional, the method to make the query with, GET by default.
   *
   * @return result
   *   Return result in xml or json of query, or false if failure.
   */
  private function _sb_api_test_make_call($resource, $url, $query = array(), $method = 'GET') {
    // Make call if we have cURL.
    if (function_exists('curl_init')) {
      $timestamp = (int) time('now');
      // Create the checksum, based on context and query data.
      $checksum = $this->_sb_api_test_make_checksum($resource, $timestamp, $this->public_key, $this->private_key);
      $curl = curl_init();
      curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
      // GET is the default curl method.
      if ($method == 'POST') {
        $query['resource'] = $resource;
        $query['time'] = $timestamp;
        $query['key'] = trim($this->public_key);
        $query['checksum'] = $checksum;
        curl_setopt($curl, CURLOPT_POST, 1);
        curl_setopt($curl, CURLOPT_POSTFIELDS, $query);
      }
      else {
        $query_string = 'key=' . trim($this->public_key) . '&time=' . $timestamp . '&checksum=' . $checksum;
        foreach ($query as $key => $value) {
          $query_string .= '&' . urlencode($key) . '=' . urlencode($value);
        }
        $url = $url . '?' . $query_string;
      }
      curl_setopt($curl, CURLOPT_URL, $url);
      $response['content'] = curl_exec($curl);
      libxml_use_internal_errors(TRUE);
      $response['object'] = simplexml_load_string($response['content']);
      $response['info'] = curl_getinfo($curl);
      if ($this->debug) {
        drupal_set_message('Debug response: '. print_r($response, 1));
      }
      curl_close($curl);
      // Check for valid response.
      if (empty($response['info']['http_code'])) {
        return FALSE; // No HTTP code returned.
      }
      if ($response['info']['http_code'] != 200) {
        return FALSE; // Not a valid response.
      }
      return $response;
    }
    return FALSE;
  }

  /**
   * Execute a query against Springboard API.
   *
   * @param string $resource
   *   The type of query.
   * @param string $timestamp
   *   The timestamp.
   * @param string $public_key
   *   The public key provided by Springboard API.
   * @param string $private_key
   *   The private key provided by Springboard API.
   *
   * @return result
   *   Return a checksum hash.
   */
  private function _sb_api_test_make_checksum($resource, $timestamp, $pubkey, $privkey) {
    // NOTE: ORDER MATTERS HERE. THIS IS IMPORTANT TO REMEMBER IF YOU BUILD YOUR OWN CHECKSUM.
    // The order here MUST be the same as the order the checksum function on Sprinabog API.
    // And the values must be simularly cleaned up to avoid mismatches.
    // Please do not change this piece of code if you reuse this object for other implementations.
    $data = array();
    $data['resource'] = trim($resource);
    $data['timestamp'] = (int) trim($timestamp);
    $data['pubkey'] = trim($pubkey);
    $data['privkey'] = trim($privkey);
    return sha1(json_encode($data));
  }
}