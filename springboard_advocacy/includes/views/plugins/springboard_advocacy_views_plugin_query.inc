<?php

/**
 * @file springbord_advocacy_views_plugin_query.inc
 *
 * Override views default db query with advocacy REST API query
 */

class springboard_advocacy_views_plugin_query extends views_plugin_query {

  /**
   * Advocay API client object
   *
   * @var client
   */
  protected $client;

  function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);
    $loader = new AdvocacyApiClientLoader();
    $this->client = $loader->getClient();
  }

  function build(&$view) {
    $view->init_pager($view);
    $this->pager->query();
    $view->build_info['springboard_advocacy_request'] = $this->client;
    $view->build_info['target_ids'] = array();
    // helpful for the "add all link"
     if (isset($view->query->search_terms)) {
       $view->build_info['springboard_advocacy_search'] = $view->query->search_terms;
     }
  }

  function execute(&$view) {
    $client = $view->build_info['springboard_advocacy_request'];

    $start = microtime(true);

    // should we pull the terms from $view->query? Why use $view->build_info?
    $fields = !empty($view->query->search_terms) ? $view->query->search_terms : NULL;

    // If a non-exposed class_name search is present and it is the only search
    // suppress results because no search has actually been performed yet
    // Will need to further limit this by view block name so the main admin screens have results by default.
    if ((count($fields) == 1) && isset($fields['class_name'])) {
      //$suppress_results = $view->filter['search_class_name']->options['exposed'] == FALSE;
    }


    if (!empty($fields) && empty($suppress_results)) {

      foreach ($fields as $field_name => $search_terms) {
        // is it a multiple value checkbox or select?
        // then format in a way the api expects
        if (is_array($search_terms)) {

          $fields[$field_name] = implode('|', $search_terms);
        }
        else {

          //strip leading and multiple whitespaces
          $search_terms = trim($search_terms);
          $search_terms = preg_replace('/\s+/', ' ', $search_terms);
          //no committees in the main search tab text input
          if(strpos($search_terms, 'committee:') === FALSE && $field_name != 'search_committee') {
            $fields[$field_name] = str_replace(' ', '|', $search_terms);
          }
          elseif( $field_name != 'search_committee') {
            //has committees in the main search tab text input
            $pattern = '/(committee:".*?")/';
            preg_match_all($pattern, $search_terms, $matches);

            if(!empty($matches[1])) {
              foreach($matches[1] as $match) {
                $fields[$field_name] = trim(str_replace($match, '', $fields[$field_name]));
              }

              $non_matches = !empty($fields[$field_name]) ? $fields[$field_name] . '|' : NULL;
              if(!empty($non_matches)) {
                $non_matches = preg_replace('/\s+/', ' ', $non_matches);
                $non_matches = str_replace(' ', '|', $non_matches);
              }

              $committees = implode('|', $matches[1]);

              $fields[$field_name] =  $non_matches . $committees;
            }
          }
          //end main tab text input committee parsing

        }
      }

      $fields['limit'] = $this->pager->options['items_per_page'];
      $fields['offset'] = $this->offset;

      if (isset($this->orderby)) {
        $fields['sort'] = $this->orderby[0]['field'];
        $fields['direction'] = $this->orderby[0]['direction'];
      }

      // Get the targets
      $targets = $client->searchTargets($fields);

      if(!isset($targets->data)) {
        if (isset($targets->error)) {
          drupal_set_message(t('An error occurred when attempting to search: @message', array('@message' => $targets->error->message)), 'error');
          watchdog('advocacy', 'An error occurred when attempting to search: @message', array('@message' => $targets->error->message), WATCHDOG_ERROR);

        } else {
          drupal_set_message(t('A response was not received from the server when attempting to search for targets'), 'error');
          watchdog('advocacy', 'A response was not received when attempting to search for targets.', array(), WATCHDOG_ERROR);

        }
      }      // Build the pager
      if (!empty($targets->data->targets))  {
        if ($this->pager->use_pager()) {
          $this->pager->total_items = $targets->data->count;
          if (!empty($this->pager->options['offset'])) {
            $this->pager->total_items -= $this->pager->options['offset'];
          }
          $this->pager->update_page_info();
        }
        $view->build_info['target_ids'] = $targets->data->ids;
      }
      else {
        // No results
        if (isset($targets->error)) {
          watchdog('advocacy', 'An error occurred while searching for targets: @message', array('@message' => $targets->error->message), WATCHDOG_ERROR);
        }
      }

      $view->result = array();
      if(!empty($targets->data->targets)) {
        $view->result = $targets->data->targets;
      }

      $this->pager->post_execute($view->result);
      
      if ($this->pager->use_pager()) {
        $view->total_rows = $this->pager->get_total_items();
      }

      $view->execute_time = microtime(true) - $start;
    }
  }

  function add_orderby($table, $field = NULL, $order = 'ASC', $alias = '', $params = array()) {
    $this->orderby[] = array(
      'field' => $alias,
      'direction' => strtoupper($order)
    );
  }

}