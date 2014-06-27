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

    $args = array(
      'account' => $account,
      'dashboard_url' => $this->dashboardUrl($account->uid),
    );

    if (is_null($campaign)) {
      $event = 'springboard_p2p_user_register_no_campaign';
    }
    else {
      $args['campaign'] = $campaign;
      if (springboard_p2p_campaign_is_private($campaign)) {
        $event = 'springboard_p2p_user_register_private_campaign';
      }
      else {
        $event = 'springboard_p2p_user_register_public_campaign';
        $args['create_personal_campaign_url'] = $this->createUrl($campaign->nid);
      }
    }

    rules_invoke_event_by_args($event, $args);
  }

  /**
   * Gets called when the user requests approval for a private campaign.
   *
   * This only gets called when the user is not also registering for p2p in
   * the same step.
   *
   * @param object $account
   *   User requesting approval.
   * @param object $campaign
   *   The private campaign node.
   */
  public function userRequestsApprovalForPrivateCampaign($account, $campaign) {

    $event = 'springboard_p2p_user_requests_approval';
    $args = array(
      'account' => $account,
      'dashboard_url' => $this->dashboardUrl($account->uid),
      'campaign' => $campaign,
    );

    rules_invoke_event_by_args($event, $args);
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

    $event = 'springboard_p2p_user_is_approved';
    $args = array(
      'account' => $account,
      'dashboard_url' => $this->dashboardUrl($account->uid),
      'campaign' => $campaign,
      'create_personal_campaign_url' => $this->createUrl($campaign->nid),
    );

    rules_invoke_event_by_args($event, $args);
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
    $event = 'springboard_p2p_user_is_rejected';
    $args = array(
      'account' => $account,
      'dashboard_url' => $this->dashboardUrl($account->uid),
      'campaign' => $campaign,
    );

    rules_invoke_event_by_args($event, $args);
  }

  /**
   * Gets called when a user requests a password reset via the p2p pages.
   *
   * @param object $account
   *   User account.
   * @param object|null $campaign
   *   The campaign node, if one is being tracked.
   */
  public function userRequestsPasswordReset($account, $campaign = NULL) {

    // Using the cid so we can add it to the reset url.
    if (!is_null($campaign) && isset($campaign->nid)) {
      $cid = $campaign->nid;
    }
    else {
      $cid = NULL;
    }

    $event = 'springboard_p2p_user_requests_password_reset';
    $args = array(
      'account' => $account,
      'reset_url' => $this->resetUrl($account, $cid),
    );
    rules_invoke_event_by_args($event, $args);

    $language = !empty($account->language) ? $account->language : 'und';

    $replacements = array(
      '%first' => $account->sbp_first_name[$language][0]['safe_value'],
      '%last' => $account->sbp_last_name[$language][0]['safe_value'],
      '%email' => $account->mail,
    );
    watchdog('Springboard P2P', 'Password reset request made by %first %last at %email.', $replacements);
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

  /**
   * Provides the password reset url.
   *
   * @param object $account
   *   The user account.
   * @param null|int $cid
   *   Campaign ID.
   *
   * @return string
   *   The URL.
   */
  protected function resetUrl($account, $cid = NULL) {
    return springboard_p2p_set_password_url($account, $cid);
  }
}
