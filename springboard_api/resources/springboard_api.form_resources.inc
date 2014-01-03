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
function springboard_api_form_action_submit($nid, $submission, $op = NULL) {
  $submission = (array) $submission; // typecast in case the submission was encoded as an object.
  if (!$op) {
    $op = t('Donate');
  }
  if (is_numeric($nid) && is_array($submission)) {
    $form_id = 'webform_client_form_' . $nid;
    $node = node_load($nid);
    $form_state['webform_completed'] = 1;
    $form_state['values'] = array(
      'submit' => $op,
      'op' => $op,
      'details' => array(
        'nid' => $nid,
        'sid' =>  '',
        'uid' => 0, // fundraiser should figure this out by email address.
        'page_num' => 1,
        'page_count' => 1,
        'finished' => 0,
      ),
      'values' => array(),
    );
    $form_state['values']['submitted'] = springboard_api_submission_tree_build($submission, $node->webform['components'], $tree, 0);
    $webform_submission = array(
      'nid' => $nid,
      'uid' => 0, // assume anonymous, webform user should pick up the slack.
      'submitted' => time(),
      'remote_addr' => $_SERVER['REMOTE_ADDR'],
      'data' => array(),
    );
    foreach ($node->webform['components'] as $cid => $component) {
      if ($component['type'] != 'fieldset') {
        $fields[] = $component['form_key'];
      //  if ($component['type'] == 'payment_fields') {
      //    $form_state['values']['submitted']['payment_information']['payment_fields'] = isset($submission[$component['form_key']]) ? $submission[$component['form_key']] : '';
      //  }
      //  else {
      //    $form_state['values']['submitted'][$cid] = isset($submission[$component['form_key']]) ? $submission[$component['form_key']] : '';
      //  }
        if (isset($submission[$component['form_key']]) && !is_array($submission[$component['form_key']])) {
          $webform_submission['data'][$cid]['value'][] =  $submission[$component['form_key']];
        }
        elseif (isset($submission[$component['form_key']])) {
          $webform_submission['data'][$cid]['value'] = $submission[$component['form_key']];
        }
        else {
          // backfill with null data to match what webform does.
          $webform_submission['data'][$cid]['value'][] = '';
        }
      }
    }
    dsm($webform_submission, 'webform_submission');
    dsm($form_state, 'form_state');
    drupal_form_submit($form_id, $form_state, $node, $webform_submission);
    return $_SESSION['messages'];
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

function springboard_api_submission_tree_build($submission, $src, &$tree, $parent) {

  foreach ($src as $cid => $component) {
    if ($component['pid']) {
      $parent_key = $src[$component['pid']]['form_key'];
      if (!isset($tree[$parent_key])) {
        $tree[$parent_key] = array();
      }
      $tree[$parent_key][$component['form_key']] = isset($submission[$component['form_key']]) ? $submission[$component['form_key']] : NULL;
    }
    else {
      $tree[$component['form_key']] = isset($submission[$component['form_key']]) ? $submission[$component['form_key']] : NULL;
    }

  }
  return $tree;
}
