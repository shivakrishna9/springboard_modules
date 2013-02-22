<?php

/**
 * @file
 * Contains SalesforceSync.
 */

/**
 * Performs syncronization processes between Salesforce and Drupal records.
 */
class SalesforceSync {

  /**
   * Salesforce object to connect with
   *
   * @var Salesforce
   */
  protected $sfapi;

  /**
   * SalesforceSoapPartner object to connect to Salesforce via SOAP.
   *
   * @var SalesforceSoapPartner
   */
  protected $soap;

  /**
   * SalesforceSyncTokenScanner object used to do token replacements in batches.
   *
   * @var $tokenScanner
   */
  protected $tokenScanner;

  /**
   * The maximum number of items that can be sent to Salesforce in one call.
   *
   * @var int
   */
  protected $maxBatchSize = 200;

  /**
   * Maximum length a query can be to Salesforce before it is rejected.
   *
   * @var int
   */
  protected $maxQueryLength = 10000;

  /**
   *  A running log of the new records created in Salesforce.
   *
   * @var array
   */
  public $newSfRecords = array();

  /**
   * Running log of all the Ids of Salesforce records that have been processed.
   *
   * @var array
   */
  public $processedItemIds = array();

  /**
   * Constructor for SalesforceSync.
   *
   * @param Salesforce $sfapi
   *   Optionally provide a Salesforce API object, otherwise the default version
   *   will be used.
   * @param SalesforceSoapPartner $soap
   *   Optionally provide a SOAP partner object, otherwise the default version
   *   will be used.
   */
  public function __construct(Salesforce $sfapi = NULL, SalesforceSoapPartner $soap = NULL) {
    $this->sfapi = is_null($sfapi) ? salesforce_get_api() : $sfapi;
    $this->soap = is_null($soap) ? new SalesforceSoapPartner($this->sfapi) : $soap;
    $this->maxQueryLength = variable_get('salesforce_sync_max_query_length', SALESFORCE_SYNC_MAX_QUERY_LENGTH_DEFAULT);
    $this->maxBatchSize = variable_get('salesforce_sync_max_batch_size', SALESFORCE_SYNC_MAX_BATCH_SIZE_DEFAULT);
    $this->tokenScanner = new SalesforceSyncTokenScanner($this->getProcessedItemIds());
  }

  /**
   * Apply sync rules to an item's fields based on its consumer's values.
   *
   * @param array $item_fields
   *   The array of fields of the item to be manipulated in order to have the
   *   sync rules  applied to it.
   * @param mixed $consumer_fields
   *   The fields of the record that will consume the item and is therefore used
   *   to compare with in order to apply the sync rules.  This can be provided
   *   as an array, or as an object, such as what is recieved from a Salesforce
   *   response.
   * @param array $sync_rules
   *   Associative array of Field-by-field sync rules for the fields that
   *   require rules.  The array is keyed by the Salesforce field name with the
   *   value being one of the 4 sync rules.
   */
  public function applySyncRules($item_fields, $consumer_fields, $sync_rules) {
    $consumer_fields = (array) $consumer_fields;
    foreach ($sync_rules as $field => $sync_rule) {
      switch ($sync_rule) {
        case SALESFORCE_SYNC_RULE_ALWAYS:
          // Leave the field as is.
          break;

        case SALESFORCE_SYNC_RULE_BLANK:
          // Check to see if source version is blank, otherwise set the target's
          // value to that of the source's value.  We are setting the value
          // instead of unsetting the target's field because 1) The dedupe field
          // must be provided when syncing to Salesforce, and the dedupe field
          // would most likely have this sync rule, and 2) When syncing with
          // Drupal unknown effects may occur when an expected field has no
          // value.
          if (isset($consumer_fields[$field]) && strlen($consumer_fields[$field]) > 0) {
            $item_fields[$field] = $consumer_fields[$field];
          }
          break;

        case SALESFORCE_SYNC_RULE_APPEND:
          // When the source version is not blank, add value from source to the
          // begining of the target's value.
          if (isset($consumer_fields[$field]) && !empty($consumer_fields[$field])) {
            $joined_value = $consumer_fields[$field] . ' ' . $item_fields[$field];
            $item_fields[$field] = $joined_value;
          }
          break;

        case SALESFORCE_SYNC_RULE_NEVER:
          // For the same reasoning as the 'blank' sync rule, we are setting the
          // value to the source value rather than unsetting the target field.
          if (isset($consumer_fields[$field])) {
            $item_fields[$field] = $consumer_fields[$field];
          }
          else {
            unset($item_fields[$field]);
          }
          break;
      }
    }
    return $item_fields;
  }


