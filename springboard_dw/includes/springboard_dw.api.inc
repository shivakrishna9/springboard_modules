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
  drush_autoload("sites/all/vendor/autoload.php");

  // Load Guzzle client
  $guzzle = new GuzzleHttp\Client(['http_errors' => false]);

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
  // TODO: point this to a gospringboard.io subdomain
  return 'http://159.203.115.210/' . ltrim($endpoint_route, '/');
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
  $item['authenticationToken'] = variable_get('authentication_token', NULL);
  $item['sbClientId'] = variable_get('client_id', NULL);

  return $item;
}

/**
 * Make an api call to the warehouse by supplying an endpoint route and the form params.
 * @param $endpoint_route
 * @param $params
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

  // Attempt Guzzle client request
  $api_response = $api_client->request($method, $endpoint, array(
    'form_params' => $params
  ));

  return $api_response;
}

/**
 * Mark an item in Drupal as being warehoused.
 * @param $type
 * @param $identifier
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