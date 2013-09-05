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
 * @param $sid
 * Webform submission id associated with share event
 *
 */
function sb_social_share_tracker($service, $sid = FALSE) {
  ctools_include('plugins');
  $plugins = ctools_get_plugins('sb_social', 'social_networks');
  $enabled_services = sb_social_get_enabled_services();

  if (in_array($service, $enabled_services)) {
    sb_social_share_event_save($service, $sid);
    return "Share tracked";
  }
  return FALSE;
}
