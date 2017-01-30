<?php

/**
 * @file
 * Hooks provided by the Funraiser Sustainers Upgrade module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Respond to fundraiser_sustainers_series upgrade.
 *
 * This hook is invoked from
 * _fundraiser_sustainers_donation_amount_form_update_donations() after the
 * fundraiser_sustainers_series has been upgraded with the upgraded amount.
 *
 * @param $master_did
 *   The donation id of the master (original) donation.
 *
 * @param $upgrade_form_values
 *   Flattened array of submitted webform component values.
 *
 */
function hook_fundraiser_sustainers_upgrade_success($master_did, $upgrade_form_values) {

}

/**
 * @} End of "addtogroup hooks".
 */
