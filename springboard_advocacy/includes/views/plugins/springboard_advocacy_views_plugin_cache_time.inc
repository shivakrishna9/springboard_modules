<?php

/**
 * @file
 * Definition of springboard_advocacy_views_plugin_cache_time.
 */

/**
 * We add an md5 hash of the serialized search term array to the cache data
 * in order to compare it to the current query and invalidate the cache if the query has
 * changed.
 */
class springboard_advocacy_views_plugin_cache_time extends views_plugin_cache_time {

  /**
   * Save data to the cache.
   *
   * We save a hash of the search terms in order to detect
   * if they change and the cache must expire
   */
  function cache_set($type) {
    switch ($type) {
      case 'query':
        // Not supported currently, but this is certainly where we'd put it.
        break;
      case 'results':
        $data = array(
          'result' => $this->view->result,
          'total_rows' => isset($this->view->total_rows) ? $this->view->total_rows : 0,
          'current_page' => $this->view->get_current_page(),
          'current_query' => md5(serialize($this->view->query->search_terms)),
        );
        cache_set($this->get_results_key(), $data, $this->table, $this->cache_set_expire($type));
        break;
      case 'output':
        $this->gather_headers();
        $this->storage['output'] = $this->view->display_handler->output;
        //$this->storage['output'] = $this->view->display_handler->output;
        cache_set($this->get_output_key(), $this->storage, $this->table, $this->cache_set_expire($type));
        break;
    }
  }


  /**
   * Retrieve data from the cache.
   *
   */
  function cache_get($type) {
    $cutoff = $this->cache_expire($type);
    switch ($type) {
      case 'query':
        // Not supported currently, but this is certainly where we'd put it.
        return FALSE;
      case 'results':
     
        if ($cache = cache_get($this->get_results_key(), $this->table)) {

          $cached_query = isset($cache->data['current_query']) ? $cache->data['current_query'] : '';
          $terms = isset($this->view->query->search_terms) ? $this->view->query->search_terms : array();
          $hashed_query = md5(serialize($terms));
          // Expire the cache if the search terms changed.
          if ($hashed_query != $cached_query) {
            return FALSE;
          }

          if (!$cutoff || $cache->created > $cutoff) {
            $this->view->result = isset($cache->data['result']) ? $cache->data['result'] : NULL;
            $this->view->total_rows = isset($cache->data['total_rows']) ? $cache->data['total_rows'] : 0;
            if (isset($cache->data['current_page'])) {
              $this->view->set_current_page($cache->data['current_page']);
            }
            $this->view->execute_time = 0;
            return TRUE;
          }
        }
        return FALSE;
      case 'output':

        if ($cache = cache_get($this->get_output_key(), $this->table)) {

          $cached_query = isset($cache->data['current_query']) ? $cache->data['current_query'] : '';
          $terms = isset($this->view->query->search_terms) ? $this->view->query->search_terms : array();
          $hashed_query = md5(serialize($terms));
          // Expire the cache if the search terms changed.
          if ($hashed_query != $cached_query) {
            return FALSE;
          }
      
          if (!$cutoff || $cache->created > $cutoff) {
            $this->storage = $cache->data;
            $this->view->display_handler->output = $cache->data['output'];
            $this->restore_headers();
            return TRUE;
          }
        }
        return FALSE;
    }
  }
}
