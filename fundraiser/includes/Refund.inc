<?php
/**
 * @file
 * ...
 */

class Refund extends Entity {
  protected $refundId;
  protected $donationId;
  protected $receiptId;
  protected $status;
  protected $amount;
  protected $currency;
  protected $transactionId;
  protected $reason;
  protected $created;
  protected $changed;

  /**
   * Constructs the entity object.
   */
  public function __construct(array $values = array(), $entity_type = NULL) {
    parent::__construct($values, 'refund');
  }
}

class RefundController extends EntityAPIController {
  /**
   * {@inheritdoc}
   */
  public function __construct() {
    parent::__construct('refund');
  }

  /**
   * Load a single refund by its refund ID.
   *
   * @param int $id
   *   The refund ID.
   *
   * @return Refund
   *   The refund entity.
   */
  public function loadSingleById($id) {
    return reset($this->load(array($id)));
  }

  /**
   * Update a refund.
   *
   * @param array $values
   *   An array of property values for the refund, keyed by the property name.
   *
   * @return Refund
   *   The updated refund entity.
   */
  public function update(array $values) {
    if (empty($values['rid'])) {
      throw new Exception(t('You must specify a refund ID when updating a refund.'));
    }

    $refund = $this->loadSingleById($values['rid']);
    foreach ($values as $property => $value) {
      $donation->{$property} = $value;
    }

    return $refund->save();
  }
}
