<?php
/**
 * @file
 * ...
 */

class Donation extends Entity {
  protected $nodeId;
  protected $userId;
  protected $submissionId;
  protected $status;
  protected $amount;
  protected $currency;
  protected $gateway;
  protected $transactionId;
  protected $fromUrl;
  protected $created;
  protected $changed;
  protected $cardId;

  /**
   * Constructs the entity object.
   */
  public function __construct(array $values = array(), $entity_type = NULL) {
    parent::__construct($values, 'donation');
  }
}

class DonationController extends EntityAPIController {
  /**
   * {@inheritdoc}
   */
  public function __construct() {
    parent::__construct('donation');
  }

  /**
   * Load a single donation by its donation ID.
   *
   * @param int $id
   *   The donation ID.
   *
   * @return Donation
   *   The Donation entity.
   */
  public function loadSingleById($id) {
    return reset($this->load(array($id)));
  }

  /**
   * Update a donation.
   *
   * @param array $values
   *   An array of property values for the donation, keyed by the property name.
   *
   * @return Donation
   *   The updated donation entity.
   */
  public function update(array $values) {
    if (empty($values['did'])) {
      throw new Exception(t('You must specify a donation ID when updating a donation.'));
    }

    $donation = $this->loadSingleById($values['did']);
    foreach ($values as $property => $value) {
      $donation->{$property} = $value;
    }

    return $donation->save();
  }
}
