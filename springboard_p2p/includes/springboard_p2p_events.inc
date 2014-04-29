<?php

/**
 * @file
 * Handles calling Springboard P2P events and invoking Rules.
 */

class SpringboardP2pEvents {

  /**
   * Gets called when the user registers for P2P.
   *
   * @param object $account
   *   User account of the registering user.
   */
  public function userRegistersForP2P($account) {
    springboard_p2p_register_user_for_p2p($account->uid);
    if (module_exists('rules')) {
      rules_invoke_event('springboard_p2p_user_register', $account);
    }
  }

  /**
   * Gets called when the user requests approval for a private campaign.
   *
   * @param object $account
   *   User requesting approval.
   * @param object $campaign
   *   The private campaign node.
   */
  public function userRequestsApprovalForPrivateCampaign($account, $campaign) {

    if (module_exists('rules')) {
      rules_invoke_event('springboard_p2p_user_requests_approval', $account, $campaign);
    }
  }

  /**
   * Gets called when the user gets approved for a private campaign.
   *
   * @param object $account
   *   User getting approved.
   * @param object $campaign
   *   The private campaign node.
   */
  public function userIsApproved($account, $campaign) {

    if (module_exists('rules')) {
      rules_invoke_event('springboard_p2p_user_is_approved', $account, $campaign);
    }
  }

  /**
   * Gets called when the user gets rejected for a private campaign.
   *
   * @param object $account
   *   User getting rejected.
   * @param object $campaign
   *   The private campaign node.
   */
  public function userIsRejected($account, $campaign) {

    if (module_exists('rules')) {
      rules_invoke_event('springboard_p2p_user_is_rejected', $account, $campaign);
    }
  }

}
