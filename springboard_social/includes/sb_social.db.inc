<?php

/**
 * @File db helper functions
 */

/**
 * Load share event
 *
 * @param $share_id int
 * Social share event ID.
 *
 * @return array
 * Returns an associative array.
 */
function sb_social_share_event_load($share_id) {
  $result = db_query('
    SELECT
      share_id,
      uid,
      sid,
      did,
      nid,
      page_name,
      internal_name,
      market_source,
      s.type,
      service,
      created,
      status,
      url,
      sf_share_id,
      opportunity_id

    FROM {sb_social_shares} s
    WHERE
      share_id = :share_id
  ', array(':share_id' => $share_id));

  return $result->fetchAssoc();
}

/**
 * Save a social share event to the database, add the new share event to the Salesforce queue, and return
 * a share url for the event.
 *
 * @param $service
 * Social service (email, facebook, twitter)
 *
 * @param $id
 * Unique settings id (nid, webform submission id, or path)
 *
 * @param $type
 * ID type (sid, nid, path)
 *
 * @param $market_source
 * Market source (ms) field value associated with this share event.
 *
 * @return string
 * Counterintuitively enough, this function returns a shortened share url for the share event.
 */
function sb_social_share_event_save($service, $id, $type, $market_source) {

  global $user;
  // deal with submission-based shares (confirmation pages)

  if (isset($_GET['sid']) && is_numeric($_GET['sid']) && $type == 'nid') {
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    $sid = $_GET['sid'];
    $did = $sid ? _sb_social_get_did_by_sid($sid) : 0;
    $submission = webform_get_submission($id, $sid);
    $nid = isset($submission->nid) ? $submission->nid : 0;
    $node = node_load($nid);
    $uid = !empty($submission->uid) ? $submission->uid : $user->uid;
    $settings_id = sb_social_get_sid_by_id($id, 'node');
    $type = 'node';
    $page_name = !empty($node->title) ? $node->title : '';
    $internal_name_field = field_get_items('node', $node, 'field_fundraiser_internal_name');
    $internal_name = !empty($internal_name_field[0]['value']) ? $internal_name_field[0]['value'] : '';
    $url_path = 'node/' . $node->nid;
  }
  // shares from node pages
  elseif (is_numeric($id) && $type == 'nid') {
    $sid = 0;
    $nid = $id;
    $uid = $user->uid;
    $node = node_load($id);
    $page_name = !empty($node->title) ? $node->title : '';
    $internal_name = ''; // TODO: fix
    $type = $node->type;
    $url_path = 'node/' . $node->nid;
    $settings_id = $node->nid;
    $internal_name_field = field_get_items('node', $node, 'field_fundraiser_internal_name');
    $internal_name = !empty($internal_name_field[0]['value']) ? $internal_name_field[0]['value'] : '';
  }
  // shares from non-node pages (path based)
  if ($type == 'path') {
    $nid = 0;
    $sid = 0;
    $page_name = '';
    $internal_name = '';
    $type = 'page';
    $url_path = sb_social_get_path($id);
    $uid = $uid ? $uid : $user->uid;
    $settings_id = sb_social_get_sid_by_id($id, 'path');
  }

  $settings = sb_social_settings_load($settings_id);

  $uid = !empty($submission->uid) ? $submission->uid : $user->uid;

  // update is not included as these records should only
  // be created when a share event has taken place
  // and should not be editable after the fact.
  $query = db_insert('sb_social_shares');
  $record = array(
    'uid' => $uid,
    'sid' => !empty($sid) ? $sid : 0,
    'did' => !empty($did) ? $did : 0,
    'nid' => !empty($nid) ? $nid : 0,
    'market_source' => $market_source,
    'page_name' => $page_name,
    'internal_name' => $internal_name,
    'type' => $type,
    'service' => $service,
    'created' => (int) time(),
    'status' => 'pending',
    'url' => '',
  );

  // go after the sfid of the parent opportunity if sid is present. This handles
  // cases where a user shares from a confirmation page after the submission has synced to salesforce.
  if ($record['sid']) {
    $record['opportunity_id'] = _sb_social_get_opportunity_id_from_submission_id($record['sid']);
  }

  $share_id = $query->fields($record)->execute();
  // generate url and update share record.
  $url = sb_social_share_tracker_url($share_id, $settings, $url_path);

  db_query('UPDATE {sb_social_shares} set url = :url WHERE share_id = :share_id', array(':url' => $url, ':share_id' => $share_id));
  watchdog('sb_social', 'share id:' . $share_id);

  if ($share_id && module_exists('salesforce_queue')) {
    watchdog('sb_social', 'attempted to add share event to queue');
    sb_social_add_share_to_queue($share_id);
  }

  return $url;
}

/**
 * Load social settings by id.
 *
 *If no id is provided, attempt to identify applicable settings from
 * the url.
 *
 * @param bool $id
 * Optional share settings id.
 *
 * @return mixed
 * Returns an associative array containing the social share settings applicable
 * to the ID passed or the current URL.
 */
function sb_social_get_current_settings($id = FALSE) {
  // TODO: consider simplifying this function, since share urls are only generated when
  // a user clicks on a share button, and that happens in the context of the share tracker url
  // most of this functionality may not be required any longer.
  if (!$id) {
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
  $settings = &drupal_static(__FUNCTION__);

  if (empty($settings[$sid])) {
    $results = db_query('SELECT * FROM {sb_social_settings} WHERE sid = :sid', array(':sid' => $sid));

    $settings[$sid] = $results->fetchAssoc();
    if ($settings[$sid] && !empty($settings[$sid]['data'])) {
      $settings[$sid]['data'] = unserialize($settings[$sid]['data']);
    }
    if ($settings[$sid]['type'] == 'path') {
      $settings[$sid]['path'] = sb_social_get_path($settings[$sid]['id']);
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

/**
 * Lookup ID associated with the share settings ID provided.
 *
 * @param $sid
 * Share settings ID
 *
 * @return mixed
 * returns the node id or path associated with the share settings ID.
 */
function sb_social_get_id_by_sid($sid) {
  $result = db_query('SELECT id FROM {sb_social_settings} WHERE sid = :sid', array(':sid' => $sid));
  return $result->fetchField();
}

/**
 * Get share settings id by drupal id
 *
 * @param $id
 * Node id for node settings, path for path settings.
 *
 * @param $type
 * optional record type
 *
 * @return bool Returns the sid associated with the given id, or FALSE if no records were found.
 */
function sb_social_get_sid_by_id($id, $type = FALSE) {

  $query = db_select('sb_social_settings', 's');
  $query->fields('s', array('sid'));
  $query->condition('id', $id, '=');
  if ($type) {
    $query->condition('type', $type, '=');
  }
  $result = $query->execute();
  $data = $result->fetchField();
  return $data ? $data : FALSE;
}

/**
 * Load all path settings, used by path admin list page.
 */
function sb_social_path_settings_load_all() {
  $settings = array();
  $results = db_query('SELECT s.sid, p.path FROM {sb_social_settings} s INNER JOIN {sb_social_path} p ON s.id = p.pid;');
  while ($item = $results->fetchAssoc()) {
    $settings[] = $item;
  }
  return $settings;
}


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

/**
 * List services enabled on the Social admin settings form.
 *
 * @return array
 * An array of enabled services.
 */
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
 * Save share settings.
 *
 * @param array $settings
 *
 */
function sb_social_settings_save($settings) {

  if (is_array($settings['data'])) {
    $settings['data'] = serialize($settings['data']);
  }
  $update = array();
  $sid = _sb_social_sid_by_id($settings['id']);

  // if there is a share settings id associated with the settings id,
  // update the existing record.
  if ($sid) {
    $settings['sid'] = $sid;
    $update = array('sid');
  }
  drupal_write_record('sb_social_settings', $settings, $update);
}

/**
 * Delete share settings.
 *
 * @param $sid
 * share settings id of record to delete.
 */
function sb_social_settings_delete($sid) {
  $query = db_delete('sb_social_settings');
  $query->condition('sid', $sid);
  $query->execute();
}

/**
 * Lookup social share settings by webform submission id.
 *
 * @param int $sid
 * Webform submission id
 *
 * @return array
 * returns
 */
function _sb_social_get_settings_by_sid($submission_id) {
  $nid = _sb_social_get_nid_by_sid($submission_id);
  $sid = sb_social_get_sid_by_id($nid, 'node');
  return sb_social_settings_load($sid);
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

/**
 * Load a partial submission record from webform_submissions
 *
 * @param int $sid
 * submission id
 *
 * @return mixed
 */
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

/**
 * Lookup donation details by webform submission id.
 *
 * @param int $sid
 * submission id
 *
 * @return mixed
 */
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

/**
 * Lookup numeric id of path.
 *
 * @param $path
 * Drupal path, may be a string or an array of original path and alias.
 */
function sb_social_get_path_id($path) {
  $query = db_select('sb_social_path', 'p');
  $query->fields('p',array('pid'));

  if (is_array($path)) {
    $or = db_or();
    foreach ($path as $item) {
      $or->condition('path', $item, '=');
    }
    $query->condition($or);
  }
  else {
    $query->condition('path', $path, '=');
  }
  $results = $query->execute();

  return $results->fetchField();
}

/**
 * Save path record and return path id.
 *
 * This permits us to reference an arbitrary Drupal path
 * with a unique numeric identifier, which can then be used as
 * the numeric portion of edit form paths or passed as an ID to
 * the share tracking callback.
 *
 * @param string $path
 * Drupal menu path.
 *
 * @return int
 */
function sb_social_save_path($path) {
  $query = db_insert('sb_social_path');
  $record = array('path' => $path);
  $path_id = $query->fields($record)->execute();
  return $path_id;
}

/**
 * Lookup Drupal menu path by numeric path id.
 *
 * @param int $id
 * Numeric id of the drupal path we're looking for.
 *
 * @return string
 * Returns the requested path.
 */
function sb_social_get_path($id) {
  $results = db_query('SELECT path FROM {sb_social_path} WHERE pid = :id', array(':id' => $id));
  return $results->fetchField();
}

/**
 * Lookup webform submission ID for a given fundraiser donation id.
 */
function _sb_social_get_webform_sid_by_donation_id($did) {
  $result = db_query('SELECT sid FROM {fundraiser_donation} WHERE did = :did', array(':did' => $did));
  return $result->fetchField();
}

/**
 * Add opportunity sfid to {sb_social_shares}
 *
 * @param $item
 * Salesforce queue item
 *
 * @param $record
 * Salesforce queue response record
 *
 * @return bool
 */
function sb_social_update_opportunity_id($item, $record) {
  if (empty($record->id) || $record->success != TRUE) {
    return FALSE;
  }
  $sid = _sb_social_get_webform_sid_by_donation_id($item->drupal_id);
  db_query('UPDATE {sb_social_shares} SET opportunity_id = :oppid WHERE sid = :sid', array(':oppid' => $record->id, ':sid' => $sid));
  return TRUE;
}

/**
 * Add share sfid to {sb_social_shares}
 *
 * @param $item
 * Salesforce queue item
 *
 * @param $record
 * Salesforce queue response record
 */
function sb_social_update_sf_share_id($item, $record) {
  db_query('UPDATE {sb_social_shares} SET sf_share_id = :sf_share_id WHERE share_id = :share_id', array(':sf_share_id' => $record->id, 'share_id' => $item->drupal_id));
}

/**
 * Lookup the sfid of a share event that is stored in {salesforce_sync_map}
 *
 * @param $share_id
 * share event id
 *
 * @return mixed
 */
function _sb_social_get_share_salesforce_id_from_map($share_id) {
  $result = db_query('SELECT sfid FROM {salesforce_sync_map} WHERE drupal_id = :share_id AND module = :module AND delta = :delta', array(':share_id' => $share_id, ':module' => "sb_social", ':delta' => "share_event"));
  return $result->fetchField();
}

/**
 * Look up the Opportunity Salesforce ID associated with a given webform submission id.
 *
 * @param $sid
 * Webform submission ID.
 *
 * @return int
 */
function _sb_social_get_opportunity_id_from_submission_id($sid) {
  $result = db_query('
    SELECT
      s.sfid
    FROM {salesforce_sync_map} s
    INNER JOIN {fundraiser_donation} f
      ON f.did = s.drupal_id
    WHERE f.sid = :sid', array(':sid' => $sid));
  $sfid = $result->fetchField();
  return $sfid ? $sfid : 0;
}

/**
 * Look up the Opportunity Salesforce ID for share events associated with a donation.
 *
 * @param int $share_id
 * share id
 *
 * @return bool|string
 * returns the sfid if one is available or FALSE.
 */
function _sb_social_get_opportunity_id_from_share_id($share_id) {
  $result = db_query('SELECT sid FROM {sb_social_shares} WHERE share_id = :share_id', array(':share_id' => $share_id));
  $sid = $result->fetchField();
  if ($sid) {
    return _sb_social_get_opportunity_id_from_submission_id($sid);
  }
  return FALSE;
}

/**
 * Save a record to the submissions tracking table.
 *
 * When a user comes to the site from a share url and makes a donation we want
 * to store that information.
 *
 * @param $share_id
 * @param $submission_id
 * @param int $donation_id
 */
function sb_social_submission_save($share_id, $submission_id, $donation_id = 0) {
  $record = array(
    'share_id' => $share_id,
    'sid' => $submission_id,
    'did' => $donation_id,
  );
  drupal_write_record('sb_social_submissions', $record);
}

/**
 * Return a list of donation ids that are related to this share event.
 *
 * @param int $share_id
 * Social Share event ID.
 */
function sb_social_load_related_donation_ids($share_id) {
  $donation_ids = array();
  $result = db_query('SELECT did FROM {sb_social_submissions} WHERE share_id = :share_id', array(':share_id' => $share_id));
  while ($donation_id = $result->fetchField()) {
    $donation_ids[] = $donation_id;
  }
  return $donation_ids;
}

/**
 * Add did value to shares table.
 */
function sb_social_update_did($sid, $did) {
  db_query('UPDATE {sb_social_shares} SET did = :did WHERE sid = :sid', array(':did' => $did, ':sid' => $sid));
}

/**
 * List share records related to a donation.
 *
 * @param $drupal_id
 * @return array
 */
function sb_social_get_shares_related_to_donation($did) {
  $related_shares = array();

  $results = db_query('SELECT * FROM {sb_social_shares} WHERE did = :did', array(':did' => $did));
  while ($share = $results->fetchAssoc) {
    $related_shares[] = $share;
  }
  return $related_shares;
}

/**
 * Stripped down version of fundraiser_get_donation_by_sid. In this instance
 * we don't need the entire donation record.
 *
 * @param $sid
 * webform submission id.
 *
 * @return mixed
 */
function _sb_social_get_did_by_sid($sid) {
  $results = db_query('SELECT did FROM {fundraiser_donation} WHERE sid = :sid', array(':sid' => $sid));
  return $results->fetchField();
}