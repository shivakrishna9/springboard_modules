<?php
/**
 * @file
 * Holds deprecated functions that will be removed.
 */

/**
 * Create a fundraiser.
 *
 * @deprecated
 */
function _fundraiser_create_fundraiser($fundraiser) {
  $fundraiser_controller = new FundraiserController();
  if (!$fundraiser instanceof Fundraiser) {
    $fundraiser = $fundraiser_controller->create($fundraiser);
  }
  $fundraiser_controller->save($fundraiser);
}

/**
 * Load a fundraiser by its node ID.
 *
 * @deprecated
 */
function _fundraiser_get_fundraiser_by_nid($nid) {
  $fundraiser_controller = new FundraiserController();
  return $fundraiser_controller->loadByNid($nid);
}

/**
 * Update a fundraiser.
 *
 * @deprecated
 */
function _fundraiser_update_fundraiser($fundraiser) {
  $fundraiser_controller = new FundraiserController();
  return $fundraiser_controller->update($fundraiser);
}

/**
 * Delete a fundraiser.
 *
 * @deprecated
 */
function _fundraiser_delete_fundraiser($nid) {
  $fundraiser_controller = new FundraiserController();
  $fundraiser = $fundraiser_controller->loadByNid($nid);
  $fundraiser_controller->delete($fundraiser);
}

/**
 * Get fundraisers by their nids.
 *
 * @deprecated
 */
function _fundraiser_get_fundraiser_by_nids($nids) {
  $fundraiser_controller = new FundraiserController();
  return $fundraiser_controller->loadByNids($nids);
}

/**
 * Delete fundraisers.
 *
 * @deprecated
 */
function _fundraiser_delete_fundraisers($fundraisers) {
  $fundraiser_controller->delete($fundraisers);
}

$donation_controller = new DonationController();

/**
 * Create a Donation.
 *
 * @deprecated
 */
function _fundraiser_create_donation($donation) {
  if (!$donation instanceof Donation) {
    $donation = $donation_controller->create($donation);
  }

  $donation_controller->save($donation);
}

/**
 * Load donation by its donation ID.
 *
 * @deprecated
 */
function _fundraiser_get_donation_by_did($did) {
  return reset($donation_controller->load(array($did)));
}

/**
 * Load all donations.
 *
 * @deprecated
 */
function _fundraiser_get_donations() {
  return entity_load('donation');
}

/**
 * Update a donation.
 *
 * @deprecated
 */
function _fundraiser_update_donation($donation) {
  return $donation_controller->update($donation);
}

/**
 * Delete a donation.
 *
 * @deprecated
 */
function _fundraiser_delete_donation($did) {
  $donation_controller->delete(array($did));
}

$refund_controller = new EntityAPIController('refund');

/**
 * Create a refund.
 *
 * @deprecated
 */
function _fundraiser_create_refund($refund) {
  if (!$refund instanceof Refund) {
    $refund = $refund_controller->create($refund);
  }

  $refund_controller->save($refund);
}

/**
 * Load a refund by refund ID.
 *
 * @deprecated
 */
function _fundraiser_get_refund_by_rid($rid) {
  $refund_controller->loadSingleById($rid);
}

/**
 * Update a refund.
 *
 * @deprecated
 */
function _fundraiser_update_refund($refund) {
  $refund_controller->update($refund);
}

/**
 * Delete a refund.
 *
 * @deprecated
 */
function _fundraiser_delete_refund($rid) {
  $refund_controller->delete(array($rid));
}

/**
 * Get an array of refunds by a donation ID.
 *
 * @deprecated
 */
function _fundraiser_get_refunds_by_did($did) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'refund')
    ->propertyCondition('did', $did, '=');
  $results = $query->execute();
  if (!empty($results)) {
    $entity_ids = array_keys($results);
    $entities = entity_load($this->entityType, $entity_ids);
    return $entities;
  }
  return FALSE;
}

/**
 * Get a refund by its transaction ID.
 *
 * @deprecated
 */
function _fundraiser_get_refund_by_txn_id($txn_id) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'refund')
    ->propertyCondition('txn_id', $txn_id, '=');
  $results = $query->execute();
  if (!empty($results)) {
    $entity_ids = array_keys($results);
    $entities = entity_load($this->entityType, $entity_ids);
    return $entities;
  }
  return FALSE;
}
