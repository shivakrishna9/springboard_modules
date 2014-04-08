<?php
/**
 * @file
 * A class for managing private campaign approvals.
 */

class SpringbaordP2pPrivateCampaignApproval {

  protected $record = array();

  /**
   * Sets up the data for the approval.
   *
   * @param int $uid
   *   The requesting user's uid.
   * @param int $nid
   *   The campaign nid for which access is being requested.
   */
  public function __construct($uid, $nid) {
    $this->record = array(
      'uid' => $uid,
      'nid' => $nid,
    );

    return $this;
  }

  /**
   * Save status as requested.
   */
  public function request() {
    $this->write('requested');
  }

  /**
   * Save status as approved.
   */
  public function approve() {
    $this->write('approved');
  }

  /**
   * Save status as rejected.
   */
  public function reject() {
    $this->write('rejected');
  }

  /**
   * Delete the record from the table.
   */
  protected function delete() {
    db_delete('springboard_p2p_private_campaign_approval')
      ->condition('nid', $this->record['nid'])
      ->condition('uid', $this->record['uid'])
      ->execute();
  }

  /**
   * Delete existing records for this uid, nid combination, and save a new one.
   *
   * @param string $status
   *   'requested', 'accepted', or 'rejected'
   */
  protected function write($status) {
    $this->record['status'] = $status;
    $this->record['created'] = time();
    $this->delete();
    drupal_write_record('springboard_p2p_private_campaign_approval', $this->record);
  }
}
