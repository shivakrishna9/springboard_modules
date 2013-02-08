<?php

/**
 * @file
 * Contains SalesforceQueueTokenScanner
 */

/**
 * Functionality for finding and replacing Salesforce relationship tokens.
 */
class SalesforceQueueTokenScanner {

  /**
   * An array of processed Salesforce items.
   *
   * @var array
   */
  protected $tokenValueCache;

  /**
   * Setter for tokenValueCache property.
   *
   * @param array $token_value_cache
   *   An array of Salesforce ids to use for token replacement.
   */
  public function setTokenValueCache($token_value_cache) {
    $this->tokenValueCache = $token_value_cache;
  }

  /**
   * Construct the object.
   *
   * @param array $token_value_cache
   *   An array of Salesforce ids to use for token replacement.
   */
  public function __construct($token_value_cache) {
    $this->tokenValueCache = $token_value_cache;
  }

  /**
   * Replaces tokens in a batch of Salesforce objects.
   *
   * @param array $batch
   *   A populated queue processor batch.
   */
  public function batchTokenReplace(&$batch) {
    foreach ($batch['sobjects'] as &$object) {
      $this->objectTokenReplace($object);
    }
  }

  /**
   * Replaces tokens in a single Salesforce object.
   *
   * @param object $object
   *   The object record whose fields to scan for tokens.
   */
  public function objectTokenReplace(&$object) {
    // See if the object has any tokens in it.
    $tokens = $this->findTokens($object);

    // If we've got tokens, replace them.
    if (!empty($tokens)) {
      $this->tokenReplace($object, $tokens);
    }
  }

  /**
   * Replace each tokenized field SFID from tokenValueCache or database.
   *
   * @param object $object
   *   The object whose tokenized field values need to be replaced.
   * @param array $tokens
   *   An array of tokenized fields.
   */
  public function tokenReplace(&$object, $tokens) {
    foreach ($tokens as $field => $token) {
      // If the token cache is populated, attempt to get value from cache.
      if ($this->cachePopulated() && $value = $this->getTokenValueFromCache($token)) {
        $object->fields[$field] = $value;
      }
      elseif ($value = $this->getTokenValueFromDB($token)) {
        $object->fields[$field] = $value;
      }
      else {
        // What do we do here? No token found, bummer! This will cause the
        // object to fail.
        $object->fields[$field] = $token;
      }
    }
  }

  /**
   * Replaces a token with its value from a cache of values.
   *
   * @param string $token
   *   The token to replace.
   */
  protected function getTokenValueFromCache($token) {
    // Split token into something useful for cache lookup.
    $token = $this->splitToken($token);
    // TODO: Optimize this! isset() perhaps?
    if (array_key_exists($token['sf_type'], $this->tokenValueCache)
      && array_key_exists($token['drupal_type'], $this->tokenValueCache[$token['sf_type']])
      && array_key_exists($token['drupal_id'], $this->tokenValueCache[$token['sf_type']][$token['drupal_type']])) {

      return $this->tokenValueCache[$token['sf_type']][$token['drupal_type']][$token['drupal_id']];
    }

    return FALSE;
  }

  /**
   * Replaces a token with its value from the database.
   *
   * @param string $token
   *   The token to replace.
   */
  protected function getTokenValueFromDB($token) {
    // Split token into something useful for database lookup.
    $token = $this->splitToken($token);

    // Look for Id in object mapping db2_tables(connection)
    $sfid = db_select('salesforce_genmap_record_map', 'm')
      ->fields('m', array('sfid'))
      ->condition('salesforce_type', $token['sf_type'])
      ->condition('module', $token['drupal_type'])
      ->condition('drupal_id', $token['drupal_id'])
      ->execute()
      ->fetchField();

    return !empty($sfid) ? $sfid : FALSE;
  }

  /**
   * Splits a token into it's 3 respective parts.
   *
   * @param string $token
   *   The token to split.
   */
  public function splitToken($token) {
    // Remove [ and ]
    $token = substr($token, 1, -1);
    $token_pieces = explode(':', $token);

    // Convert to something more friendly.
    return array(
      'sf_type' => $token_pieces[0],
      'drupal_type' => $token_pieces[1],
      'drupal_id' => $token_pieces[2],
    );
  }

  /**
   * Searches for tokens in an sobject's fields array.
   *
   * @param object $object
   *   A Salesforce sobject.
   */
  public function findTokens($object) {
    return preg_grep('/
      \[             # [ - pattern start
      ([^\s\[\]:]*)  # match salesforce type not containing whitespace : [ or ]
      :              # : - separator
      ([^\s\[\]:]*)  # match module not containing whitespace : [ or ]
      :              # : - separator
      ([^\s\[\]:]*)   # match module not containing whitespace : [ or ]
      \]             # ] - pattern end
      /x', $object->fields);
  }

  /**
   * Check if the cache is populated.
   */
  protected function cachePopulated() {
    return !empty($this->tokenValueCache);
  }

}
