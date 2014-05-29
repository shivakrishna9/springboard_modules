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
  public function userRegistersForP2P($account, $campaign = NULL) {
    springboard_p2p_register_user_for_p2p($account->uid);

    if (module_exists('rules')) {
      $dashboard_url = $this->dashboardUrl($account->uid);
      $create_personal_campaign_url = $dashboard_url;

      if (!is_null($campaign) && isset($campaign->nid)) {
        $create_personal_campaign_url = $this->createUrl($campaign->nid);
      }

      rules_invoke_event(
        'springboard_p2p_user_register',
        $account,
        $dashboard_url,
        $create_personal_campaign_url
      );
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
      rules_invoke_event(
        'springboard_p2p_user_requests_approval',
        $account,
        $this->dashboardUrl($account->uid),
        $this->createUrl($campaign->nid),
        $campaign
      );
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
      rules_invoke_event(
        'springboard_p2p_user_is_approved',
        $account,
        $this->dashboardUrl($account->uid),
        $this->createUrl($campaign->nid),
        $campaign
      );
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
      rules_invoke_event(
        'springboard_p2p_user_is_rejected',
        $account,
        $this->dashboardUrl($account->uid),
        $this->createUrl($campaign->nid),
        $campaign
      );
    }
  }

  /**
   * Provides the user P2P dashboard URL.
   *
   * @param int $uid
   *   The user ID.
   *
   * @return string
   *   The URL.
   */
  protected function dashboardUrl($uid) {
    $options = array('absolute' => TRUE);
    return url('user/' . $uid . '/personal-campaigns', $options);
  }

  /**
   * Provides the create personal campaign URL for a given campaign.
   *
   * @param int $campaign_nid
   *   The campaign node ID.
   *
   * @return string
   *   The URL.
   */
  protected function createUrl($campaign_nid) {
    $options = array(
      'absolute' => TRUE,
      'query' => array(
        'p2p_cid' => $campaign_nid,
      ),
    );
    return url('node/add/p2p-personal-campaign', $options);
  }

}
