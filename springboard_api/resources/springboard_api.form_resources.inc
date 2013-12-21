<?php

/**
 * @file
 * Service endpoint callbacks & access functions.
 */


/**
 *
 *
 */
function springboard_api_form_resource_retrieve($nid, $op = NULL) {

  if ($op) {
    return springboard_api_form_action_render($nid);
  }
  $form_details = springboard_api_get_form_details($nid);
  if ($form_details) {
    return $form_details;
  }
  else {
    return services_error("The requested form does not exist.", 404);
  }
}

/**
 *
 *
 */
function springboard_api_form_resource_index() {

  $form_types = variable_get('webform_node_types', array());
  $form_select = db_select('node', 'n');
  $form_select->fields('n', array('nid', 'type', 'title'));
  $form_select->leftJoin('field_data_field_fundraiser_internal_name', 'f', 'n.nid = f.entity_id');
  $form_select->addExpression("COALESCE(f.field_fundraiser_internal_name_value, '')",'internal_name');
  $form_select->condition('type', $form_types, 'in');
  $results = $form_select->execute();
  return $results->fetchAll();

}

/**
 *
 *
 */
function springboard_api_form_action_submit($nid, $submission) {
  if (is_numeric($nid) && is_array($submission)) {
    $form_id = 'webform_client_form_' . $nid;
    $node = node_load($nid);
    $form_state['webform_completed'] = 1;
    $form_state['values'] = array(
      'submit' => t('Donate'),
      'op' => t('Donate'),
      'details' => array(
        'nid' => $nid,
        'sid' =>  '',
        'uid' => 0, // fundraiser should figure this out by email address.
        'page_num' => 1,
        'page_count' => 1,
        'finished' => 0,
      ),
    );
    foreach ($node->webform['components'] as $cid => $component) {
      if ($component['type'] == 'payment_method') {
        $field_info = _springboard_api_get_payment_field_info($component);

      }
      elseif ($component['type'] != 'fieldset') {
        $fields[] = $component['form_key'];
        $form_state['values']['submitted'][$cid] = isset($submission[$component['form_key']]) ? $submission[$component['form_key']] : '';
      }
    }
    return $form_state['values']['submitted'];
    return $fields;
  }
}

function springboard_api_form_action_render() {
  $args = func_get_args();
  $nid = $args[0];
  $node = node_load($nid);
  $submission = array();
  $wrapper = isset($args[1]) ? $args[1] : FALSE;

  $form = drupal_get_form('webform_client_form_' . $nid, $node, $submission, $is_draft = FALSE, $filter = TRUE);
  return render($form);
}