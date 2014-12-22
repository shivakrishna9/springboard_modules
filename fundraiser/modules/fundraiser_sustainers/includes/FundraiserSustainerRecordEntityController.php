<?php

class FundraiserSustainerRecordEntityController extends EntityAPIController {

  /**
   * Create a default sustainer record.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   A sustainer record object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'master_did' => NULL,
      'did' => NULL,
      'next_charge' => NULL,
      'gateway_resp' => NULL,
      'attempts' => 0,
      'cancellation_reason' => NULL,
      'sustainer_key' => '',
      'lock_id' => '0',
      'revision_id' => NULL,
      'created' => '',
      'changed' => '',
    );

    return parent::create($values);
  }

  /**
   * Saves a sustainer record.
   *
   * When saving a sustainer record without a donation ID, this function will
   * create a new sustainer record at that time. For new sustainer records,
   * it will also save the initial revision of the sustainer record. Subsequent
   * sustainer records that should be saved as new revisions should set
   * $sustainer_record->is_new_revision to TRUE and include a log string in
   * $sustainer_record->log.
   *
   * @param FundraiserSustainerRecordEntity $sustainer_record
   *   The full sustainer record entity object to save.
   * @param DatabaseTransaction $transaction
   *   An optional transaction object.
   *
   * @return
   *   SAVED_NEW or SAVED_UPDATED depending on the operation performed.
   */
  public function save($sustainer_record, DatabaseTransaction $transaction = NULL) {
    $sustainer_record->is_new = !empty($sustainer_record->is_new) || empty($sustainer_record->did);

    // Default to making a new revision if it hasn't been explicitly set.
    if (!isset($sustainer_record->is_new_revision)) {
      $sustainer_record->is_new_revision = TRUE;
    }

    // Set the timestamp fields.
    if ($sustainer_record->is_new && empty($sustainer_record->created)) {
      $sustainer_record->created = REQUEST_TIME;
    }
    else {
      // Otherwise if the entity is not new but comes from an entity_create()
      // or similar function call that initializes the created timestamp
      // value to an empty string, unset it to prevent
      // destroying existing data in that property on update.
      if ($sustainer_record->created === '') {
        unset($sustainer_record->created);
      }
    }

    $sustainer_record->changed = REQUEST_TIME;
    $sustainer_record->revision_timestamp = REQUEST_TIME;

    if ($sustainer_record->is_new || !empty($sustainer_record->is_new_revision)) {
      // When inserting either a new entity or revision, $sustainer_record->log
      // must be set because {fundraiser_sustainers_revision}.log is a
      // text column and therefore cannot have a default value.
      // However, it might not be set at this point, so we ensure that it is
      // at least an empty string in that case.
      if (!isset($sustainer_record->log)) {
        $sustainer_record->log = '';
      }
    }
    elseif (empty($sustainer_record->log)) {
      // If we are updating an existing entity without adding a new revision,
      // we need to make sure $sustainer_record->log is unset whenever it is
      // empty. As long as $sustainer_record->log is unset,
      // drupal_write_record() will not attempt to update the existing
      // database column when re-saving the revision.
      unset($sustainer_record->log);
    }

    return parent::save($sustainer_record, $transaction);
  }

}
