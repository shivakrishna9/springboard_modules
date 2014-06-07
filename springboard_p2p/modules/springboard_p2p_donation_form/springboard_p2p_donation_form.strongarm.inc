<?php
/**
 * @file
 * springboard_p2p_donation_form.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function springboard_p2p_donation_form_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_options_p2p_donation_form';
  $strongarm->value = array(
    0 => 'main-menu',
  );
  $export['menu_options_p2p_donation_form'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_parent_p2p_donation_form';
  $strongarm->value = 'main-menu:0';
  $export['menu_parent_p2p_donation_form'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_p2p_donation_form';
  $strongarm->value = array(
    0 => 'status',
  );
  $export['node_options_p2p_donation_form'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_preview_p2p_donation_form';
  $strongarm->value = '0';
  $export['node_preview_p2p_donation_form'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_submitted_p2p_donation_form';
  $strongarm->value = 0;
  $export['node_submitted_p2p_donation_form'] = $strongarm;

  return $export;
}