  /**
   * @name salesforce_sync_drupal_to_salesforce Drupal to Salesforce methods
   * @{
   */

  /**
   * Send a bundled batch of instructions to Salesforce.
   *
   * @param array $batch
   *   An array of instructions to send to Salesforce.  The array is structured
   *   as follows:
   *   <code>
   *     $batch = array(
   *       'operation' => 'UPSERT',
   *       'dedupe_field' => 'Email',
   *       'items' => array(
   *         array of items...
   *       ),
   *       'sobjects' => array(
   *         array of sobjects...
   *       ),
   *     );
   *   </code>
   *   Only UPSERT requires the 'dedupe_field' key.  For the 'DELETE' operation,
   *   instead of an 'sobjects' key, it requires an 'ids' key that is an array
   *   of Salesforce Ids of the items to delete.  The 'items' key is not
   *   required but recommended so that full information can be provided to
   *   modules that hook into the send processes.
   *
   * @return mixed
   *   Upon success, the Salesforce results are provided.  If there is an
   *   exception then it will return FALSE.
   */
  public function sendBatch($batch) {
    $this->sendBatchPrepare($batch);

    debug($batch['object_type']);
    try {
      switch ($batch['operation']) {
        case 'CREATE':
          // Whole sobject.
          $results = $this->soap->create($batch['sobjects']);
          $error_message = SALESFORCE_SYNC_ITEM_FAIL_CREATE;
          break;

        case 'UPSERT':
          // Whole sobject + dedupe field.
          $results = $this->soap->upsert($batch['dedupe_field'], $batch['sobjects']);
          $error_message = SALESFORCE_SYNC_ITEM_FAIL_UPSERT;
          break;

        case 'UPDATE':
          // Whole sobject that includes Id + fields +/or fields to null.
          $results = $this->soap->update($batch['sobjects']);
          $error_message = SALESFORCE_SYNC_ITEM_FAIL_UPDATE;
          break;

        case 'DELETE':
          // Require just Id of the object.
          $results = $this->soap->delete($batch['ids']);
          $error_message = SALESFORCE_SYNC_ITEM_FAIL_DELETE;
          break;

        case 'UNDELETE':
          // Require just Id of the object.
          $results = $this->soap->undelete($batch['ids']);
          $error_message = SALESFORCE_SYNC_ITEM_FAIL_UNDELETE;
          break;

        default:
          // Unsupported operation.
          throw new Exception('salesforce_sync does not support the operation "' . $batch['operation'] . '"');
      }

      // Process results.
      $items = isset($batch['items']) ? $batch['items'] : (isset($batch['sobjects']) ? $batch['sobjects'] : array());
      foreach ($results as $key => $result) {
        // Since we pass an item to others to trigger actions based on its
        // success, we should make sure we send the same type and structure of
        // item.
        if (isset($items[$key]) && isset($items[$key]->sobject)) {
          $item = $items[$key];
        }
        elseif (isset($items[$key])) {
          $sobject = $items[$key];
          $dedupe_field = isset($batch['dedupe_field']) ? $batch['dedupe_field'] : NULL;
          $item = new SalesforceSyncItem($sobject->type, $batch['operation'], array(), $dedupe_field);
          $item->sobject = $sobject;
        }
        else {
          $item = new SalesforceSyncItem();
          $item->sobject->Id = $result->Id;
        }
        if ($result->success) {
          $this->passItem($item, $result);
        }
        else {
          $this->failItem($item, $error_message, $result);
        }
      }
    }
    catch (Exception $error) {
      // Log message for later investigation.
      watchdog('salesforce_queue', 'Could not send batch !op: !message', array('!message' => $error->getMessage(), '!op' => $batch['operation']), WATCHDOG_DEBUG);
      $results = FALSE;
      $this->failBatch($batch, SALESFORCE_SYNC_ITEM_FAIL_BATCH, $error);
    }
    return $results;
  }

