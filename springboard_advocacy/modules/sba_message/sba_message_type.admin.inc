<?php

/**
 * @file
 * SBA Message type editing UI.
 */

/**
 * UI controller.
 */
//class SbaMessageTypeUIController extends EntityDefaultUIController {ç
//
//  /**
//   * Overrides hook_menu() defaults.
//   */
////  public function hook_menu() {
////    $items = parent::hook_menu();
////		$items[$this->path]['description'] = 'Manage sba_message entity types, including adding
////		and removing fields and the display of fields.';
////    return $items;
////  }
//}

/**
 * Generates the sba_message type editing form.
 *
 * @todo remove before production
 */
//function sba_message_type_form($form, &$form_state, $sba_message_type, $op = 'edit') {
//
//  if ($op == 'clone') {
//    $sba_message_type->label .= ' (cloned)';
//    $sba_message_type->type = '';
//  }
//
//  $form['label'] = array(
//    '#title' => t('Label'),
//    '#type' => 'textfield',
//    '#default_value' => $sba_message_type->label,
//    '#description' => t('The human-readable name of this sba_message type.'),
//    '#required' => TRUE,
//    '#size' => 30,
//  );
//  // Machine-readable type name.
//  $form['type'] = array(
//    '#type' => 'machine_name',
//    '#default_value' => isset($sba_message_type->type) ? $sba_message_type->type : '',
//    '#maxlength' => 32,
////    '#disabled' => $sba_message_type->isLocked() && $op != 'clone',
//    '#machine_name' => array(
//      'exists' => 'sba_message_get_types',
//      'source' => array('label'),
//    ),
//    '#description' => t('A unique machine-readable name for this sba_message type. It must only contain lowercase letters, numbers, and underscores.'),
//  );
//
//  $form['data']['#tree'] = TRUE;
//  // $form['data']['sample_data'] = array(
//  //   '#type' => 'checkbox',
//  //   '#title' => t('An interesting sba_message switch'),
//  //   '#default_value' => !empty($sba_message_type->data['sample_data']),
//  // );
//
//  $form['actions'] = array('#type' => 'actions');
//  $form['actions']['submit'] = array(
//    '#type' => 'submit',
//    '#value' => t('Save message type'),
//    '#weight' => 40,
//  );
//
//  //Locking not supported yet
//  /*if (!$sba_message_type->isLocked() && $op != 'add') {
//    $form['actions']['delete'] = array(
//      '#type' => 'submit',
//      '#value' => t('Delete sba_message type'),
//      '#weight' => 45,
//      '#limit_validation_errors' => array(),
//      '#submit' => array('sba_message_type_form_submit_delete')
//    );
//  }*/
//  return $form;
//}
//
///**
// * Form API submit callback for the type form.
// *
// * @todo remove
// */
//function sba_message_type_form_submit(&$form, &$form_state) {
//  $sba_message_type = entity_ui_form_submit_build_entity($form, $form_state);
//  $sba_message_type->save();
//  $form_state['redirect'] = 'admin/structure/sba_message_types';
//}
//
///**
// * Form API submit callback for the delete button.
// *
// * @todo remove
// */
//function sba_message_type_form_submit_delete(&$form, &$form_state) {
//  $form_state['redirect'] = 'admin/structure/sba_message_types/manage/' . $form_state['sba_message_type']->type . '/delete';
//}
