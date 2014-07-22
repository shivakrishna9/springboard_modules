<?php

/**
 * @file
 * The controller for the donation entity containing the CRUD operations.
 */

/**
 * The controller class for donations contains methods for the donation CRUD
 * operations. The load method is inherited from the default controller.
 */
class FundraiserDonationEntityController extends EntityAPIController {

  /**
   * Create a default donation entity.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   An order object with all default fields initialized.
   */
  public function create(array $values = array()) {
    if (!isset($values['uid'])) {
      $account = $GLOBALS['user'];
      $values['uid'] = $account->uid;
    }

    $values += array(
      'nid' => 0,
      'sid' => 0,
      'status' => 1,
      'amount' => '',
      'currency' => '',
      'txn_id' => '',
      'form_url' => '',
      'created' => '',
      'changed' => '',
    );

    return parent::create($values);
  }

  /**
   * Saves an order.
   *
   * Saves the donation. Runs before _fundraiser_create_donation which saves the non-field data.
   * _fundraiser_create_donation checks for an existing record so we don't have to worry about id collisions
   *
   * @param $order
   *   The full donation object to save.
   * @param $transaction
   *   An optional transaction object.
   *
   * @return
   *   SAVED_NEW or SAVED_UPDATED depending on the operation performed.
   */
  public function save($donation, DatabaseTransaction $transaction = NULL) {
    if (!isset($transaction)) {
      $transaction = db_transaction();
      $started_transaction = TRUE;
    }

    try {
      global $user;

      // Determine if we will be inserting a new donation. 
      // The id for the donations are set by Commerce; we have to check the table to ensure its new
      if (empty($donation->is_new) && !empty($donation->did))  {
        $exist = db_query('SELECT did FROM {fundraiser_donation} WHERE did = :did', array(':did' => $donation->did))->fetchField();
        if (!$exist) {
          $donation->is_new = TRUE;
        }
      }

      // Set the timestamp fields.
      if ($donation->is_new) {
        $donation->created = REQUEST_TIME;
      }
      else {
        // Otherwise if the order is not new but comes from an entity_create()
        // or similar function call that initializes the created timestamp, uid,
        // and hostname values to empty strings, unset them to prevent
        // destroying existing data in those properties on update.
        if ($donation->created === '') {
          unset($donation->created);
        }
        if ($donation->uid === '') {
          unset($donation->uid);
        }
        if ($donation->nid === '') {
          unset($donation->nid);
        }
        if ($donation->sid === '') {
          unset($donation->sid);
        }
      }

      $donation->changed = REQUEST_TIME;

      return parent::save($donation, $transaction);
    }
    catch (Exception $e) {
      if (!empty($started_transaction)) {
        $transaction->rollback();
        watchdog_exception($this->entityType, $e);
      }
      throw $e;
    }
  }

  /**
   * Unserializes the data property of loaded orders.
   */
  public function attachLoad(&$queried_orders, $revision_id = FALSE) {
    foreach ($queried_orders as $order_id => &$order) {
      $order->data = unserialize($order->data);
    }

    // Call the default attachLoad() method. This will add fields and call
    // hook_commerce_order_load().
    parent::attachLoad($queried_orders, $revision_id);
  }
}
