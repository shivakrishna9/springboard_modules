<?php

/**
 * @file
 * Helper for constructing common objects. Although these classes could be used
 * to save a bit of time, this file is primarily intended for documentation.
 */

/**
 * Helper class to build the base structure of a sync item.
 */
class SalesforceSyncItem {

  /**
   * Constructs a SalesforceSyncItem.
   *
   * @param string $object_type
   *   The type of object the item is as it lives in Salesforce, such as
   *   'Contact' or 'Opportunity'.
   * @param string $operation
   *   The operation to perform against the target system.  The supported
   *   options are 'CREATE', 'UPSERT', 'UPDATE', 'DELETE' and 'UNDELETE'.
   * @param array $sync_rules
   *   Instructions for how the item should syncronize with its consumer.  It is
   *   formatted as follows:
   *   <code>
   *     $sync_rules = array(
   *       'FirstName' => SALESFORCE_SYNC_RULE_ALWAYS,
   *       'Email' => SALESFORCE_SYNC_RULE_BLANK,
   *     );
   *   </code>
   * @param string $dedupe_field
   *   The field with which whose value identifies this record as a unique item,
   *   such as 'Email' or 'Name'.
   */
  public function __construct($object_type = '', $operation = NULL, $sync_rules = array(), $dedupe_field = NULL) {
    $this->object_type = $object_type;
    $this->dedupe_field = $dedupe_field;
    $this->operation = $operation;
    $this->sobject = new SalesforceObject($object_type);
    $this->sync_rules = $sync_rules;
  }

}

/**
 * Helper class to build the base structure of a sObject.
 */
class SalesforceObject {

  /**
   * Unique identifier provided by Salesforce.
   *
   * @var string
   */
  public $Id;

  /**
   * Type of object in Salesforce, such as 'Contact' or 'Opportunity'.
   *
   * @var string
   */
  public $type;

  /**
   * Subtype of the object type in Salesforce.  Not all have a record.
   *
   * @var type
   */
  public $record;

  /**
   * Values of the fields for this record.
   *
   * The array is formatted as follows:
   *   <code>
   *     $fields = array(
   *       'FirstName' => 'John',
   *       'LastName' => 'Doe',
   *       'Email' => 'john.doe@example.com',
   *     );
   *   </code>
   *
   * @var array
   */
  public $fields = array();

  /**
   * Instructions to Salesforce for which fields to null.
   *
   * The array is an unassociated array of field names, such as:
   *   <code>
   *     $fieldsToNull = array(
   *       'FirstName',
   *       'LastName',
   *       'Email',
   *     );
   *   </code>
   *
   * @var type
   */
  public $fieldsToNull = array();

  /**
   * Constructs a sObject.
   *
   * @param string $object_type
   *   The type of object the item is as it lives in Salesforce, such as
   *   'Contact' or 'Opportunity'.
   * @param array $fields
   *   An array of field names and values of the names for this record.  It is
   *   formatted as follows:
   *   <code>
   *     $fields = array(
   *       'FirstName' => 'John',
   *       'LastName' => 'Doe',
   *       'Email' => 'john.doe@example.com',
   *     );
   *   </code>
   * @param string $record
   *   Subtype of the object type.
   */
  public function __construct($object_type = '', $fields = array(), $record = NULL) {
    $this->type = $object_type;
    $this->record = $record;
    $this->fields = $fields;
  }

}
