<?php
/**
 * @file springboard_dw_advocacy_stats_views_plugin_query.inc
 */

/**
 * Class springboard_dw_advocacy_stats_views_views_plugin_query
 * Override views default db query with data warehouse guzzle request.
 */
class springboard_dw_advocacy_stats_views_plugin_query extends views_plugin_query {
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
  function execute(&$view) {
    if (!isset($view->args[0]) || !is_numeric($view->args[0])) {
      $view->result = array();
      return;    
    } 

    $start = microtime(TRUE);
    $method = 'GET';
    $target_action_id = 'Advocacy-D-Test-Messa-56ec40b05e722';
    $params = array('actionId' => $target_action_id);
    $view->result = array();

    // Fetch the statistics
    $endpoint = 'metrics/action/count';
    $response = springboard_dw_api_call($endpoint, $params, $method);
    $response_data = json_decode((string) $response->getBody(), TRUE);
    if (!empty($response_data)) {
      foreach ($response_data as $response_key => $value) {
        if (!is_array($value)) {
          continue;
        }
        // Break up first/latest donation data into individual fields:
        if (isset($value['action_id']) && isset($value['created_at'])) {
          if ($response_key == 'first_submission') {
            $response_data['first_action_id'] = $value['action_id'];
            $response_data['first_action_date'] = $value['created_at'];
          }
          elseif ($response_key == 'latest_submission') {
            $response_data['latest_action_id'] = $value['action_id'];
            $response_data['latest_action_date'] = $value['created_at'];
          }
          unset($response_data[$response_key]); 
        } 
      }
      $response_data['latest_fetch'] = time();
      // Populate the status based on node ID:
     /* $node_status = db_query("SELECT status FROM {node} WHERE nid = :nid", array(':nid' => $target_donation_id))
        ->fetchField();
      if ($node_status){
        $response_data['node_status'] = $node_status == 0 ? t('Not Published') : t('Published');
      }
      else {
        $response_data['node_status'] = t('N/A');
      } */
      $view->result[] = $response_data;
    }

    // Add currency symbol to amount raised:
    /* if (isset($view->result[0]['raised'])) {
      $default_currency_code = variable_get('commerce_default_currency', 'USD');
      $view->result[0]['raised'] = commerce_currency_format($view->result[0]['raised'], $default_currency_code, NULL, FALSE);
    } */
    
    $view->execute_time = microtime(TRUE) - $start;
  }

  function add_orderby() {}
  function build(&$view) {}
  function add_field() {}
  function add_where() {}
  function add_where_expression() {}
  function ensure_table() {}
  function placeholder() {}
}
