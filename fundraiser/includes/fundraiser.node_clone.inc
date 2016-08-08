<?php
/**
 * @file
 * Fundraiser functions related to the node_clone module.
 */

/**
 * Implements hook_clone_access_alter().
 */
function fundraiser_clone_access_alter(&$access, $node) {
  if (fundraiser_is_donation_type($node->type)) {
    // Remove the access if the user isn't allowed to clone the donation form.
    if (!user_access('clone donation form')) {
      $access = 0;
    }
    if (user_access('clone donation form')) {
      $access = 1;
    }
  }
}

/**
 * Implements hook_clone_node_alter().
 *
 * From: http://drupal.org/node/1256478
 */
function fundraiser_clone_node_alter(&$node, $context) {
  // Set the internal name.
  if (fundraiser_is_donation_type($context['original_node']->type)) {
    if (!empty($context['original_node']->field_fundraiser_internal_name[$context['original_node']->language][0]['value'])) {
      $internal_name = t('Clone of @internal', array('@internal' => $context['original_node']->field_fundraiser_internal_name[$context['original_node']->language][0]['value']));
      $node->field_fundraiser_internal_name  = array($node->language => array(0 => array('value' => $internal_name)));
    }
  }
  // The additional load in hook_node_load already will have brought in
  // additional node data.
  // The additional saves in hook_node_insert / hook_node_update will save the
  // additional node data.
}
