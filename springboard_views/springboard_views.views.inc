<?php

/**
 * Implements hook_views_handlers().
 */
function springboard_views_views_handlers() {
  $handlers = _springboard_views_invoke_all('views_handlers');
  return $handlers;
}

/**
 * Implementation of hook_views_data().
 */
function springboard_views_data() {
  $data = _springboard_views_invoke_all('views_data');
  return $data;
}

/**
 * Scan the modules directory for enabled Springboard modules.
 *
 * @return array
 *   Enabled Springboard modules that have views integration in this module.
 */
function _springboard_views_load_includes() {
  static $modules_enabled = array();

  if (empty($modules_enabled)) {
    $path = drupal_get_path('module', 'springboard_views') . '/modules';
    $modules_list = file_scan_directory($path, '\.views.inc');
    if (!empty($modules_list) && is_array($modules_list)) {
      foreach ($modules_list as $path => $file) {
        if (!empty($file->name) && preg_match('/(.+)\.views/', $file->name, $match) !== FALSE) {
          $module = $match[1];
          if (module_exists($module)) {
            module_load_include('inc', 'springboard_views', 'modules/' . $file->name);
            $modules_enabled[$module] = $file;
          }
        }
      }
    }
  }

  return $modules_enabled;
}

/**
 * For all enabled Springboard modules, try calling $hook. Return all results.
 */
function _springboard_views_invoke_all($hook) {
  $data = array();

  foreach (_springboard_views_load_includes() as $module => $file) {
    $function = 'springboard_views_' . $module . '_' . $hook;
    if (function_exists($function)) {
      $module_data = $function();
      if (!empty($module_data) && is_array($module_data)) {
        $data += $module_data;
      }
    }
  }

  return $data;
}
