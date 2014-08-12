<?php
/**
 * @file
 * fundraiser_entity.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function fundraiser_entity_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
