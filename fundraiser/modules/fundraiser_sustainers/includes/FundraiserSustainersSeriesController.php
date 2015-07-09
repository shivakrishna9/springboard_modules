<?php
/**
 * @file
 * The controller for the fundraiser sustainers donation entity.
 */

class FundraiserSustainersDonationEntityController extends EntityAPIController {

  /**
   * Saves a fundraiser sustainers donation entity.
   *
   * @param FundraiserSustainersDonation $fundraiser_sustainers_donation
   *   The fundraiser sustainers donation to save.
   *
   * @return FundraiserSustainersDonation
   *   The saved fundraiser sustainers donation.
   */
  public function save($fundraiser_sustainers_donation) {
    $fundraiser_sustainers_donation->updated = time();

    if (isset($fundraiser_sustainers_donation->is_new) && $fundraiser_sustainers_donation->is_new) {
      $fundraiser_sustainers_donation->created = time();
    }

    parent::save($fundraiser_sustainers_donation);

    return $fundraiser_sustainers_donation;
  }

}
