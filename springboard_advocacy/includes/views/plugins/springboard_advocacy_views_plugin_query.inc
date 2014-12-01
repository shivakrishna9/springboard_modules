<?php

/**
 * @file springbord_advocacy_views_plugin_query.inc
 */

class springboard_advocacy_views_plugin_query extends views_plugin_query {

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
    if (isset($view->query->search_terms)) {
      dpm($view->query->search_terms);
      $view->build_info['springboard_advocacy_search'] = $view->query->search_terms;  
      foreach ($view->query->search_terms as $key => $value) {
       // $view->build_info['springboard_advocacy_request']->setSearchTerms($key, $value);   
      } 
    } 
  }

  function execute(&$view) {

    $client = $view->build_info['springboard_advocacy_request'];

    $start = microtime(true);
    
    $targets = $client->getCustomTargets($view->query->search_terms);

    if ($this->pager->use_pager()) {
      $this->pager->total_items = count($targets);
      if (!empty($this->pager->options['offset'])) {
        $this->pager->total_items -= $this->pager->options['offset'];
      }
      $this->pager->update_page_info();
    }
   
    if (!empty($this->limit) || !empty($this->offset)) {
      $limit  = intval(!empty($this->limit) ? $this->limit : 999999);
      $offset = intval(!empty($this->offset) ? $this->offset : 0);
      $targets = array_slice($targets, $offset, $limit, TRUE);
    }
 
    $view->result = array();
    $view->result = $targets;

    $this->pager->post_execute($view->result);
    
    if ($this->pager->use_pager()) {
      $view->total_rows = $this->pager->get_total_items();
    }

    $view->execute_time = microtime(true) - $start;
  }
}