  /**
   * Wrapper for sendBatch in order to send a single item to Salesforce.
   *
   * @param object $item
   *   An individual representation of a record and instructions of how that
   *   record will be consumed by the recipient.
   *
   * @return mixed
   *   Results of sendBatch. Upon success, the Salesforce results are provided.
   *   If there is an exception then it will return FALSE.
   */
  public function sendItem($item) {
    $batch = array(
      'object_type' => $item->object_type,
      'operation' => $item->operation,
      'dedupe_field' => (isset($item->dedupe_field) ? $item->dedupe_field : NULL),
      'items' => array($item),
      'sobjects' => array($item->sobject),
    );
    if (isset($item->sobject->Id)) {
      $batch['ids'] = array($item->sobject->Id);
    }
    return $this->sendBatch($batch);
  }

  /**
   * Applies instructions on a batch in order to prepare it to be sent.
   *
   * @param array $batch
   *   An array of instructions to send to Salesforce.  The array is structured
   *   as follows:
   *   <code>
   *     $batch = array(
   *       'operation' => 'UPSERT',
   *       'dedupe_field' => 'Email',
   *       'items' => array(
   *         array of items...
   *       ),
   *       'sobjects' => array(
   *         array of sobjects...
   *       ),
   *     );
   *   </code>
   *   Only UPSERT requires the 'dedupe_field' key.  For the 'DELETE' operation,
   *   instead of an 'sobjects' key, it requires an 'ids' key that is an array
   *   of Salesforce Ids of the items to delete.  The 'items' key is not
   *   required but recommended so that full information can be provided to
   *   modules that hook into the send processes.
   */
  public function sendBatchPrepare(&$batch) {
    if (isset($batch['batch_prepared']) && $batch['batch_prepared']) {
      // No use continuing if this batch is already prepared.
      return;
    }
    $this->batchReplaceTokens($batch);
    $this->batchConvertToSalesforceFormat($batch);
    $this->batchApplySalesforceSyncRules($batch);
    $batch['batch_prepared'] = TRUE;
  }

  /**
   * Uses a SalesforceSyncTokenScanner instance to search for and replace 
   * relationship tokens in a batch of items.
   *
   * @param array $batch
   *   An array of instructions to send to Salesforce.  The array is structured
   *   as follows:
   *   <code>
   *     $batch = array(
   *       'operation' => 'UPSERT',
   *       'dedupe_field' => 'Email',
   *       'items' => array(
   *         array of items...
   *       ),
   *       'sobjects' => array(
   *         array of sobjects...
   *       ),
   *     );
   *   </code>
   */
  public function batchReplaceTokens(&$batch) {
    $this->tokenScanner->batchTokenReplace($batch);
    //throw new Exception("TODO: Implement token replacement routine", 1);
  }

