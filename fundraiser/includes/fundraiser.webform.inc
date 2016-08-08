<?php
/**
 * @file
 * Webform-related functions for Fundraiser.
 */

/**
 * Implements hook_webform_token_selector_blacklist_alter().
 *
 * Filter out credit card webform components.
 * Note: this replaces _fundraiser_field_blacklist().
 */
function fundraiser_webform_token_selector_blacklist_alter(&$tokens) {
  // These fields may or may not even have a value depending on the user's
  // selected form of submission.
  // So this may not be a viable solution anymore.
  $fundraiser_blacklist = array(
    'card_number',
    'card_cvv',
    'recurs_monthly',
    'card_expiration_month',
    'card_expiration_year',
  );
  $tokens = array_merge($fundraiser_blacklist, $tokens);
}
