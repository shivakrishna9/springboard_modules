<?php

/**
 * @file
 * API functions for Springboard data warehouse.
 */

/**
 * Returns a new instance of Guzzle Http client.
 * @return \GuzzleHttp\Client
 */
function springboard_dw_api_client()
{
  // Composer Vendor autoload
  if(function_exists('drush_autoload')) {
    drush_autoload("sites/all/libraries/springboard_composer/vendor/autoload.php");
  }
  else {
    require_once ("sites/all/libraries/springboard_composer/vendor/autoload.php");
  }

  // Load Guzzle client
  $guzzle = new GuzzleHttp\Client(['http_errors' => false, 'verify' => false]);

  // Return an instance of the client
  return $guzzle;
}

/**
 * Returns the endpoint used to make API calls to the data warehouse receiver app.
 * @param $endpoint_route
 *  The endpoint route to use in constructing the full URL for the api call.
 * @return string
 */
function springboard_dw_api_endpoint($endpoint_route)
{
  $api_endpoint = variable_get('springboard_dw_api_endpoint', NULL);

  // TODO: point this to a gospringboard.io subdomain
  return $api_endpoint .'/'. ltrim($endpoint_route, '/');
}

/**
 * Removes the queue type used to distinguish api calls and adds authentication token and client ID to the item params.
 * @param $item
 *  An array of values.
 * @return $item
 *  An array of values with authentication and client identification parameters added.
 */
function springboard_dw_api_prep_call($item)
{
  // Remove the queue type from the item
  unset($item['queue_type']);

  // Add the authentication token and client id
  $item['authenticationToken'] = variable_get('springboard_dw_authentication_token', NULL);

  return $item;
}

/**
 * Make an api call to the warehouse by supplying an endpoint route and the form params.
 * @param        $endpoint_route
 *  The endpoint route used to construct the full URI for the API call.
 * @param        $params
 *  The parameters used in the API call added to the URI for debugging purposes.
 * @param string $method
 *  The HTTP verb/method to use for the API call (defaults to POST).
 * @return mixed|\Psr\Http\Message\ResponseInterface
 */
function springboard_dw_api_call($endpoint_route, $params, $method = 'POST')
{
  // Define the endpoint
  $endpoint = springboard_dw_api_endpoint($endpoint_route);

  // Instantiate an API client instance
  $api_client = springboard_dw_api_client();

  // Clean up the item and add authentication to the form params
  $params = springboard_dw_api_prep_call($params);

  try {

    drush_print_r($params);

    // Attempt Guzzle client request
    $api_response = $api_client->request($method, $endpoint, array(
      'form_params' => $params
    ));

    return $api_response;
  }
  catch (GuzzleHttp\Exception\ClientException $e) {
    $response = $e->getResponse();
    $responseBodyAsString = $response->getBody()->getContents();

    watchdog("data_warehouse", '<pre>' . print_r($e, true) . '</pre>');
  }
}

/**
 * Mark an item in Drupal as being warehoused.
 * @param $type
 *  The type or identifier for what kind of item we are tracking.
 * @param $identifier
 *  The unique identifier for the item (typically a Drupal ID unless it is a message item in which case it is a MongoId string)
 * @throws \Exception
 */
function springboard_dw_track_item_warehouse_success($type, $identifier)
{
  // Insert the user account into the Drupal data warehouse table
  db_insert('springboard_data_warehouse')->fields(array(
    'type'      => $type,
    'drupal_id' => $identifier,
    'created'   => time()
  ))->execute();
}

/**
 * API utility method for logging API call errors thrown in the queue.
 * @param        $error
 *  The error to log with Drush.
 * @param        $endpoint
 *  The endpoint for use in constructing a debug URI to log with Drush.
 * @param        $item
 *  The parameters used in the API call added to the URI for debugging purposes.
 * @param        $response_body
 *  The response body generated from the API call with the Guzzle Http client.
 * @param string $log_type
 */
function springboard_dw_api_log_error($error, $endpoint, $item, $response_body, $log_type = 'warning')
{
  // Log the API error
  drush_log($error, $log_type);
  drush_print($endpoint . '?' . http_build_query($item));
  drush_print_r($response_body);
}