  /**
   * Applys the sync rules to a batch of items with Salesforce as the consumer.
   *
   * @param array $batch
   *   An array of instructions to send to Salesforce.  The array is structured
   *   as follows:
   *   <code>
   *     $batch = array(
   *       'operation' => 'UPSERT',
   *       'dedupe_field' => 'Email',
   *       'items' => array(
   *         array of items...
   *       ),
   *       'sobjects' => array(
   *         array of sobjects...
   *       ),
   *     );
   *   </code>
   *   Only UPSERT requires the 'dedupe_field' key.  For the 'DELETE' operation,
   *   instead of an 'sobjects' key, it requires an 'ids' key that is an array
   *   of Salesforce Ids of the items to delete.  The 'items' key is not
   *   required but recommended so that full information can be provided to
   *   modules that hook into the send processes.
   */
  public function batchApplySalesforceSyncRules(&$batch) {
    if (isset($batch['sync_rules_applied']) && $batch['sync_rules_applied']) {
      // Skip because the sync rules have already been applied to this item.
      return;
    }
    else {
      // Declare we've checked this batch.
      $batch['sync_rules_applied'] = TRUE;
    }

    // Only required on UPSERT and UPDATE operations.
    $operation = $batch['operation'];
    if (!($operation != 'UPSERT' || $operation != 'UPDATE')) {
      return;
    }

    // Check to see if there are items to prepare and sync_rules to apply.
    if (!isset($batch['sync_rules']) || !isset($batch['items']) || empty($batch['items'])) {
      // Could not find items to prepare.
      return;
    }
    else {
      $items = &$batch['items'];
    }

    // Retreive records from Salesforce about the items in this batch.
    $object_type = $batch['object_type'];
    $sync_rules = $batch['sync_rules'];
    $sf_records = array();
    $upsert_ids = array();
    $dedupe_field = isset($batch['dedupe_field']) ? $batch['dedupe_field'] : NULL;
    if ($operation == 'UPDATE') {
      $records = $this->fetchByIds($object_type, $sync_rules['fields'], $sync_rules['ids']);
      foreach ($records as $record) {
        $sf_records[$record->Id] = $record->fields;
      }
    }
    elseif ($operation == 'UPSERT') {
      $records = $this->fetchByField($object_type, $sync_rules['fields'], $dedupe_field, $sync_rules['dedupe_values']);
      foreach ($records as $record) {
        $sf_records[$record->Id] = $record->fields;
        $upsert_ids[$record->fields->$dedupe_field] = $record->Id;
      }
    }

    // No point continuing if we didn't find anything.
    if (empty($sf_records)) {
      return;
    }

    // Go through the items and apply the sync rules based upon the information
    // just retreived from Salesforce.
    foreach ($items as $key => $item) {
      if (isset($item->sync_rules_applied) && $item->sync_rules_applied) {
        // Skip if the sync rules have already been applied to this item.
      }
      else {
        // Make sure item was actually found in Salesforce before continuing.
        $item_fields = &$items[$key]->sobject->fields;
        if ($operation == 'UPDATE') {
          $id = $item->sobject->Id;
        }
        else {
          $id = isset($upsert_ids["$item_fields[$dedupe_field]"]) ? $upsert_ids["$item_fields[$dedupe_field]"] : NULL;
        }
        if (is_string($id) && isset($sf_records[$id])) {
          $items[$key]->sobject->fields = $this->applySyncRules($item_fields, $sf_records[$id], $item->sync_rules);
        }
        $items[$key]->sync_rules_applied = TRUE;
      }
    }
  }

  /**
   * Applies the Salesforce format conversion to all items in the batch.
   *
   * @param array $batch
   *   An array of instructions to send to Salesforce.  The array is structured
   *   as follows:
   *   <code>
   *     $batch = array(
   *       'operation' => 'UPSERT',
   *       'dedupe_field' => 'Email',
   *       'items' => array(
   *         array of items...
   *       ),
   *       'sobjects' => array(
   *         array of sobjects...
   *       ),
   *     );
   *   </code>
   *   Only UPSERT requires the 'dedupe_field' key.  For the 'DELETE' operation,
   *   instead of an 'sobjects' key, it requires an 'ids' key that is an array
   *   of Salesforce Ids of the items to delete.  The 'items' key is not
   *   required but recommended so that full information can be provided to
   *   modules that hook into the send processes.
   */
  public function batchConvertToSalesforceFormat(&$batch) {
    if (isset($batch['converted_to_salesforce_format']) && $batch['converted_to_salesforce_format']) {
      // Skip because this batch has already had its items formats converted.
      return;
    }

    // Convert each item.
    foreach ($batch['items'] as $item) {
      $this->convertToSalesforceFormat($item);
    }

    // Mark the batch as converted.
    $batch['converted_to_salesforce_format'] = TRUE;
  }

