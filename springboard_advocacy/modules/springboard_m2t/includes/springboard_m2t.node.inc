<?php

/**
 * @file
 * node functions for Springboard M2T.
 */

/**
 * Implements hook_node_insert
 */
function springboard_m2t_node_insert($node) {
  if($node->type == 'springboard_m2t') {
    module_load_include('inc', 'webform', 'includes/webform.components');
    
    //add custom fields for the messages
    $fields[] = array(
      'nid' => $node->nid,
      'form_key' => 'springboard_m2t_phone',
      'pid' => 0,
      'name' => t('Phone Number'),
      'type' => 'textfield',
      'mandatory' => 1,
      'weight' => 13,
      'email' => 1,
      'extra' => array(
      'description' => t(''),
      ),
    );

    $fields[] = array(
        'nid' => $node->nid,
        'form_key' => 'springboard_m2t_message',
        'pid' => 0,
        'name' => t('Message'),
        'type' => 'textarea',
        'mandatory' => 1,
        'weight' => 18,
        'email' => 1,
        'extra' => array(
        'description' => t('Customize your message'),
      ),
    );
    //limit the sate field to US states
    $fields[] = array(
        'nid' => $node->nid,
        'form_key' => 'sbp_state',
        'pid' => 0,
        'name' => t('State'),
        'type' => 'select',
        'mandatory' => 1,
        'weight' => 12,
        'email' => 1,
        'extra' => array(
        'options_source' => 'united_states', //may have to remove some.
        'aslist' => 'Y',
        'multiple' => 0,
      ),
    );
    $fields[] = array(
        'nid' => $node->nid,
        'form_key' => 'sbp_zip',
        'pid' => 0,
        'name' => t('Zip Code'),
        'type' => 'textfield',
        'mandatory' => 1,
        'weight' => 13,
        'email' => 1,
        'extra' => array(
        'maxlength' => 5,
      ),
    );
    // Add the component to the Webform.
    foreach ($fields as $field) {
      //dpm($field);
      $cid = webform_component_insert($field);
      //user map the state field
      if ($field['form_key'] == 'sbp_state' || $field['form_key'] == 'sbp_zip') {
            $map = array(
            'nid' => $node->nid,
            'cid' => $cid,
            'map_id' => $field['form_key'],
          );
          drupal_write_record('webform_user_component_map', $map);
      }
    }
  }
}

/**
 * Implements hook_form_alter
 */
function springboard_m2t_form_alter(&$form, &$form_state, $form_id) {
  if(isset($form['#node']) && $form['#node']->type == 'springboard_m2t') {
    $pos = strpos($form_id, 'client_form');
    if($pos !== FALSE) {
      $form['#submit'][] = 'springboard_m2t_submit';
      $form['#attributes']['class'][] = 'geocode-form';
      $form['#attributes']['class'][] = 'geocode-form';
      // if (variable_get('springboard_m2t_geocoder', '') == 'googlej') {
      //   $form['#attached']['js'][]  = array('data' => 'https://maps.googleapis.com/maps/api/js?sensor=false', 'type' => 'external');
      // }
      $form['#attached']['js'][] = array('data' => drupal_get_path('module', 'springboard_m2t') . '/js/m2t.js'); 
      $form['#attached']['js'][] = array('data' => drupal_get_path('module', 'fundraiser') . '/js/jquery.validate.min.js'); 
      $form['#attached']['js'][] = array(
        'data' => array(
          'springboard_m2t' => array(
              'geocoder' => variable_get('springboard_m2t_geocoder', ''),
            ),
          ),
        'type' => 'setting',
      );
    }
  }
}

/**
 * Submission hook will contain logic for knowwho lookups, perhaps.
 */
function springboard_m2t_submit($form, $form_state) {

}
