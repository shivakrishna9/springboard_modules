<?php
/**
 * @file springboard_advocacy_views_plugin_query.inc
 */

/**
 * Class springboard_dw_views_plugin_query
 * Override views default db query with data warehouse guzzle request.
 */
class springboard_dw_donations_views_plugin_query extends views_plugin_query {
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
  function build(&$view) {
    $view->init_pager($view);
    $this->pager->query();
  }

  /**
   * @param view $view
   */
  function execute(&$view) {
    $start = microtime(TRUE);
    $method = 'GET';
    $endpoint = 'donations';
    //  $endpoint = 'metrics/donation/count';
    //  $endpoint = 'donation/item/74251';
    //dpm($view);
    $params = array(
   //   'search' => 'jen',
    //  'createdAtStart' => '2015-01-17 0:0:0',
  //    'search' => 'Olivia',
      'page' => empty($view->query->pager->current_page) ? 0 : $view->query->pager->current_page,
      'perPage' => empty($view->query->limit) ? 10 : $view->query->limit,
    );

    // Apply any global keyword filtering:
    if (isset($view->exposed_data)) {
      $filter_values_to_skip = array('submit', 'items_per_page', 'form_build_id', 'form_token', 'form_id');
      foreach ($view->exposed_data as $filter_key => $filter_value) {
        if (empty($filter_value) || empty($filter_key) || in_array($filter_key, $filter_values_to_skip)) {
          continue;
        }
        elseif ($filter_key == 'dw_global_search') {
          if (!empty($filter_value)) {
            $params['first_name'] = $filter_value;
          }
          continue;
        }
        $params[$filter_key] = $filter_value;
      }
    }

    // Apply view sorting to the request
    if (isset($view->exposed_input['order']) && !empty($view->exposed_input['order'])) {
      $params['orderBy'] = $view->exposed_input['order'];
    }
    if (isset($view->exposed_input['sort']) && !empty($view->exposed_input['sort'])) {
      $params['order'] = $view->exposed_input['sort'];
    }

    // Submit the request to the Data Warehouse:
    $response = springboard_dw_api_call($endpoint, $params, $method);
    $response_data = (string) $response->getBody();

    $response_data = json_decode($response_data);
    $view->result = $response_data->donation->data;
    //dpm($response_data);
    //$view->result = array((object) array('count' => $result));

    // Build the pager.
    $this->pager->total_items = $response_data->donation->total;
    $this->pager->update_page_info();
    $this->pager->post_execute($view->result);
    if ($this->pager->use_pager()) {
      $view->total_rows = $response_data->donation->total;
    }

      $view->total_rows = $response_data->donation->total;



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

  function add_where() {}
}
