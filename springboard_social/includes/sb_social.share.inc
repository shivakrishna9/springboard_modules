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
function sb_social_share_tracker($service, $sid) {
 sb_social_share_event_save($service, $sid);
 return 'Share tracked';
}