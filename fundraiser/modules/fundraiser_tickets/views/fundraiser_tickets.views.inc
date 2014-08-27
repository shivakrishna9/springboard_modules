<?php

/**
 * Implements hook_views_query_alter
 * Prevent ticket forms from appearing in the autocomplete
 */

function fundraiser_tickets_views_query_alter(&$view, &$query) {
  if ($view->name == 'donation_forms' && $view->current_display == 'entityreference_1') {
    $types = fundraiser_tickets_get_ticket_types();
    $omit = array();
    foreach ($types as $type => $value) {
      if ($value) {
        $omit[$type] = $type;
      }
    }
    $view->query->add_where('omit', 'node.type', array_keys($omit), 'NOT IN');
  }
}
