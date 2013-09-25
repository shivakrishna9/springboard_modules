<?php

/**
 * @File db helper functions
 */

/**
 * Load share event
 */
function sb_social_share_event_load($share_id) {
  $result = db_query('
    SELECT
      share_id,
      uid,
      sid,
      nid,
      service,
      created,
      status,
      url
    FROM {sb_social_shares}
    WHERE
      share_id = :share_id
  ', array(':share_id' => $share_id));

  return $result->fetchAssoc();
}

/**
 * Save share event
 */
function sb_social_share_event_save($service, $sid) {
  global $user;
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  $nid = _sb_social_get_nid_by_sid($sid);
  $submission = webform_get_submission($nid, $sid);
  $settings = sb_social_get_current_settings();
  $uid = !empty($submission->uid) ? $submission->uid : $user->uid;

  $url = sb_social_share_url($nid, $uid, $sid, $service, $settings);

  // update is not included as these records should only
  // be created when a share event has taken place
  // and should not be editable after the fact.
  $query = db_insert('sb_social_shares');
  $record = array(
    'uid' => $uid,
    'sid' => $sid,
    'nid' => $nid,
    'service' => $service,
    'created' => (int) time(),
    'status' => 'pending',
    'url' => $url,
  );
  $share_id = $query->fields($record)->execute();
  watchdog('sb_social', 'share id:' . $share_id);
  if ($share_id && module_exists('salesforce_queue')) {
    watchdog('sb_social', 'attempted to add share event to queue');
    sb_social_add_share_to_queue($share_id);
  }

}

function sb_social_get_current_settings() {
  if (arg(0) == 'node' && is_numeric(arg(1))) {
    $node = node_load(arg(1));
    $id = is_object($node) ? $node->nid : FALSE;
  }
  else {
    // social supports both internal paths and path aliases so
    // we need to check both.
    $path = current_path();
    $alias = drupal_get_path_alias($path);
    $id = !empty($alias) ? array($path, $alias) : $path;
  }

  if ($id) {
    $sid = _sb_social_sid_by_id($id);
    $settings = sb_social_settings_load($sid);
  }
  return $settings;
}

/**
 * Delete share event
 */
function sb_social_share_event_delete($sid) {
  db_query('DELETE FROM {sb_social_shares} WHERE sid = :sid', array(':sid' => $sid));
}

/**
 * Load shared content settings
 */
function sb_social_settings_load($sid) {
  // This can get called by hook_block_view() in addition to hook_init() so we should cache.
  static $settings = array();

  if (empty($settings[$sid])) {
    $results = db_query('SELECT * FROM {sb_social_settings} WHERE sid = :sid', array(':sid' => $sid));

    $settings[$sid] = $results->fetchAssoc();
    if ($settings[$sid] && !empty($settings[$sid]['data'])) {
      $settings[$sid]['data'] = unserialize($settings[$sid]['data']);
    }

    // supply global defaults if no settings are available.
    if (empty($settings[$sid])) {
      // check for node view page.
      $node = menu_get_object();
      // confirmation page
      if (!$node && arg(2) == 'done') {
        $node = menu_get_object('webform_menu');
      }
      if (isset($node->nid)) {
        $settings[$sid]['id'] = $node->nid;
        $settings[$sid]['type'] = 'node';
      }
      else {
        // if we still don't have settings, use the current path.
        $settings[$sid]['id'] = current_path();
        $settings[$sid]['type'] = 'path';
      }

      $enabled_services = sb_social_get_enabled_services();
      foreach ($enabled_services as $service) {
        $settings[$sid]['data'][$service] = _sb_social_settings_defaults($service);
      }
      if (module_exists('market_source')) {
        $settings[$sid]['data']['market_source'] = _sb_social_market_source_defaults();
      }
    }
  }
  return $settings[$sid];
}


function sb_social_get_id_by_sid($sid) {
  $result = db_query('SELECT id FROM {sb_social_settings} WHERE sid = :sid', array(':sid' => $sid));
  return $result->fetchField();
}

/**
 * Load all path settings, used by path admin list page.
 */
function sb_social_path_settings_load_all() {
  $settings = array();
  $results = db_query('SELECT id as path, sid FROM {sb_social_settings} WHERE type = :type', array(':type' => 'path'));
  while ($item = $results->fetchAssoc()) {
    $settings[] = $item;
  }
  return $settings;
}

// TODO: replace with plugin defaults.
/**
 * provide service default settings
 */
function _sb_social_settings_defaults($service) {
  $settings = array();
  ctools_include('plugins');
  $plugins = ctools_get_plugins('sb_social', 'social_networks');
  $services = array_keys($plugins);

  if (in_array($service, $services)) {
    $func = $plugins[$service]['defaults'];
    $settings = $func();
  }

  return $settings;
}

/**
 * Provide default MS settings when no node settings are available.
 */
function _sb_social_market_source_defaults() {
  $market_source = array();
  $fields = market_source_field_info();
  $blacklist = _sb_social_component_blacklist();
  foreach ($fields as $key => $field) {
    if (in_array($key, $blacklist)) {
        continue;
    }
    $market_source[$key] = variable_get('springboard_social_ms_' . $key, '');
  }
  return $market_source;
}

function sb_social_get_enabled_services() {
  static $enabled_services = array();
  if (empty($enabled_services) && $enabled_services !== FALSE) {
    $services = variable_get('springboard_social_services', array());
    if (!empty($services)) {
      foreach ($services as $service => $status) {
        if ($status) {
          $enabled_services[] = $service;
        }
      }
    }
  }
  return $enabled_services;
}


/**
 * Save path settings.
 * @param type $settings
 * @param type $values
 */
function sb_social_settings_save($settings, $values = NULL) {
  if (is_array($settings['data'])) {
    $settings['data'] = serialize($settings['data']);
  }
  $update = array();
  $sid = _sb_social_sid_by_id($settings['id']);

  if ($sid) {
    $settings['sid'] = $sid;
    $update = array('sid');
  }

  drupal_write_record('sb_social_settings', $settings, $update);
}


function _sb_social_path_record_exists($path) {
  return db_query('SELECT 1 FROM {sb_social_path} WHERE path = :path', array(':path' => $path))->fetchField();
}


function sb_social_settings_delete($sid) {
  $query = db_delete('sb_social_settings');
  $query->condition('sid', $sid);
  $query->execute();
}

/**
 * Lookup webform node settings by webform submission id.
 * @param type $sid
 * @return type
 */
function _sb_social_get_settings_by_sid($sid) {
  $nid = _sb_social_get_nid_by_sid($sid);
  return sb_social_settings_load($nid);
}

/**
 * Lookup webform node id from webform submission ID.
 *
 * @staticvar array $data
 * @param type $sid
 * @return type
 */
function _sb_social_get_nid_by_sid($sid) {
  static $data = array();
  if (empty($data[$sid])) {
    $result = db_query('SELECT nid FROM {webform_submissions} WHERE sid = :sid', array(':sid' => $sid));
    $nid = $result->fetchField();
    $data[$sid] = $nid ? $nid : 0;
  }
  return $data[$sid];
}

function _sb_social_load_submission($sid) {
  $result = db_query('
    SELECT
      nid,
      sid,
      uid
    FROM {webform_submissions}
    WHERE
      sid = :sid
  ', array(':sid' => $sid));
  return $result->fetchAssoc();
}

function sb_social_get_donation_by_sid($sid) {
  $query = db_select('fundraiser_donation', 'f');
  $query->fields('f');
  $query->condition('sid', $sid);
  $result = $query->execute();

  return $result->fetchObject();
}

function _sb_social_sid_by_id($id) {
  $query = db_select('sb_social_settings', 's');
  $query->fields('s', array('sid'));

  if (is_array($id)) {
    $or = db_or();
    foreach ($id as $data) {
      $or->condition('id', $data, '=');
    }
    $query->condition($or);
  }
  else {
    $query->condition('id', $id, '=');
  }
  $results = $query->execute();

  return $results->fetchField();
}
