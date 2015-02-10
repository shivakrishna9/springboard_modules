<?php

/**
 * @file
 * SBA Message editing UI.
 *
 */

/**
 * UI controller.
 */
class SbaMessageUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {

    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    // @todo the menu type and context definitions here are definitely not "correct"
    // but they do allow the tabs to remain consistent throughout editing workflow
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

    //@todo remove if we don't need a view path
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
  $build = array();

  $build['#attached']['css'] = array(
    drupal_get_path('module', 'sba_message') . '/css/sba_message.css',
  );

  $build['#attached']['js'] = array(
    drupal_get_path('module', 'sba_message') . '/js/sba_message.js',
  );

  $build['content'] = array(
    'sba_message_edit_form' => drupal_get_form('sba_message_edit_form', $sba_message),
    'message_recipients' => array(
      '#id' => 'springboard-advocacy-message-recipients',
      '#type' => 'markup',
      '#markup' => '<div id="springboard-advocacy-message-recipients"><h3>Message Recipients</h3><div class = "sba-message-status"></div></div>',
      '#prefix' => '<div id="springboard-advocacy-message-recipients-container">',
      '#suffix' => '</div>',
      '#weight' => 2,
    ),
    'find_targets' => array(
      '#id' => 'springboard-advocacy-find-targets',
      '#type' => 'markup',
      '#markup' => theme(
        'ctools_collapsible',
        array(
          'handle' => 'Find Targets',
          'content' => views_embed_view('targets', 'block_3'),
          'collapsed' => TRUE
        )
      ),
      '#prefix' => '<div class="springboard-advocacy-find-targets-container"><div class="springboard-advocacy-find-targets">',
      '#suffix' => '</div></div>',
      '#weight' => 3,
    ),
  );
  return $build;
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
 * Message create/edit form
 *
 * This is where all the targeting elements are added.
 *
 * @param $form
 * @param $form_state
 * @param $sba_message
 * @return mixed
 */
function sba_message_edit_form($form, &$form_state, $sba_message) {

  // This form should only appear at node/%/messages.
  // Grab the node so we can get the alert form UUID and node ID
  $node = menu_get_object();
  $form['message_error'] = array (
    '#markup' => '',
    '#prefix' => '<div id = "advo-error-wrapper">',
    '#suffix' => '</div>'
  );
  $form['parent_node']['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );

  $form['advocacy_id'] = array(
    '#type' => 'hidden',
    '#value' => $node->advocacy_id,
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
  // Get/Set the message UUID
  $form['data']['message_id'] = array(
    '#type' => 'hidden',
    '#value' => isset($sba_message->data['message_id']) ? $sba_message->data['message_id'] : uniqid(variable_get('site_name', 'Drupal') . '-'),
  );

  $form['data']['recipients'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($sba_message->data['recipients']) ? $sba_message->data['recipients'] : '',
  );
  $form['data']['group_id'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($sba_message->data['group_id']) ? $sba_message->data['group_id'] : '',
  );

  // Add the field related form elements.
  $form_state['sba_message'] = $sba_message;
  field_attach_form('sba_message', $sba_message, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 400,
  );

  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save message'),
    '#submit' => $submit + array('sba_message_edit_form_submit'),
//    '#ajax' => array(
//      'callback' => 'sba_message_edit_form_submit',
//      'wrapper' => 'advo-error-wrapper',
//    ),
  );
  
  if (!empty($sba_message->name)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete message'),
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

  $element = $form['message_error'];

  if (form_get_errors()) {
    $element['#markup'] = "";
    return $element;
  }

  $form_state['values']['data']['recipients'] = str_replace('&quot;', '"', $form_state['values']['data']['recipients']);

  $sba_message = entity_ui_controller('sba_message')->entityFormSubmitBuildEntity($form, $form_state);
  // Add in created and changed times.
  $is_new = FALSE;
  if ($sba_message->is_new = isset($sba_message->is_new) ? $sba_message->is_new : 0) {
    $sba_message->created = time();
    $is_new = TRUE;
  }
  $sba_message->changed = time();
  
  $sba_message->save();

  _sba_message_api_save($sba_message, $is_new, $form_state);


  $wrapper = entity_metadata_wrapper('sba_message', $sba_message);
  $nid = $wrapper->field_sba_alert_id->value()->nid;
  $form_state['redirect'] = 'node/' . $nid . '/messages';
}

/**
 * Form API submit callback for the delete button.
 * 
 * @todo Remove hard-coded path?
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

  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $client->deleteTargetGroup($sba_message->data['group_id']);

  drupal_set_message(t('The sba_message %name has been deleted.', array('%name' => $sba_message->name)));
  watchdog('sba_message', 'Deleted sba_message %name.', array('%name' => $sba_message->name));

  $form_state['redirect'] = 'node/' . $nid . '/messages';
}



/**
 *
 * Save the target message data to the API server
 * @param $sba_message
 * @param $is_new
 * @param $form_state
 *
 * The api is expecting an array structured like so:
 *
    array(
    'name' => 'group name', //required
    'chambers' => array(
    0 => array('chamber' => 'fs', 'party =>'r', state => 'MA'),
    1 => array('chamber' => 'fs', 'party =>'d', state => 'MA')
    )
    'executives' => array(
    0 => array('role' => 'PRES01', 'party =>'r'),
    1 => array('role' => 'PRES03', 'party =>'d')
    )
    )
    'target_ids' => array('1', '2', '3', '4', '5', '6', '7'),
    )
 */
function _sba_message_api_save($sba_message, $is_new, $form_state) {

  $values = $form_state['values'];
  $indvds = array();
  $unstructured_chambers = array();

  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  if(!empty($values['data']['recipients'])) {

    $targets = json_decode($values['data']['recipients']);

    foreach ($targets as $target) {
      if (isset($target->id)) {
        $indvds[] = $target->id;
      } else {
        $unstructured_chambers[] = $target;
      }
    }
  }
  if (!empty($unstructured_chambers)) {
    $chamber_rows = _sba_message_build_chambers($unstructured_chambers);
  }
  // Executive build call goes here.

  // Is this a new message? Then create a new target group for it.
  if ($is_new) {
    $response =
      $client->createTargetGroup(
        array(
          'name' => $sba_message->name,
          'chambers' => $chamber_rows,
          'target_ids' => $indvds,
          'message' => array(
            'message_id' => $values['data']['message_id'],
            'form_id' => $values['advocacy_id'],
          ),
        )
      );
    if (!empty($response->data->id)) {
      $sba_message->data['group_id'] = $response->data->id;
      $sba_message->changed = time();
      $sba_message->save();

    } else {
      drupal_set_message('There was a problem retrieving the group ID');
    }
  } else {
     $response =
       $client->updateTargetGroup(
       array(
       'name' => $sba_message->name,
       'chambers' => $chamber_rows,
       'target_ids' => $indvds,
       'message' => array(
         'message_id' => $values['data']['message_id'],
         'form_id' => $values['advocacy_id'],
       ),
     ), $sba_message->data['group_id']);

  }
}

/**
 * Take the chambers form input and format it for the API callback.
 * @param $unstructured_chambers
 * @return array
 */
function _sba_message_build_chambers($unstructured_chambers) {
  $chamber_rows_container = array();
  foreach ($unstructured_chambers as $segment_id => $chamber_segments) {
    $chamber_rows_container[$segment_id] = array();
    foreach ($chamber_segments as $name => $segment_values) {
      $segment_values = explode('|', $segment_values);
      if ($name == 'role') {
        $name = 'chamber';
      }
      $i = 0;
      $num = count($chamber_rows_container[$segment_id]);
      if ($num) {
        foreach ($chamber_rows_container[$segment_id] as $chamber_field) {
          foreach ($segment_values as $value) {
            _sba_message_build_chambers_shmoosher($chamber_rows_container, $segment_id, $i, $name, $value, $chamber_field);
            $i++;
          }
        }
      } else {
        foreach ($segment_values as $value) {
          _sba_message_build_chambers_shmoosher($chamber_rows_container, $segment_id, $i, $name, $value);
          $i++;
        }
      }
    }
  }
  $chamber_rows = array();
  foreach ($chamber_rows_container as $container)  {
    $chamber_rows = array_values($container);
  }
  return $chamber_rows;
}

/**
 * Helper function to build chamber rows
 * @param $chamber_rows_container
 * @param $segment_id
 * @param $i
 * @param $name
 * @param $value
 * @param array $chamber_field
 */
function _sba_message_build_chambers_shmoosher(&$chamber_rows_container, $segment_id, $i, $name, $value, $chamber_field = array()) {
  $chamber = isset($chamber_field['chamber']) ? $chamber_field['chamber'] : '';
  $party = isset($chamber_field['party']) ? $chamber_field['party'] : '';
  $gender = isset($chamber_field['gender']) ? $chamber_field['gender'] : '';
  $chamber_rows_container[$segment_id][$i]['chamber'] = $chamber;
  $chamber_rows_container[$segment_id][$i]['party'] = $party;
  $chamber_rows_container[$segment_id][$i]['gender'] = $gender;
  $chamber_rows_container[$segment_id][$i][$name] = $value;
}
