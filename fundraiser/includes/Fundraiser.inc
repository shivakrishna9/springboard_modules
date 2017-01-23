<?php
/**
 * @file
 * ...
 */

class Fundraiser extends Entity {
  protected $nodeId;
  protected $receiptEmailFrom;
  protected $receiptEmailAddress;
  protected $receiptEmailSubject;
  protected $receiptEmailMessage;
  protected $donationAmounts = array();
  protected $showOtherAmount;
  protected $askForQuantity;
  protected $minimumDonationAmount;
  protected $gateways = array();
  protected $defaultAmount;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $values = array(), $entity_type = NULL) {
    parent::__construct($values, 'fundraiser');
  }

  /**
   * {@inheritdoc}
   */
  public function save() {
    // Prepare the donation amounts.
    $donation_amounts = array();
    foreach ($this->donationAmounts as $index => $donation_amount) {
      if (!empty($donation_amount['amount']) && !empty($donation_amount['label']) && !empty($donation_amount['weight'])) {
        $donation_amounts[$donation_amount['weight']] = $donation_amount['amount'] . '|' . $donation_amount['label'];
      }
    }

    if (!empty($donation_amounts)) {
      ksort($donation_amounts);
      $donation_amounts = array_unique($donation_amounts);
    }

    $this->donationAmounts = implode('|||', $donation_amounts);

    // Serialize the gateways for storage.
    $this->gateways = serialize($this->gateways);

    if (empty($this->receiptEmailFrom)) {
      $this->receiptEmailFrom = variable_get('site_name', '');
    }

    if (empty($this->receiptEmailAddress)) {
      $this->receiptEmailAddress = variable_get('site_mail', '');
    }

    if (empty($this->receiptEmailSubject)) {
      $this->receiptEmailSubject = t('Thank you for your donation');
    }

    return parent::save();
  }

  /**
   * {@inheritdoc}
   */
  protected function defaultUri() {
    return array('path' => 'node/' . $this->nodeId);
  }
}

class FundraiserController extends EntityAPIController {
  protected $entityType = 'fundraiser';

  /**
   * {@inheritdoc}
   */
  public function __construct() {
    parent::__construct('fundraiser');
  }

  /**
   * Update a fundraiser.
   *
   * @param array $values
   *   An array of property values for the fundraiser, keyed by the property
   *   name.
   *
   * @return Entity
   *   The updated fundraiser entity.
   */
  public static function update(array $values) {
    if (empty($values['nid'])) {
      throw new FundraiserNotFoundException('Cannot update a fundraiser without an associated node ID.');
    }

    $fundraiser = $this->loadByNid($values['nid']);
    foreach ($values as $property => $value) {
      $fundraiser->{$property} = $value;
    }

    // Let other modules respond to the updating of a fundraiser.
    module_invoke_all('fundraiser_update_fundraiser_alter', $fundraiser);

    return $fundraiser->save();
  }

  /**
   * Delete a fundraiser by its node ID.
   *
   * @param int $nid
   *   The node ID.
   */
  public function deleteByNid($nid) {
    $this->delete($this->loadByNid($nid));
  }

  /**
   * Load a fundraiser by its node ID.
   *
   * @param int $nid
   *   The fundraiser's node ID.
   *
   * @return Fundraiser
   *   The fundraiser entity.
   *
   * @throws FundraiserNotFoundException
   */
  public function loadByNid($nid) {
    $nids = array($nid);
    $entities = $this->loadByNids($nids);
    $entity = array_shift($entities);
    return $entity;
  }

  /**
   * Load multiple fundraisers by their node IDs.
   *
   * @param array $nids
   *   An array of node IDs.
   *
   * @return array
   *   An array of fundraiser entities.
   */
  public function loadByNids(array $nids) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $this->entityType)
      ->propertyCondition('nid', $nids, 'in');
    $results = $query->execute();

    if (empty($results)) {
      throw new FundraiserNotFoundException('No entities could be loaded with the node IDs provided.');
    }

    $entity_ids = array_keys($results);
    $entities = entity_load($this->entityType, $entity_ids);

    if (empty($results)) {
      throw new FundraiserNotFoundException('No entities could be loaded with the node IDs provided.');
    }

    return $entities;
  }
}

class FundraiserNotFoundException extends Exception {}
