<?php

/**
 * @file
 * Smarty Streets functions for Springboard M2T.
 */

/**
 * Ajax callback for smarty streets api
 */
function _springboard_target_api_test() {

  return views_embed_view('targets', 'page_1');
  // $geo['zip'] = '12866';
  // $geo['plus4'] = '3300';
  // springboard_m2t_get_legislators($geo['zip'], $geo['plus4']);
  // exit();
}