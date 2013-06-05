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
function sb_social_webform_settings_load($nid) {
  static $settings = array();

  if (empty($settings[$nid])) {
    $settings[$nid] = array();
    $results = db_query('
      SELECT
        nid,
        service,
        data
      FROM {sb_social_webform}
      WHERE
        nid = :nid
    ', array(':nid' => $nid));

    while ($result = $results->fetchAssoc()) {
      $settings[$nid][$result['service']] = unserialize($result['data']);
    }
  }
  return $settings[$nid];
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
function sb_social_webform_settings_save($settings, $update = FALSE) {
  $update = $update ? 'nid' : array();
  if (is_array($settings['data'])) {
    $settings['data'] = serialize($settings['data']);
  }
  drupal_write_record('sb_social_webform', $settings, $update);
}


/**
 * Delete shared content settings
 */
function sb_social_webform_settings_delete($nid, $service = FALSE) {
  // TODO: reimplement with db_delete() to optionally remove single records.
  db_query('DELETE FROM {sb_social_webform} WHERE nid = :nid', array(':nid' => $nid));
}

function _sb_social_get_settings_by_sid($sid) {
  $nid = _sb_social_get_nid_by_sid($sid);
  return sb_social_webform_settings_load($nid);
}

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