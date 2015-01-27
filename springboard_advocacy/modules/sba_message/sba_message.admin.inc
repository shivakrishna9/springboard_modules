<?php

/**
 * @file
 * SBA Message editing UI.
 *
 * We make very little use of the EntityAPI interface for this - preferring instead to use
 * views. That offers more flexibility to change a UI that will, more often than not,
 * be end-user facing.
 */

/**
 * UI controller.
 */
class SbaMessageUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {

    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;


    // Add menu items to add each different type of entity.
    foreach (sba_message_get_types() as $type) {
      $items[$this->path . '/add/' . $type->type] = array(
        'title' => 'Add ' . $type->label,
        'page callback' => 'sba_message_form_wrapper',
        'page arguments' => array(sba_message_create(array('type' => $type->type))),
        'access callback' => 'sba_message_access',
        'access arguments' => array('edit', 'edit ' . $type->type),
        'file' => 'sba_message.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module']),
         'type' => MENU_DEFAULT_LOCAL_TASK,
         'context' =>  MENU_CONTEXT_INLINE
      );
    }

    // Loading and editing sba_message entities
    $items[$this->path . '/sba_message/' . $wildcard] = array(
      'page callback' => 'sba_message_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'sba_message_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
    //  'context' => MENU_CONTEXT_INLINE,
      'file' => 'sba_message.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']),
      'type' => MENU_LOCAL_TASK,

    );
    $items[$this->path . '/sba_message/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    //  'context' => MENU_CONTEXT_INLINE,
    );
    
    $items[$this->path . '/sba_message/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'sba_message_delete_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'sba_message_access',
      'access arguments' => array('edit', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'sba_message.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module'])
    );
    
    // Menu item for viewing sba_messages
    // $items['sba_message/' . $wildcard] = array(
    //   //'title' => 'Title',
    //   'title callback' => 'sba_message_page_title',
    //   'title arguments' => array(1),
    //   'page callback' => 'sba_message_page_view',
    //   'page arguments' => array(1),
    //   'access callback' => 'sba_message_access',
    //   'access arguments' => array('view', 1),
    //   'type' => MENU_CALLBACK,
    // );
    return $items;
  }
  
}


/**
 * Form callback wrapper: create or edit a sba_message.
 *
 * @param $sba_message
 *   The sba_message object being edited by this form.
 *
 * @see sba_message_edit_form()
 */
function sba_message_form_wrapper($sba_message) {
  // Add the breadcrumb for the form's location
  return drupal_get_form('sba_message_edit_form', $sba_message);
}


/**
 * Form callback wrapper: delete a sba_message.
 *
 * @param $sba_message
 *   The sba_message object being edited by this form.
 *
 * @see sba_message_edit_form()
 */
function sba_message_delete_form_wrapper($sba_message) {
  // Add the breadcrumb for the form's location.
  return drupal_get_form('sba_message_delete_form', $sba_message);
}


/**
 * Form callback: create or edit a sba_message.
 *
 * @param $sba_message
 *   The sba_message object to edit or for a create form an empty sba_message object
 *     with only a sba_message type defined.
 */
function sba_message_edit_form($form, &$form_state, $sba_message) {

  $node = menu_get_object();

  $form['parent_node']['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );
  // Add the default field elements.

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Internal Title'),
    '#default_value' => isset($sba_message->name) ? $sba_message->name : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );
  
  $form['data']['#tree'] = TRUE;

  $form['data']['message_id'] = array(
    '#type' => 'hidden',
    '#value' => isset($sba_message->data['message_id']) ? $sba_message->data['message_id'] : uniqid(variable_get('site_name', 'Drupal') . '-'),
  );

  // Add the field related form elements.
  $form_state['sba_message'] = $sba_message;
  field_attach_form('sba_message', $sba_message, $form, $form_state);




  $form['quick_targets'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quick Target'),
    '#attributes' => array('class' => array('form-quick-groups')),
    '#weight' => 10,
  );

  $form['quick_targets']['#tree'] = TRUE;

  $form['quick_targets']['chambers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Chamber'),
    '#attributes' => array('class' => array('form-quick-groups')),
    '#weight' => 1,
  );

   

   $form['quick_targets']['chambers'] = array(
    '#type' => 'checkboxes',
    '#options' => 
      array(
      'fs' => t('Federal Senate'),
      'fr' => t('Federal House'),
      // 'ss' => t('State Senate'),
      // 'sr' => t('State House')
      ),
    '#title' => t('Chamber'),
  );

   $form['quick_targets']['party'] = array(
    '#type' => 'checkboxes',
    '#options' => array(
      'd' => t('Democrat'),
      'r' => t('Republican')
      ),
    
    '#title' => t('Party'),
  );

  $form['advocacy_id'] = array(
    '#type' => 'hidden',
    '#value' => $node->advocacy_id,
  );

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save message'),
    '#submit' => $submit + array('sba_message_edit_form_submit'),
  );
  
  if (!empty($sba_message->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete sba_message'),
      '#suffix' => l(t('Cancel'), 'node/' . $node->nid . '/messages'),
      '#submit' => $submit + array('sba_message_form_submit_delete'),
      '#weight' => 45,
    );
  }
  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'sba_message_edit_form_validate';
  return $form;
}


