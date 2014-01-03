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
  $submission = (array) $submission; // typecast in case the submission was encoded as an object.
  // TODO: find a long term solution here. We have to have the exact text of the submission button on
  // the form when submitting. This value is added to the form by a post process function or after build,
  // making it potentially expensive to go through all the steps required to generate the full form
  // programmatically so we can then crawl the form api array looking for the submit button title.
  if (module_exists('fundraiser') && fundraiser_is_donation_type($node->type)) {
    $submit_text = t('Donate');
  }
  else {
    $submit_text = t('Submit');
  }
  if (is_numeric($nid) && is_array($submission)) {
    $form_id = 'webform_client_form_' . $nid;
    $node = node_load($nid);
    $form_state['webform_completed'] = 1;
    $form_state['values'] = array(
      'submit' => $submit_text,
      'op' => $submit_text,
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
    drupal_form_submit($form_id, $form_state, $node, $webform_submission);
    // taken from node_resources.inc, if errors are encountered during form submission we
    // return a 406 HTTP response code and any errors encountered.
    if ($errors = form_get_errors()) {
      return services_error(implode(" ", $errors), 406, array('form_errors' => $errors));
    }
    else {
      return t('Submission successful');
    }
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
