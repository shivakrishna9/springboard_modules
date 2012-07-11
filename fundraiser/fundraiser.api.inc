<?php

/**
 * Available fundraiser hooks.
 */

/**
 * For all modules, provide additional replacement methods for _fundraiser_replace_token().
 * Return the token replaced message string.
 */
function hook_fundraiser_replace_tokens($message, $donation) {}

/**
 * For all modules, get the field info array for fields that fundraiser is expecting.
 * This is used by display and charge glue modules to track field information and provide callbacks for formatting.
 * Return an info array.
 */
function hook_fundraiser_field_info() {}
function hook_fundraiser_field_info_alter(&$info) {}

/**
 * For display glue modules, if display is handled in a non-standard form array, return the requested field.
 * Return a field definition as per normal form arrays.
 */
function hook_fundraiser_get_form_field($form, $field_key) {}

/**
 * For display glue modules, if display is handled in a non-standard form array, update the requested field.
 * Return the updated form. (Yes, all of the form).
 */
function hook_fundraiser_update_form_field($form, $field_key) {}

/**
 * For display glue modules, to provide any additional changes for the donation form display.
 * Called in fundraiser_donation_form(), which must be called from the display module to display the donation form.
 * Return form.
 */
function hook_fundraiser_donation_form($form, $field_key) {}

/**
 * For display glue modules, to provide validation for the donation form.
 * Return $error state.
 */
function hook_fundraiser_donation_validate($form, $form_state) {}

/**
 * For display glue modules, to provide submission for the donation form.
 * Return nothing, but update the donation object with submitted values per field information keys.
 */
function hook_fundraiser_donation_submit($form, $form_state, $donation) {}

/**
 * For all modules, handle any post submission behaviors as needed.
 */
function hook_fundraiser_donation_post_submit($form, $form_state, $donation) {}

/**
 * For all modules, handle any post failed validation behaviors.
 */
function hook_fundraiser_donation_failed_validation($errors) {}

/**
 * For all modules, add any additional data before attenptin to create the donation.
 * Return nothing, but update the donation object with data.
 */
function hook_fundraiser_donation_create_add_data($donation) {}

/**
 * For charge glue modules, create the  donation in the system used to track donations and charges.
 * Return nothing, but update the donation object with data.
 */
function hook_fundraiser_donation_create($donation) {}

/**
 * For all modules, last chance to add data to the donation before updating it.
 * Return nothing, but update the donation object with data.
 */
function hook_fundraiser_donation_update($donation) {}

/**
 * For charge glue modules, attempt to charge the donation.
 * Return nothing, but update the donation object with success results.
 */
function hook_fundraiser_donation_process($donation, $charge_function) {}

/**
 * For all modules, respond to a successful donation.
 */
function hook_fundraiser_donation_success($donation) {}

/**
 * For all modules, respond to a declined donation.
 */
function hook_fundraiser_donation_decline($donation) {}

/**
 * For all modules, respond to an excepted donation. TODO determine where this is called.
 */
function hook_fundraiser_donation_exception($donation) {}

/**
 * For all modules, respond to a cancelled donation.
 */
function hook_fundraiser_donation_cancel($donation) {}

/**
 * For charge glue modules, respond to a cancelled donation.
 * Return nothing, but update the donation object with success results.
 */
function hook_fundraiser_donation_refund($donation, $refund_function) {}

/**
 * For all modules, respond to a successful refund.
 */
function hook_fundraiser_donation_refund_success($donation) {}

/**
 * For all modules, respond to a declined refund.
 */
function hook_fundraiser_donation_refund_decline($donation) {}

/**
 * For all modules, add additional data to the donation object.
 * Return nothing, but update the donation object with results.
 */
function hook_fundraiser_donation_get_donation($donation) {}

/**
 * For all modules, provide available gateway information for all gateways.
 */
function hook_fundraiser_gateway_info() {}
function hook_fundraiser_gateway_info_alter(&$gateways) {}

/**
 * For all modules, respond to the creation of a fundraiser type node.
 * Return nothing, but update the fundraiser object with results.
 */
function hook_fundraiser_create_fundraiser_alter($fundraiser) {}

/**
 * For all modules, add additional data to the fundraiser object.
 * Return nothing, but update the fundraiser object with results.
 */
function hook_fundraiser_get_fundraiser_alter($fundraiser) {}

/**
 * For all modules, add additional data to the fundraiser object.
 * Return nothing, but update the fundraiser object with results.
 */
function hook_fundraiser_update_fundraiser_alter($fundraiser) {}

/**
 * For all modules, add additional data to the fundraiser object.
 * Return nothing, but update the fundraiser object with results.
 */
function hook_fundraiser_delete_fundraiser_alter($fundraiser) {}

/**
 * For all modules allow gateway information to be updated or changed.
 * Return gateway information from a given donation.
 */
function hook_fundraiser_get_donation_gateway($did, $gateway) {}
