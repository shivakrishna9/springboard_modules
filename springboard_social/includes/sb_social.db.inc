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
  module_load_include('inc', 'webform', 'includes/webform.submissions');
  $nid = _sb_social_get_nid_by_sid($sid);
  $submission = webform_get_submission($nid, $sid);
  $url = sb_social_share_url($nid, $submission->uid, $sid);
  // update is not included as these records should only
  // be created when a share event has taken place
  // and should not be editable after the fact.
  $query = db_insert('sb_social_shares');
  $record = array(
    'uid' => $submission->uid,
    'sid' => $submission->sid,
    'nid' => $submission->nid,
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

/**
 * Delete share event
 */
function sb_social_share_event_delete($sid) {
  db_query('DELETE FROM {sb_social_shares} WHERE sid = :sid', array(':sid' => $sid));
}

/**
 * Load shared content settings
 */
function sb_social_settings_load($nid) {
  static $settings = array();

  if (empty($settings[$nid])) {
    $settings[$nid] = array();
    $query = db_select('sb_social_node', 's');
    $query->fields('s');
    $query->condition('nid', $nid, '=');
    $results = $query->execute();

    while ($result = $results->fetchAssoc()) {
      $settings[$nid][$result['service']] = unserialize($result['data']);
    }
  }
  // supply global defaults if no settings are available.
  if (empty($settings[$nid])) {
    $enabled_services = sb_social_get_enabled_services();
    foreach ($enabled_services as $service) {
      $settings[$nid][$service] = _sb_social_settings_defaults($service);
    }
  }
  return $settings[$nid];
}

function sb_social_path_settings_load($pid) {
  $path = sb_social_get_path_by_pid($pid);

  $query = db_select('sb_social_path', 's');
  $query->fields('s');
  $query->condition('path', $path, '=');
  $results = $query->execute();

  while ($result = $results->fetchAssoc()) {
    $settings[$path][$result['service']] = unserialize($result['data']);
  }

  if (empty($settings[$path])) {
    $enabled_services = sb_social_get_enabled_services();
    foreach ($enabled_services as $service) {
      $settings[$path][$service] = _sb_social_settings_defaults($service);
    }
  }
  return $settings[$path];
}

function sb_social_get_path_by_pid($pid) {
  $result = db_query('SELECT path FROM {sb_social_path} WHERE pid = :pid', array(':pid' => $pid));
  return $result->fetchField();
}

function sb_social_path_settings_load_all() {
  $settings = array();
  $results = db_query('SELECT path, pid FROM {sb_social_path} GROUP BY path');
  while ($item = $results->fetchAssoc()) {
    $settings[] = $item;
  }
  return $settings;
}

/**
 * provide service default settings
 */
function _sb_social_settings_defaults($service) {
  switch ($service) {
    case 'facebook':
      $settings = array(
        'title' => variable_get('springboard_social_facebook_title', '%title'),
        'description' => variable_get('springboard_social_facebook_description', '%teaser'),
        'image' => '', variable_get('springboard_social_facebook_image', ''),
      );
      break;
    case 'twitter':
      $settings = array(
        'message' => variable_get('springboard_social_twitter_message', '%title'),
      );
      break;
    case 'email':
      $settings = array(
        'subject' => variable_get('springboard_social_email_subject', '%title'),
        'message' => variable_get('springboard_social_email_message'. ''),
      );
      break;
    default:
      break;
  }
  if (module_exists('market_source')) {
    $settings['market_source'] = _sb_social_market_source_defaults();
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
 * Save shared content settings
 *
 * @param $settings
 * Associative array with the following required keys:
 * nid - node id
 * service - service name (ex. twitter, facebook)
 * data - array of shared content settings
 */
function sb_social_node_settings_save($settings, $values = NULL) {

  if (module_exists('market_source') && $values) {
    $fields = market_source_field_info();
    foreach ($fields as $key => $field) {
      if (isset($values[$key])) {
        $settings['data']['market_source'][$key] = $values[$key];
      }
    }
  }
  if (is_array($settings['data'])) {
    $settings['data'] = serialize($settings['data']);
  }
  dsm($settings, 'save settings');
  sb_social_node_settings_delete($settings['nid'], $settings['service']);
  drupal_write_record('sb_social_node', $settings);
}

/**
 * Save path settings.
 * @param type $settings
 * @param type $values
 */
function sb_social_path_settings_save($settings, $values = NULL) {
  if (module_exists('market_source') && $values) {
    $fields = market_source_field_info();
    foreach ($fields as $key => $field) {
      if (isset($values[$key])) {
        $settings['data']['market_source'][$key] = $values[$key];
      }
    }
  }
  if (is_array($settings['data'])) {
    $settings['data'] = serialize($settings['data']);
  }
  $update = array();
  $pid = _sb_social_pid_by_path_service($settings['path'], $settings['service']);

  if ($pid) {
    $settings['pid'] = $pid;
    $update = array('pid', 'service');
  }
  $record = (object) $settings;
  $ret = drupal_write_record('sb_social_path', $settings, $update); //, $update);
}


function _sb_social_path_record_exists($path) {
  return db_query('SELECT 1 FROM {sb_social_path} WHERE path = :path', array(':path' => $path))->fetchField();
}

/**
 * Delete shared content settings
 */
function sb_social_node_settings_delete($nid, $service = FALSE) {
  // TODO: reimplement with db_delete() to optionally remove single records.
  $query = db_delete('sb_social_node');
  $query->condition('nid', $nid);
  if ($service) {
    $query->condition('service', $service);
  }
  $query->execute();
 // db_query('DELETE FROM {sb_social_webform} WHERE nid = :nid', array(':nid' => $nid));
}

function sb_social_path_settings_delete($path, $service = FALSE) {
  $query = db_delete('sb_social_path');
  $query->condition('path', $path);
  if ($service) {
    $query->condition('service', $service);
  }
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
    $data[$sid] = $result->fetchField();
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

function _sb_social_pid_by_path_service($path, $service) {
  $result = db_query("SELECT pid FROM {sb_social_path} WHERE path = :path AND service = :service", array(':path' => $path, ':service' => $service));
  return $result->fetchField();
}