  /**
   * Convert the mapped values into a format compatible to send to Salesforce.
   *
   * @param object $item
   *   An individual representation of a record and instructions of how that
   *   record will be consumed by the recipient.
   */
  public function convertToSalesforceFormat(&$item) {
    if (isset($item->converted_to_salesforce_format) && $item->converted_to_salesforce_format) {
      // Skip because this item has already had its formats converted.
      return;
    }

    // Find the data types of all the Salesforce fields and store it in a means
    // so that we can attempt to reduce the number of times we have to search
    // for this information.
    $sfapi = salesforce_get_api();
    $sfobject = $sfapi->objectDescribe($item->object_type);
    $field_datatypes = &drupal_static(__FUNCTION__);
    $field_missing = FALSE;
    foreach ($item->sobject->fields as $field_name => $value) {
      if (!isset($field_datatypes) || !isset($field_datatypes[$field_name])) {
        $field_missing = TRUE;
      }
    }
    $count = count($item->sobject->fields);
    if (!isset($field_datatypes) || $field_missing) {
      foreach ($sfobject['fields'] as $field) {
        if (array_key_exists($field['name'], $item->sobject->fields)) {
          $field_datatypes[$field['name']] = $field['type'];
          $count--;
          if ($count == 0) {
            break;
          }
        }
      }
    }

    // Perform the actual conversion.
    foreach ($item->sobject->fields as $field_name => $value) {
      switch ($field_datatypes[$field_name]) {
        case 'datetime':
          // Salesforce requires ISO 8601 format for dates.
          $item->sobject->fields[$field_name] = date('c', strtotime($value));
          break;

        default:
          // Do nothing.
      }
    }

    // Mark the item as converted.
    $item->converted_to_salesforce_format = TRUE;
  }

  /**
   * Helper to remove newly created items from Salesforce. Used for simpletests.
   */
  public function deleteNewSfRecords() {
    if (empty($this->newSfRecords)) {
      return;
    }
    $chunks = array_chunk($this->newSfRecords, $this->maxBatchSize);
    $this->newSfRecords = array();
    foreach ($chunks as $items) {
      $ids = array();
      foreach ($items as $key => $item) {
        $items[$key]->operation = 'DELETE';
        $ids[] = $item->sobject->Id;
      }
      $batch = array(
        'operation' => 'DELETE',
        'items' => $items,
        'ids' => $ids,
      );
      $this->sendBatch($batch);
    }
  }

  /**
   * @} salesforce_sync_drupal_to_salesforce
   */


  /**
   * @name salesforce_sync_salesforce_to_drupal Salesforce to Drupal methods
   * @{
   */

  /**
   * Import a bundled batch of instructions from Salesforce.
   *
   * @param array $batch
   *   It is unknown at this time if Drupal batches will be formatted
   *   differently from the Salesforce batches.
   */
  public function importBatch($batch) {
    $this->importBatchPrepare($batch);
    // @TODO
  }

  /**
   * Wrapper for importBatch in order to import a single item from Salesforce.
   *
   * @param object $item
   *   It is unknown at this time if Drupal items will be formatted differently
   *   from the Salesforce items.
   */
  public function importItem($item) {
    $batch = array(
      'items' => array($item),
    );
    // @TODO
    $this->importBatch($batch);
  }

  /**
   * Applies instructions on a batch in order to prepare it to be imported.
   *
   * @param array $batch
   *   It is unknown at this time if Drupal batches will be formatted
   *   differently from the Salesforce batches.
   */
  public function importBatchPrepare($batch) {
    if (isset($batch['batch_prepared']) && $batch['batch_prepared']) {
      // No use continuing if this batch is already prepared.
      return;
    }
    $this->batchConvertToDrupalFormat($batch);
    $this->batchApplyDrupalSyncRules($batch);
    $batch['batch_prepared'] = TRUE;
  }

  /**
   * Applys the sync rules to a batch of items with Drupal as the consumer.
   *
   * @param array $batch
   *   It is unknown at this time if Drupal batches will be formatted
   *   differently from the Salesforce batches.
   */
  public function batchApplyDrupalSyncRules($batch) {
    if (isset($batch['sync_rules_applied']) && $batch['sync_rules_applied']) {
      // Skip because the sync rules have already been applied to this item.
      return;
    }
    else {
      // Declare we're checking this batch.
      $batch['sync_rules_applied'] = TRUE;
    }
    // @TODO
  }

