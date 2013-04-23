
CONTENTS OF THIS FILE
---------------------

 * Springboard API Object
 * Requirements
 * Functions

SPRINGBOARD API OBJECT
----------------------

Springboard API class: Contained in the include folder of this module is a class implementation against Springboard API. It can be copied and used in custom PHP systems, using the module as a demonstration of use. It is designed to be modular and does not make use of any Drupal-standard functions.

REQUIREMENTS
------------

To use this class, cURL must be available on your server.

FUNCTIONS
---------

The following functions are available for this class.

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
  $object = new SpringboardAPIObject($url, $public_key, $private_key, $debug);

  /**
   * Execute a base query against Springboard API.
   *
   * @param string $format
   *   Optional, return in json or xml format.
   *
   * @return result
   *   Return true if successful connection or false if failure.
   */
  $response = $object->sb_api_test_base($format);

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
  $response = $object->sb_api_test_index($query, $format);

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
  $response = $object->sb_api_test_retrieve($nid, $format);

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
  $response = $object->sb_api_test_submit($nid, $query, $format);

A response object includes the following infomation:

$response['content'] = The content of the retrieved Springboard API result.
$response['object'] = The object result of simplexml_load_string(), if format is XML and content is valid XML.
$response['info] = Further query context, includeing http_code.
  