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
 * @param $amount
 *   The amount that future donations are upgraded to.
 *
 */
function hook_fundraiser_sustainers_upgrade_success($master_did, $amount) {

}

/**
 * @} End of "addtogroup hooks".
 */