  /**
   * Applies the Drupal format conversion to all items in the batch.
   *
   * @param array $batch
   *   It is unknown at this time if Drupal batches will be formatted
   *   differently from the Salesforce batches.
   */
  public function batchConvertToDrupalFormat($batch) {
    if (isset($batch['converted_to_drupal_format']) && $batch['converted_to_drupal_format']) {
      // Skip because this batch has already had its items formats converted.
      return;
    }

    // @TODO: Convert each item.
    // Mark the batch as converted.
    $batch['converted_to_salesforce_format'] = TRUE;
  }

  /**
   * Convert the mapped values into a format compatible to import into Drupal.
   *
   * @param object $item
   *   It is unknown at this time if Drupal items will be formatted differently
   *   from the Salesforce items.
   */
  public function convertToDrupalFormat($item) {
    if (isset($item->converted_to_drupal_format) && $item->converted_to_drupal_format) {
      // Skip because this item has already had its formats converted.
      return;
    }

    // @TODO: Convert each field.
    // Mark the item as converted.
    $item->converted_to_drupal_format = TRUE;
  }

  /**
   * @} salesforce_sync_salesforce_to_drupal
   */


  /**
   * @name salesforce_sync_results Result processing
   * @{
   */

  /**
   * Marks an item has having succeeded processing.
   *
   * @param object $item
   *   Item to be processed.
   * @param object $result
   *   Result of the item's processing from Salesforce.
   */
  public function passItem($item, $result) {
    // Keep a log of newly created items.
    // TODO: Include updates
    if ($item->operation == 'CREATE' || $item->operation == 'UPSERT') {
      $item->sobject->Id = $result->id;
      $this->newSfRecords[] = $item;
      if (isset($item->module) && isset($item->delta) && isset($item->drupal_id)) {
        $id_key = $item->module . '-' . $item->delta . '-' . $item->drupal_id;
        $this->processedItemIds[$item->object_type][$id_key] = $result->id;
        // Pass the item to the token scanner's internal cache so it doesn't have
        // to go to the db as often.
        $this->tokenScanner->addItemToTokenCache($item, $result->id);
      }
    }

    // Invokes hook_salesforce_sync_pass_item().
    module_invoke_all('salesforce_sync_pass_item', $item, $result);
  }

  /**
   * Marks an item has having failed.
   *
   * @param object $item
   *   Item to be processed.
   * @param string $message
   *   Error message.
   * @param object $result
   *   Salesforce result if the item had received a result from salesforce, or
   *   exception caught when call failed.
   */
  public function failItem($item, $message, $result = NULL) {
    // Invokes hook_salesforce_sync_fail_item().
    module_invoke_all('salesforce_sync_fail_item', $item, $message, $result);
  }

  /**
   * Wrapper for failItem() so that every item in a batch is failed.
   *
   * @param array $batch
   *   Batch array as set up for processBatch() for execute
   * @param Exception $result
   *   Error caught when the call to Salesforce failed.
   */
  public function failBatch($batch, $message, $result = NULL) {
    foreach ($batch['items'] as $item) {
      $this->failItem($item, $message, $result);
    }

    // Invokes hook_salesforce_sync_fail_batch().
    module_invoke_all('salesforce_sync_fail_batch', $batch, $message, $result);
  }

  /**
   * @} salesforce_sync_results
   */


  /**
   * @name salesforce_sync_fetch Helpers to fetch records from Salesforce
   * @{
   */

