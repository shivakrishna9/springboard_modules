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
        // 'type' => MENU_DEFAULT_LOCAL_TASK,
        // 'context' =>  MENU_CONTEXT_INLINE
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



  $form['quick_groups'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-quick-groups')),
    '#weight' => 400,
  );

  $form['quick_groups']['us_house'] = array(
    '#type' => 'checkbox',
    '#title' => t('US House'),
    '#value' => t('fr'),
  );

  $form['quick_groups']['advocacy_id'] = array(
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
      '#suffix' => l(t('Cancel'), 'node/%node/messages'),
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
 * @todo remove hard-coded link
 */
function sba_message_edit_form_submit(&$form, &$form_state) {
  $sba_message = entity_ui_controller('sba_message')->entityFormSubmitBuildEntity($form, $form_state);
  // Save the sba_message and go back to the list of sba_messages
  
  // Add in created and changed times.
  if ($sba_message->is_new = isset($sba_message->is_new) ? $sba_message->is_new : 0){
    $sba_message->created = time();
    $values = $form_state['values'];
    $loader = new AdvocacyApiClientLoader();
    $client = $loader->getClient();
    $response = 
    $client->createTargetGroup(
      array(
        'name' => $sba_message->name, 
        'chambers' => array (
           0 => array(
             'chamber' => $values['us_house']
           )
        ),
        'message' => array(
          'message_id' => $values['data']['message_id'],
          'form_id' => $values['advocacy_id'],
        ),
      )
    );
  }

  $sba_message->changed = time();
  
  $sba_message->save();
  $nid = $_GET['field_sba_alert_id'];
  $form_state['redirect'] = 'node/' . $nid . '/messages';
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path
 */
function sba_message_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'node/%node/messages/sba_message/' . $form_state['sba_message']->sba_message_id . '/delete';
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

  sba_message_delete($sba_message);

  drupal_set_message(t('The sba_message %name has been deleted.', array('%name' => $sba_message->name)));
  watchdog('sba_message', 'Deleted sba_message %name.', array('%name' => $sba_message->name));

  $form_state['redirect'] = 'node/%node/messages';
}



