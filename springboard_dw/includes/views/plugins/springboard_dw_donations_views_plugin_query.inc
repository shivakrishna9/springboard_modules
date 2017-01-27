<?php
/**
 * @file springboard_dw_donations_views_plugin_query.inc
 */

/**
 * Class springboard_dw_donations_views_plugin_query
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
    $params = array();

    // Disable data fetch during view edit preview (drupal_get_path() returns other during view preview):
    $current_path = isset($_SERVER['REQUEST_URI']) ? $_SERVER['REQUEST_URI'] : '';
    if (strpos($current_path, 'admin/structure/views') !== FALSE) {
      $view->result = array();
      drupal_set_message('View output disabled during preview.');
      return;
    } 

    // Determine endpoint based on whether there's a donation_id contextual filter:
    if (isset($view->args[0]) && is_numeric($view->args[0])) {
      $request_type = 'single_donation';
      $endpoint = 'donation/item/' . $view->args[0];
    }
    else {
      $request_type = 'multi_donation';
      $endpoint = 'donations';
    }

    if ($request_type == 'multi_donation' && isset($view->exposed_data)) {

      // Add filters to the request:
      $filter_values_to_skip = array('submit', 'form_build_id', 'form_token', 'form_id', 'reset');
      foreach ($view->exposed_data as $filter_key => $filter_value) {
        if (empty($filter_value) || empty($filter_key) || in_array($filter_key, $filter_values_to_skip)) {
          continue;
        }
        if (is_string($filter_value)) {
          $filter_value = trim($filter_value);
        }
        // Set filters expected by the data warehouse:
        switch ($filter_key) {
          case 'form_name':
            $filter_key = 'formLike';
            break;
          case 'items_per_page':
            $filter_key = 'perPage';
            break;
          case 'email':
            $filter_key = 'emailLike';
            break;
          case 'first_name':
            $filter_key = 'firstNameLike';
            break;
          case 'last_name':
            $filter_key = 'lastNameLike';
            break;
          case 'gateway':
            $filter_key = 'gatewayLike';
            break;
        }

        // Alter gateway filter to replace submitted human-readable filter with machine name to match d/w data:
        if ($filter_key == 'gatewayLike') {
          unset($gateway_match);
          $gateways = commerce_payment_methods();

          // Look for exact matches:
          foreach ($gateways as $gateway_machine => $gateway) {
            if (isset($gateway['title']) && strtolower($filter_value) == strtolower($gateway['title'])) {
              $gateway_match = $gateway_machine;
              break;
            }
            elseif (isset($gateway['short_title']) && strtolower($filter_value) == strtolower($gateway['short_title'])) {
              $gateway_match = $gateway_machine;
              break;
            }
          }

          if (isset($gateway_match)) {
            $filter_value = $gateway_match;
          }
          // Try submitting the value, as is, with spaces and dashes replaced with % for a wildcard match:
          elseif (!isset($gateway_match)) {
            $filter_value = str_replace(array(' ', '-', '_'), '%', $filter_value);
            $filter_value = str_replace('%%', '%', $filter_value);
          }
        }

        $params[$filter_key] = $filter_value;
      }

      // Re-format date range filtering:
      if (isset($params['transaction_date']['min'])) {
        $params['transactionDateRangeStart'] = date('m/d/Y', strtotime($params['transaction_date']['min']));
        unset($params['transaction_date']['min']);
      }
      if (isset($params['transaction_date']['max'])) {
        $params['transactionDateRangeEnd'] = date('m/d/Y', strtotime($params['transaction_date']['max']));
        unset($params['transaction_date']['max']);
      }
      unset($params['transaction_date']);

      // Apply view click sorting to the request:
      if (isset($view->exposed_input['order']) && !empty($view->exposed_input['order'])) {
        $params['orderBy'] = $view->exposed_input['order'];
      }
      if (isset($view->exposed_input['sort']) && !empty($view->exposed_input['sort'])) {
        $params['order'] = $view->exposed_input['sort'];
      }
      // If click sorting is not being used then use the view's default sort order, if available:
      if (!isset($params['orderBy']) && !isset($params['order']) && isset($view->sort) && is_array($view->sort)) {
        $sort = array_values($view->sort)[0];
        if (isset($sort->field)) {
          $params['orderBy'] = $sort->field;
        }
        if (isset($sort->opions['order'])) {
          $params['order'] = strtolower($sort->opions['order']);
        }
      }

      // Set the current page:
      if (isset($_GET['page']) && is_numeric($_GET['page'])) {
        $params['page'] = $_GET['page'];
      }
    }

    // Submit the request to the Data Warehouse and get the response:
    $response = springboard_dw_api_call($endpoint, $params, $method);
    if (is_object($response) && method_exists($response, 'getBody')) {
      $response_data = json_decode((string) $response->getBody());
    }

    // Handle results from the donations endpoint:
    if ($request_type == 'multi_donation' && isset($response_data->donation->data)) {
      $view->result = $response_data->donation->data;
      // Build the pager.
      $this->pager->total_items = $response_data->donation->total;
      $this->pager->update_page_info();
      $this->pager->post_execute($view->result);
      if ($this->pager->use_pager()) {
         $view->total_rows = $response_data->donation->total;
      }

      $view->total_rows = $response_data->donation->total;
    }
    elseif ($request_type == 'single_donation' && isset($response_data->donation)) {
      $view->result = array($response_data->donation);
    }
    else {
      $view->result = array();
    }

    // Alter result data:
    foreach ($view->result as $key => $result) {
      // Populate the payment link with the transaction_id value, so it can become a link:
      if (isset($view->result[$key]->transaction_id) && is_numeric($view->result[$key]->transaction_id)) {
        $view->result[$key]->payment_link = $view->result[$key]->transaction_id;
      }

      // Prefix the donation amount with the appropriate currency symbol:
      if (isset($view->result[$key]->amount) && is_numeric($view->result[$key]->amount) && isset($view->result[$key]->currency)) {
        $view->result[$key]->amount = commerce_currency_format($view->result[$key]->amount, $view->result[$key]->currency, NULL, FALSE);
      }
    }

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

  function add_field() {}

  function add_where() {}
  
  function ensure_table() {}
}
