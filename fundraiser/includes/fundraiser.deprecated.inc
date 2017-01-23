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
  $controller = entity_get_controller('fundraiser');
  if (!$fundraiser instanceof Fundraiser) {
    $fundraiser = $controller->create($fundraiser);
  }
  $controller->save($fundraiser);
}

/**
 * Load a fundraiser by its node ID.
 *
 * @deprecated
 */
function _fundraiser_get_fundraiser_by_nid($nid) {
  return entity_get_controller('fundraiser')->loadByNid($nid);
}

/**
 * Update a fundraiser.
 *
 * @deprecated
 */
function _fundraiser_update_fundraiser($fundraiser) {
  return entity_get_controller('fundraiser')->update($fundraiser);
}

/**
 * Delete a fundraiser.
 *
 * @deprecated
 */
function _fundraiser_delete_fundraiser($nid) {
  $controller = entity_get_controller('fundraiser');
  $fundraiser = $controller->loadByNid($nid);
  $controller->delete($fundraiser);
}

/**
 * Get fundraisers by their nids.
 *
 * @deprecated
 */
function _fundraiser_get_fundraiser_by_nids($nids) {
  return entity_get_controller('fundraiser')->loadByNids($nids);
}

/**
 * Delete fundraisers.
 *
 * @deprecated
 */
function _fundraiser_delete_fundraisers($fundraisers) {
  entity_get_controller('fundraiser')->delete($fundraisers);
}

/**
 * Create a Donation.
 *
 * @deprecated
 */
function _fundraiser_create_donation($donation) {
  $controller = entity_get_controller('donation');
  if (!$donation instanceof Donation) {
    $donation = $controller->create($donation);
  }

  $controller->save($donation);
}

/**
 * Load donation by its donation ID.
 *
 * @deprecated
 */
function _fundraiser_get_donation_by_did($did) {
  return reset(entity_get_controller('donation')->load(array($did)));
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
  return entity_get_controller('donation')->update($donation);
}

/**
 * Delete a donation.
 *
 * @deprecated
 */
function _fundraiser_delete_donation($did) {
  entity_get_controller('donation')->delete(array($did));
}

/**
 * Create a refund.
 *
 * @deprecated
 */
function _fundraiser_create_refund($refund) {
  $controller = entity_get_controller('refund');
  if (!$refund instanceof Refund) {
    $refund = $controller->create($refund);
  }

  $controller->save($refund);
}

/**
 * Load a refund by refund ID.
 *
 * @deprecated
 */
function _fundraiser_get_refund_by_rid($rid) {
  entity_get_controller('refund')->loadSingleById($rid);
}

/**
 * Update a refund.
 *
 * @deprecated
 */
function _fundraiser_update_refund($refund) {
  entity_get_controller('refund')->update($refund);
}

/**
 * Delete a refund.
 *
 * @deprecated
 */
function _fundraiser_delete_refund($rid) {
  entity_get_controller('refund')->delete(array($rid));
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
