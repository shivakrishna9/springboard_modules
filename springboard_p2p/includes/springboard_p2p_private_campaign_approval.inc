<?php
/**
 * @file
 * A class for managing private campaign approvals.
 */

class SpringbaordP2pPrivateCampaignApproval {

  protected $record = array();

  public function __construct($uid, $nid) {
    $this->record = array(
      'uid' => $uid,
      'nid' => $nid,
    );

    return $this;
  }

  public function request() {
    $this->write('requested');
  }

  public function approve() {
    $this->write('approved');
  }

  public function reject() {
    $this->write('rejected');
  }

  protected function delete() {
    db_delete('springboard_p2p_private_campaign_approval')
      ->condition('nid', $this->record['nid'])
      ->condition('uid', $this->record['uid'])
      ->execute();
  }

  protected function write($status) {
    $this->record['status'] = $status;
    $this->record['created'] = time();
    $this->delete();
    drupal_write_record('springboard_p2p_private_campaign_approval', $this->record);
  }
}
