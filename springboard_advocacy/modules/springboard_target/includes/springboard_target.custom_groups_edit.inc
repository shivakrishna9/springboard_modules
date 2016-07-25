<?php
/**
 * @file
 * Functions for target updates.
 */

/**
 * Group edit form.
 *
 * Add edit form items to the generic form defined
 * in springboard_target.custom_groups.
 *
 * @param $form
 * @param $form_state
 * @param $group_id
 *
 * @return array
 *   The edit form.
 */
function springboard_target_edit_custom_groups_form($form, &$form_state, $group_id) {

  module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_groups');

  $form = springboard_target_group_form($group_id);
  
  if (!isset($form['not_found'])) {

    $form['header'] = array(
      '#prefix' => '<h2>',
      '#suffix' => '</h2>',
      '#markup' => check_plain($form['row_one']['group_name']['#default_value']),
      '#weight' => '-100' ,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#weight' => 10,
      '#attributes' => array('class' => 'group-add'),
    );
  }
  $form['#validate'][] = 'springboard_target_group_form_validate';

  return $form;

}

/**
 * Submit function for group edit form.
 *
 * @param $form
 * @param $form_state
 */
function springboard_target_edit_custom_groups_form_submit($form, &$form_state) {
  $id = $form_state['build_info']['args'][0];
  $input = $form_state['values'];
  $api_call = springboard_advocacy_api_call();
  /* @TODO ensure $input follows the following format:
   *
   * 'name' => 'group name', //required
   *    'is_template' => 0, //required boolean @TODO
   *    'chambers' => array(
   *         0 => array('chamber' => 'fs', 'party =>'r', state => 'MA'),
   *         1 => array('chamber' => 'fs', 'party =>'d', state => 'MA')
   *       )
   *    'executives' => array(
   *         0 => array('role' => 'PRES01', 'party =>'r'),
   *         1 => array('role' => 'PRES03', 'party =>'d')
   *       )
   *     )
   *     'target_ids' => array('1', '2', '3', '4', '5', '6', '7'),
   *
   *     'message' => array(
   *       group_id =>
   *       message_id =>
   *       form_id =>
   *      ),
   *   )
   */
  $group = $api_call->invokeClientMethod('updateTargetGroup', $input, $id);

  if (!empty($group->data)) {
    $message = implode(' ', (array) $group->data->status);
    drupal_set_message($message);
    $form_state['redirect'] = 'admin/springboard/advocacy/targets/custom-groups/';
  }
}
