<?php
/**
 * @file springboard_advocacy_views_plugin_query.inc
 *
 */

/**
 * Class springboard_dw_views_plugin_query
 * Override views default db query with data warehouse guzzle request.
 */
class springboard_dw_views_plugin_query extends views_plugin_query {

  /**
   * @param $base_table
   * @param $base_field
   * @param $options
   */
  function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);
  }

  /**
   * @param view $view
   */
//  function build(&$view) {
  //  $view->init_pager($view);
 //   $this->pager->query();
//    $view->build_info['springboard_dw_request'] = $this->apiCall;
    //$view->build_info['target_ids'] = array();
    // Helpful for the "add all link".
/*    if (isset($view->query->search_terms)) {
      $view->build_info['springboard_dw_search'] = $view->query->search_terms;
    } */
 // }

  function execute(&$view) {
    $start = microtime(TRUE);
    $method = 'GET';
    $endpoint = 'metrics/donation/count';
    $endpoint = 'donation/item/74251';
 //   $params = array('formId' => '10');

    $response = springboard_dw_api_call($endpoint, $params, $method);
    $result = (string) $response->getBody();
    $result = json_decode($result);
dpm($result);
    $view->result = array((object) array('count' => $result));

    $view->execute_time = microtime(TRUE) - $start;
  }

  /**
   * @param $table
   * @param null $field
   * @param string $order
   * @param string $alias
   * @param array $params
   */
  function add_orderby(
    $table,
    $field = NULL,
    $order = 'ASC',
    $alias = '',
    $params = array()
  ) {
    $this->orderby[] = array(
      'field' => $alias,
      'direction' => strtoupper($order),
    );
  }

}
