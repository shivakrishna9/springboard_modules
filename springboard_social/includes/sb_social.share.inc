<?php

/**
 * @file share event tracking
 */


/**
 * Track individual share event
 *
 * @param $service
 * Social media service (facebook, twitter)
 *
 * @param $id
 * id associated with share event, submission id for webforms, node id for node pages, path for path-based shares.
 *
 * @param $type
 * ID type, sid, nid, or path
 *
 * @param $market_source
 * Market source value for the share event.
 */
function sb_social_share_tracker($service, $id, $type, $market_source) {
  $enabled_services = sb_social_get_enabled_services();

  if (in_array($service, $enabled_services)) {
    $share_url = sb_social_share_event_save($service, $id, $type, $market_source);
    $short_url = shorten_url($share_url);
    return drupal_json_output($share_url); //drupal_json_output($share_id);
  }
  else {
   return drupal_json_output(FALSE);
  }
}