  /**
   * Helper method to fetch records from Salesforce based upon provided Ids.
   *
   * @param string $object_type
   *   The type of object of the desired records, such as 'Account' or 'User'.
   * @param array $fields
   *   Array of field names of the records to return.
   * @param array $ids
   *   Array of Salesforce Ids of the desired records.
   *
   * @return mixed
   *   If records are found, an array of Salesforce objects for reach found
   *   record is returned.  Otherwise, receive FALSE.
   */
  public function fetchByIds($object_type, $fields, $ids) {
    if (empty($ids)) {
      // If there are no Ids to fetch, there is no point wasting an API call.
      return array();
    }

    // Salesforce expects a comma deliminated string of fields.
    $fields = implode(', ', $fields);

    // Ids need to be an unassociated array or Salesforce will reject it.
    $ids = array_values($ids);

    try {
      $response = $this->soap->retrieve($fields, $object_type, $ids);
    }
    catch (Exception $error) {
      // Log message for later investigation.
      watchdog('salesforce_queue', 'Could not fetch records by Ids: !message', array('!message' => $error->getMessage()), WATCHDOG_DEBUG);
      $response = array();
    }
    return $response;
  }

  /**
   * Helper method to fetch records from Salesforce based on values of a field.
   *
   * @param array $fields
   *   Array of field names of the records to return.
   * @param string $key_field
   *   Field name of the field to query on.
   * @param array $key_field_values
   *   Array of values of the $key_field the searched for records can have.
   *
   * @return array
   *   Array of Salesforce objects for reach found record.
   */
  public function fetchByField($object_type, $fields, $key_field, $key_field_values) {
    $key_field_values_string = "'" . implode("', '", $key_field_values) . "'";
    $query_part_1 = "SELECT Id, $key_field FROM $object_type WHERE $key_field IN ";
    $query = $query_part_1 . "($key_field_values_string)";

    // Verify the query string does not exceed maxQueryLength.  If it does,
    // chunk up the $key_field_values until it does not.  Salesforce cannot
    // handle it when the query is too long.
    $queries = array($query);
    if (strlen($query) > $this->maxQueryLength) {
      $count = count($key_field_values);
      $split = 2;
      while (strlen($query) > $this->maxQueryLength) {
        $chunk_size = $count / $split;
        $chunks = array_chunk($key_field_values, $chunk_size);
        $chunk = array_shift($chunks);
        $key_field_values_string = "'" . implode("', '", $chunk) . "'";
        $query = $query_part_1 . "($key_field_values_string)";
        $split++;
      }
      $queries = array($query);
      foreach ($chunks as $chunk) {
        $key_field_values_string = "'" . implode("', '", $chunk) . "'";
        $queries[] = $query_part_1 . "($key_field_values_string)";
      }
    }

    // Partner API requests are formatted incorrectly, so we are reciving the Id
    // as an array, and the fields are not retreived in a structured format.
    // Because of this, we are grabbing only the Ids from this query, then
    // passing them on to fetchByIds to complete the request.  Once the bug is
    // fixed, remove this work around and retreive the records directly from
    // this query.
    // @see http://goo.gl/Xv8k7
    $ids = array();
    foreach ($queries as $query) {
      try {
        $response = $this->soap->query($query);
        if ($response->size > 0) {
          foreach ($response->records as $record) {
            $ids[] = $record->Id[0];
          }
        }
      }
      catch (Exception $error) {
        // Log message for later investigation.
        watchdog('salesforce_queue', 'Could not fetch records by fields: !message', array('!message' => $error->getMessage()), WATCHDOG_DEBUG);
      }
    }

    return $this->fetchByIds($object_type, $fields, $ids);
  }

  /**
   * @}
   */


  /**
   * Helper to get all records created by this SalesforceSync.
   *
   * Implemented to mimic version in SalesforceQueueProcessor.
   *
   * @return array
   *   Records that have been created.
   */
  public function getNewSfRecords() {
    return $this->newSfRecords;
  }

  /**
   * Helper to get all item Ids processed by this SalesforceSync.
   *
   * Implemented to mimic version in SalesforceQueueProcessor.
   *
   * @return array
   *   Ids of items that have been processed keyed by their unique identifier.
   */
  public function getProcessedItemIds() {
    return $this->processedItemIds;
  }

  /**
   * Helper to externally set the Salesforce API. Used for simpletests.
   */
  public function setSalesforceApi(Salesforce $sfapi) {
    $this->sfapi = $sfapi;
  }

  /**
   * Helper to externally set the Salesforce SOAP Partner. Used for simpletests.
   */
  public function setSalesforceSoap($soap) {
    $this->soap = $soap;
  }
}
