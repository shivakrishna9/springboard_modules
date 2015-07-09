<?php

/**
 * @file
 * The Views controller class for the Fundraiser Sustainer record entity.
 */

class SalesforceNgocViewsController extends EntityDefaultViewsController {

  /**
   * Defines the result for hook_views_data().
   *
   * For the Fundraiser Sustainer record entity. This allows filtering on
   * gateway response using "is null" and "is not null".
   *
   * This also adds a join on fundraiser_donation, which I copied from the
   * original Springboard Views implementation of hook_views_data for
   * the sustainers table.
   *
   * @return array
   *   Data for Views.
   */
  public function views_data() {
    $data = parent::views_data();
    $table = $this->info['base table'];

    // Set the date fields to use date handlers.
    $data[$table]['created'] = array(
      'title' => t('Created'),
      'help' => t('The date the item was created.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_date',
      ),
    );

    $data[$table]['changed'] = array(
      'title' => t('Changed'),
      'help' => t('The date the item was last changed.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_date',
      ),
    );

    /**
     * Table definitions.
     */
    $data['salesforce_ngoc_batch_upload_objects']['table']['group'] = $data[$table]['table']['group'];
    $data['salesforce_ngoc_batch_upload_objects']['table']['join'] = array(
      $table => array(
        'left_field' => 'buid',
        'field' => 'buid',
        'type' => 'LEFT',
      ),
    );

    // Drupal ID.
    $data['salesforce_ngoc_batch_upload_objects']['drupal_id'] = array(
      'title' => t('Drupal ID'),
      'help' => t('The id of the drupal object.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'title' => t('Remote address'),
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Drupal ID.
    $data['salesforce_ngoc_batch_upload_objects']['module'] = array(
      'title' => t('Module'),
      'help' => t('The Drupal module for managing the object.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'title' => t('Remote address'),
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Drupal ID.
    $data['salesforce_ngoc_batch_upload_objects']['delta'] = array(
      'title' => t('Delta'),
      'help' => t('The Drupal module delta for managing the object.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'title' => t('Remote address'),
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Salesforce ID.
    $data['salesforce_ngoc_batch_upload_objects']['sfid'] = array(
      'title' => t('Created Object SFID'),
      'help' => t('The SFID of the object created by the Batch Upload Object.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'title' => t('Remote address'),
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    // Salesforce Object Type.
    $data['salesforce_ngoc_batch_upload_objects']['object_type'] = array(
      'title' => t('Created Object Type'),
      'help' => t('The type of object created by the Batch Upload Object.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'title' => t('Remote address'),
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );

    return $data;
  }
}
