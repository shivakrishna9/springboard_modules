<?php

/**
 * Available fundraiser hooks.
 */

/**
 * Allow modifications to the fundraiser settings form.
 */
function hook_fundraiser_donation_settings_form_alter(&$form, &$form_state, &$node) {}

/**
 * For display modules, retrieve donation form implementation.
 */
function hook_fundraiser_donation_form(&$form, &$form_state) {}

/**
 * For display modules, validate donation form submissions.
 */
function hook_fundraiser_donation_validate(&$form, &$form_state) {}

/**
 * For display modules, submit donation form submissions.
 */
function hook_fundraiser_donation_submit(&$form, &$form_state) {}

/**
 * For display modules, act after donation form submissions.
 */
function hook_fundraiser_donation_post_submit(&$form, &$form_state, &$donation, &$result) {}

/**
 * Respond if a donation failed validation at the form level.
 */
function hook_fundraiser_donation_failed_validation(&$errors) {}

/**
 * Retrieve additional fields for donation creation.
 */
function hook_fundraiser_donation_create_additional(&$donation) {}

/**
 * For payment modules, respond to create a donation.
 */
function hook_fundraiser_donation_create(&$donation) {}

/**
 * For payment modules, respond to recursive donations, to create future ones.
 */
function hook_fundraiser_donation_create_future(&$donation) {}

/**
 * Respond to donation processing.
 */
function hook_fundraiser_donation_success(&$donation, &$result) {}

/**
 * Respond if a donation was successfully completed at the gateway.
 */
function hook_fundraiser_donation_success(&$donation) {}

/**
 * Respond if a donation was declined at the gateway.
 */
function hook_fundraiser_donation_decline(&$donation) {}

/**
 * Respond if a donation was excepted. //TODO figure out where this is called.
 */
function hook_fundraiser_donation_exception(&$donation) {}

/**
 * Respond if a donation was cancelled. //TODO figure out where this is called.
 */
function hook_fundraiser_donation_cancel(&$donation) {}

/**
 * Retrieve available gateway implementations.
 */
function hook_fundraiser_gateway_info(&$gateways) {}

/**
 * Respond to a new donation form created.
 */
function hook_fundraiser_create_fundraiser_alter(&$fundraiser) {}

/**
 * Retrieve additional information for a donation form.
 */
function hook_fundraiser_get_fundraiser_alter(&$fundraiser) {}

/**
 * Respond to a new donation form updated.
 */
function hook_fundraiser_update_fundraiser_alter(&$fundraiser) {}

/**
 * Respond to a new donation form deleted.
 */
function hook_fundraiser_delete_fundraiser_alter($nid) {}

/**
 * Retrieve infomation for a recurring donation.
 */
function hook_fundraiser_donation_retrieve_future(&$donation) {}

/**
 * Respond to a recurring donation successfully completed.
 */
function hook_fundraiser_donation_success_recurring(&$donation) {}

/**
 * Respond to a recurring donation declined.
 */
function hook_fundraiser_donation_decline_recurring(&$donation) {}

/**
 * Act before a recurring donation processing round (on cron).
 */
function hook_fundraiser_donation_recurring() {}

/**
 * Act after a recurring donation processing round (on cron).
 */
function hook_fundraiser_donation_post_recurring() {}