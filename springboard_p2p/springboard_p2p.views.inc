<?php
/**
 * @file
 * Provides Springboard P2P data and relationships to Views.
 */


/**
 * Implements hook_views_data().
 */
function springboard_p2p_views_data() {
  $data = array();

  $data['springboard_p2p_private_campaign_approval'] = array(
    'table' => array(
      'group' => t('P2P Private Campaign Approvals'),
      'base' => array(
        'field' => 'nid',
        'title' => t('P2P private campaign approvals'),
        'help' => t('Users waiting for approval or approvaed to create content in a private P2P campaign.'),
        'weight' => -10,
      ),
      'join' => array(
        'node' => array(
          'left_field' => 'nid',
          'field' => 'nid',
        ),
        'users' => array(
          'left_field' => 'uid',
          'field' => 'uid',
        ),
      ),

    ),
    'nid' => array(
      'title' => t('Private P2P campaign'),
      'help' => t('The private P2P campaign.'),
      'relationship' => array(
        'base' => 'node',
        'base field' => 'nid',
        'handler' => 'views_handler_relationship',
        'label' => t('Private P2P campaign'),
        'title' => t('Private P2P campaign'),
        'help' => t('The private P2P campaign.'),
      ),
    ),
    'uid' => array(
      'title' => t('User requesting approval'),
      'help' => t('User requesting approval.'),
      'relationship' => array(
        'base' => 'users',
        'base field' => 'uid',
        'handler' => 'views_handler_relationship',
        'label' => t('User requesting approval'),
        'title' => t('User requesting approval.'),
        'help' => t('User requesting approval to the private campaign.'),
      ),
    ),
    'status' => array(
      'title' => t('Approval status'),
      'help' => t('The state of the approval request.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),
    'created' => array(
      'title' => t('Requested time'),
      'help' => t('The time that the user requested approval.'),
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
    ),

  );

  return $data;
}
