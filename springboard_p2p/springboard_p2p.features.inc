<?php
/**
 * @file
 * springboard_p2p.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function springboard_p2p_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "page_manager" && $api == "pages_default") {
    return array("version" => "1");
  }
}
