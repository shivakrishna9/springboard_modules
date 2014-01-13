<?php
/**
 * @file
 * Hooks for fundraiser_upsell
 */

/**
 * Some definitions.
 *
 * Upsold Donation
 *   The original one-time donation.  This should always be in the past tense.
 *
 * Upsell Recurring Donation
 *   The recurring donation that is a result of being upsold.  In Salesforce
 *   terms this is the recurring donation object.  In Drupal terms this is
 *   the same as the upsell master donation.  Both objects have the same
 *   donation ID.
 *
 * Upsell Master Donation
 *   The donation that is the first in the sustainer series and is copied
 *   for the other donations in the series.
 *
 * Upsell (Slave) Donation
 *   The individual donations in the upsell sustainer series that are not the
 *   master donation.
 */

/**
 * Add your own settings to the fundraiser upsell area.
 *
 * For example, adding custom salesforce and market source fields that would
 *   apply to only the upsell sustaienr series and not the original upsold
 *   donation.  You'll need to add your own submit handler to modify or save
 *   the values.
 *
 * @param array $form
 *   The forms API array for the node add/edit page, after fundraiser_upsell
 *   has added its fields, but possibly before other form alters.
 *   Edit in place.
 */
function hook_upsell_node_settings_alter(&$form) {

}

/**
 * Alter the upsold donation object.
 *
 * This happens after the master donation for the sustainer series has been
 *   copied from the upsold donation.  So changes here will not be applied to
 *   the master donation.
 *
 * @param object $upsold_donation
 *   The donation object for the original upsold donation.
 *   $upsold_donation->fundraiser_upsell->related_did will contain the
 *   donation ID of the master donation.
 */
function hook_upsold_donation_alter(&$upsold_donation) {

}

/**
 * Alter the master donation in the upsell sustainer series.
 *
 * This happens after the master donation is copied from the upsold donation,
 *   its values are edited, and its recurring DB record is created. It's
 *   before the sustainer series is created.  So changes here will
 *   get copied to the sustainer donations.
 *
 * An example of this hook in use is to queue up the master donation as a
 *   recurring donation and as a donation/opportunity in Salesforce.
 *
 * @param $master_donation
 *   The master donation object.
 *   $master_donation->fundraiser_upsell->related_did will contain the
 *   donation ID of the upsold donation.
 */
function hook_upsell_master_donation_alter(&$master_donation) {

}