/**
 * Form API validate callback for the sba_message form
 */
function sba_message_edit_form_validate(&$form, &$form_state) {
  $sba_message = $form_state['sba_message'];
  
  // Notify field widgets to validate their data.
  field_attach_form_validate('sba_message', $sba_message, $form, $form_state);
}


/**
 * Form API submit callback for the sba_message form.
 * 
 */
function sba_message_edit_form_submit(&$form, &$form_state) {
  $sba_message = entity_ui_controller('sba_message')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the sba_message and go back to the list of sba_messages
  // Add in created and changed times.

  // Load the API client
  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $values = $form_state['values'];
  
  // array(
  //  'name' => 'group name', //required
  //  'chambers' => array(
  //       0 => array('chamber' => 'fs', 'party =>'r', state => 'MA'),
  //       1 => array('chamber' => 'fs', 'party =>'d', state => 'MA')
  //     )
  //  'executives' => array(
  //       0 => array('role' => 'PRES01', 'party =>'r'),
  //       1 => array('role' => 'PRES03', 'party =>'d')
  //     )
  //   )
  //   'target_ids' => array('1', '2', '3', '4', '5', '6', '7'),
  // )

  $chambers = array_flip($values['quick_targets']['chambers']);
  unset($chambers[0]);

  $parties = array_flip($values['quick_targets']['party']);
  unset($parties[0]);

  $chamber_count = count($chambers);
  $party_count = count($parties);
  $exec_is_checked = FALSE; //isset(yadda)

  $selected = array();
  
  if ($chamber_count == 0 && exec_is_checked == FALSE) {
    $chambers = array('fr', 'fs');
  }
  if ($party_count == 0) {
    $parties = array('r', 'd');
  }
  
  if (!empty($chamber_count) || (!empty($party_count) && exec_is_checked == FALSE)) {
    foreach ($chambers as $chamber) {
      foreach ($values['quick_targets']['party'] as $party) {
        $selected[] = array('chamber' => $chamber, 'party' => $party, 'state' => '');
      }
    }
  }
  // Executive build goes here.

  //New message
  if ($sba_message->is_new = isset($sba_message->is_new) ? $sba_message->is_new : 0) {
    $sba_message->created = time();
    $response = 
    $client->createTargetGroup(
      array(
        'name' => $sba_message->name, 
        'chambers' => $selected,
        'message' => array(
          'message_id' => $values['data']['message_id'],
          'form_id' => $values['advocacy_id'],
        ),
      )
    );
    if (!empty($response->data->id)) {
      $sba_message->data['group_id'] = $response->data->id;
    }
    else {
      drupal_set_message('There was a problem retrieving the group ID');
    }
  }
  else {
    // Editing an existing message

    //
    // $response = $client->updateTargetGroup($values, $group_id)

  }

  $sba_message->changed = time();
  
  $sba_message->save();
  $wrapper = entity_metadata_wrapper('sba_message', $sba_message);
  $nid = $wrapper->field_sba_alert_id->value()->nid;
  $form_state['redirect'] = 'node/' . $nid . '/messages';
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function sba_message_form_submit_delete(&$form, &$form_state) {
  $nid =  $form_state['values']['nid'];
  $form_state['redirect'] = 'node/' . $nid . '/messages/sba_message/' . $form_state['sba_message']->sba_message_id . '/delete';
}


/**
 * Form callback: confirmation form for deleting a sba_message.
 *
 * @param $sba_message
 *   The sba_message to delete
 *
 * @see confirm_form()
 */
function sba_message_delete_form($form, &$form_state, $sba_message) {
  $form_state['sba_message'] = $sba_message;

  $form['#submit'][] = 'sba_message_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete sba_message %name?', array('%name' => $sba_message->name)),
    'node/%node/messages/sba_message',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );
  
  return $form;
}

/**
 * Submit callback for sba_message_delete_form
 */
function sba_message_delete_form_submit($form, &$form_state) {
  $sba_message = $form_state['sba_message'];
  $wrapper = entity_metadata_wrapper('sba_message', $sba_message);
  $nid = $wrapper->field_sba_alert_id->value()->nid;
  sba_message_delete($sba_message);

  //need to invoke client

  drupal_set_message(t('The sba_message %name has been deleted.', array('%name' => $sba_message->name)));
  watchdog('sba_message', 'Deleted sba_message %name.', array('%name' => $sba_message->name));

  $form_state['redirect'] = 'node/' . $nid . '/messages';
}



