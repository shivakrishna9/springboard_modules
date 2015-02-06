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
    // helpful for the "add all link"
     if (isset($view->query->search_terms)) {
       $view->build_info['springboard_advocacy_search'] = $view->query->search_terms;
     }
  }

  function execute(&$view) {
    $client = $view->build_info['springboard_advocacy_request'];

    $start = microtime(true);

    // should we pull the terms from $view->query? Why use $view->build_info?
    $terms = !empty($view->query->search_terms) ? $view->query->search_terms : NULL;

    // If a non-exposed class_name search is present and it is the only search
    // suppress results because no search has actually been performed yet

    // Will need to further limit this by view block name so the main admin screens have results by default.
    if ((count($terms) == 1) && isset($terms['class_name'])) {
      //$suppress_results = $view->filter['search_class_name']->options['exposed'] == FALSE;
    }

    if (!empty($terms) && empty($suppress_results)) {
      foreach ($terms as $key => $term) {
        // is it a multiple value checkbox or select?
        // then format in a way the api expects
        if (is_array($term)) {
          $terms[$key] = implode('|', $term);
        }
      }

      $terms['limit'] = $this->pager->options['items_per_page'];
      $terms['offset'] = $this->offset;

      // Get the targets
      $targets = $client->searchTargets($terms);
      // Build the pager
      if (!empty($targets->data->targets))  {
        if ($this->pager->use_pager()) {
          $this->pager->total_items = $targets->data->count;
          if (!empty($this->pager->options['offset'])) {
            $this->pager->total_items -= $this->pager->options['offset'];
          }
          $this->pager->update_page_info();
        }
       
        // we're gonna do this on the laravel side
        // if (!empty($this->limit) || !empty($this->offset)) {
        //   $limit  = intval(!empty($this->limit) ? $this->limit : 999999);
        //   $offset = intval(!empty($this->offset) ? $this->offset : 0);
        //   $targets->data = array_slice($targets->data->targets, $offset, $limit, TRUE);
        // }

      }
      else {
        // No results
        $targets->data->targets = array();
      }

      $view->result = array();
      $view->result = $targets->data->targets;

      $this->pager->post_execute($view->result);
      
      if ($this->pager->use_pager()) {
        $view->total_rows = $this->pager->get_total_items();
      }

      $view->execute_time = microtime(true) - $start;
    }
  }
}