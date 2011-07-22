<?php

/**
 * Goals admin settings form.
 */
function fundraiser_goals_admin($form, $form_state) {
  $form = array();

  return $form;
}

function fundraiser_goals_block_admin($edit = array(), $configure = FALSE) {

 // TODO: add module check, disable campaign radio and associated UI if salesforce integration isn't enabled?
// TODO: get saved visibility values displaying on block configuration form

 module_load_include('inc', 'block', 'block.admin');
  $block_form = block_admin_configure($edit);
  $form['user_vis_settings'] = $block_form['user_vis_settings'];
  $form['user_vis_settings']['custom']['#default_value'] = $edit['values']['custom'];
  $form['page_vis_settings'] = $block_form['page_vis_settings'];
  $form['page_vis_settings']['pages']['#default_value'] = $edit['values']['pages'];
  $form['page_vis_settings']['visibility']['#default_value'] = $edit['values']['visibility']; 
  $form['role_vis_settings'] = $block_form['role_vis_settings'];

  unset($block_form);
  
  $form['configure'] = array(
    '#type' => 'value',
    '#value' => $configure,
  );
  $form['block_settings']['block_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Block Title'),
    '#default_value' => $edit['values']['block_title'],
  );
  $form['block_settings']['block_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Block Description'),
    '#default_value' => $edit['values']['block_description'],
    '#required' => TRUE,
  );
  $form['block_settings']['fundraiser_goal_delta'] = array(
     '#type' => 'textfield',
     '#title' => t('Machine Name'),
     '#required' => TRUE,
     '#default_value' => $edit['values']['fundraiser_goal_delta'],  
  );
  $form['submissions_wrapper'] = array(
    '#prefix' => '<div id="goal-submission-settings">',
    '#suffix' => '</div>',
    '#theme' => 'fundraiser_goals',
  );
  $form['submissions_wrapper']['submissions'] = array(
    '#type' => 'radios',
    '#title' => t('Submissions'),
    '#description' => t('Submission scope'),
    '#options' => array('0' => 'Webform', 1 => 'Campaign'),
    '#required' => TRUE,
    '#ahah' => array(
      'path' => 'fundraiser_goals_admin/js',
      'wrapper' => 'goal-submission-settings',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#default_value' => isset($edit['post']['submissions']) ? $edit['post']['submissions'] : $edit['values']['submissions'],
  );
  if ((!isset($edit['post']['submissions']) && $edit['values']['submissions'] == 0) || (isset($edit['post']['submissions']) && (int) $edit['post']['submissions'] == 0)) {
    $select_title = t('Target Webform');
    // get list of webforms, construct options array
    $select_options = array(0 => t('- select -'));
    $types = variable_get('webform_node_types', array());
    $types = implode("','", $types);
    $results = db_query("SELECT nid, title FROM {node} WHERE type IN ('%s')", $types);

    while ($result = db_fetch_object($results)) {
      $select_options[$result->nid] = $result->title;
    }
  }
  else {
    // TODO: if (module_exists()), check campaigns even exist before providing radio button.
    $select_title = t('Target Campaign');
    // get list of campaigns, construct options array
    $select_options = array(0 => t('- select -'));
    $campaigns = salesforce_management_api_get_campaigns();
    $select_options = array_merge($select_options, $campaigns);
  }
  $form['submissions_wrapper']['fundraiser_goal_parent_item'] = array(
    '#type' => 'select',
    '#title' => $select_title,
    '#options' => $select_options,
    '#required' => TRUE,
    '#ahah' => array(
      'path' => 'fundraiser_goals_admin/js',
      'wrapper' => 'goal-submission-settings',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#default_value' => isset($edit['post']['fundraiser_goal_parent_item']) ? $edit['post']['fundraiser_goal_parent_item'] : $edit['values']['fundraiser_goal_parent_item'],
  );

  $goal_options  = array(
    0 => t('- select -'),
    1 => t('# of submissions'),
    2 => t('custom'),
  );
  // We have a posted webform node id, act accordingly.
  if (is_numeric((string) $edit['post']['fundraiser_goal_parent_item']) && $edit['post']['fundraiser_goal_parent_item'] ) {
    $type = db_result(db_query('SELECT type FROM {node} WHERE nid=%d', $edit['post']['fundraiser_goal_parent_item']));
    // if it's a donation form add max donation as an option
    if ($type == 'donation_form') {

      $goal_options[3] = 'Donation';
      unset($goal_options[1], $goal_options[2]);
    }
  }
  // Intial form build with data from db.
  elseif (!isset($edit['post']['fundraiser_goal_parent_item']) && is_numeric((string) $edit['values']['fundraiser_goal_parent_item']) && $edit['values']['fundraiser_goal_parent_item']) {
    // get type
    $type = db_result(db_query('SELECT type FROM {node} WHERE nid=%d', $edit['values']['fundraiser_goal_parent_item']));
    // if it's a donation form add max donation as an option
    if ($type == 'donation_form') {

      $goal_options[3] = 'Donation';
      unset($goal_options[1], $goal_options[2]);
    }
  }
  else {
    $goal_options[3] = 'Donation';
    unset($goal_options[2]);
  }

  $form['submissions_wrapper']['fundraiser_goal_type'] = array(
    '#type' => 'select',
    '#title' => t('Type of goal'),
    '#options' => $goal_options,
    '#ahah' => array(
      'path' => 'fundraiser_goals_admin/js',
      'wrapper' => 'goal-submission-settings',
      'method' => 'replace',
      'effect' => 'fade',
    ),
    '#default_value' => isset($edit['post']['fundraiser_goal_type']) ? $edit['post']['fundraiser_goal_type'] : $edit['values']['fundraiser_goal_type'],
  );
  if ($edit['values']['fundraiser_goal_type'] == 2) {
  
    $results = db_query('SELECT cid, name FROM {webform_component} WHERE type = "textfield" AND nid = %d', $edit['values']['fundraiser_goal_parent_item']);
    while ($component = db_fetch_object($results)) {
     $options[$component->cid] = $component->name;
    }
    if (!count($options)) {
      form_set_error('fundraiser_goal_target_cid', t('Custom goals require a text field to target. Please add at least one text field to this form before selecting Custom. If you are creating a new webform you may need to save it and add fields before trying to assign a goal.'));
    }
    $form['submissions_wrapper']['fundraiser_goal_target_cid'] = array(
      '#type' => 'select',
      '#title' => t('Choose a field to target for this goal'),
      '#options' => $options,
      '#default_value' => $edit['values']['fundraiser_goal_target_cid'],
    );
  }
  $form['submissions_wrapper']['fundraiser_goal_target'] = array(
    '#type' => 'textfield',
    '#title' => t('Goal target'),
    '#default_value' => $edit['values']['fundraiser_goal_target'],
    '#required' => TRUE,
  );
  $form['submissions_wrapper']['fundraiser_goal_display_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Display Threshold'),
    '#description' => t('The goal block will not show up on a page until this value is reached.'),
    '#default_value' => $edit['values']['fundraiser_goal_display_threshold'] ? $edit['values']['fundraiser_goal_display_threshold'] : 0,
  );
  $form['submissions_wrapper']['fundraiser_goal_default_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Starter message'),
    '#description' => t('This is an optional message to show users if no form submission has been made. Note: if display threshold is set users will not see this message.'),
    '#default_value' => $edit['values']['fundraiser_goal_default_message'],
  );

  // Block body template and associated cruft.
  $form['submissions_wrapper']['template'] = array(
    '#type' => 'fieldset',
    '#title' => t('Goal template'),
    '#collapsible' => TRUE,
  );
  $form['submissions_wrapper']['template']['fundraiser_goal_template'] = array(
    '#type' => 'textarea',
    '#title' => t('Template'),
    '#description' => t('Markup appearing for this goal.'),
    '#default_value' => $edit['values']['fundraiser_goal_template'],
  );
  $form['submissions_wrapper']['template']['goal_tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available goal tokens'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 11,
  );
  $filter = $edit['values']['submissions_wrapper']['template'] ? $edit['values']['submissions_wrapper']['template']  : 2;
  $form['submissions_wrapper']['template']['fundraiser_goal_template_format'] =  filter_form($filter, 12, array('fundraiser_goals', 'goals_wrapper', 'template'));
  $form['submissions_wrapper']['template']['goal_tokens']['help'] = array(
    '#value' => theme('token_help', 'goal'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 12,
  );
  // Shove display settings to the bottom of the form where they belong.
  $form['user_vis_settings']['#weight'] = 11;
  $form['page_vis_settings']['#weight'] = 11;
  $form['role_vis_settings']['#weight'] = 11;

  $form['#validate'] = array('fundraiser_goals_admin_validate');
  $form['#submit'] = array('fundraiser_goals_admin_submit');
  return $form;
}

/**
 * Admin validate callback.
 */
function fundraiser_goals_admin_validate(&$form, &$form_state) {
  $form_state['values']['title'] = $form_state['values']['fundraiser_goal_delta'];
  if (!$form_state['values']['configure'] && db_result(db_query("SELECT 1 FROM {fundraiser_goals} WHERE delta='%s'", $form_state['values']['fundraiser_goal_delta']))) {
    form_set_error('fundraiser_goals_delta', t('Machine Name must be unique. %delta is already taken.', array('%delta' => $form_state['values']['fundraiser_goal_delta'])));
  }
}

/**
 * Admin submit callback.
 */
function fundraiser_goals_admin_submit($form, &$form_state) {
 
   $object = (object) $form_state['values'];
   if (!$form_state['values']['configure']) {
     $object->pid = $object->fundraiser_goal_parent_item;
     _fundraiser_goals_save_goal($object);

   }
   else {
     _fundraiser_goals_update_goal($object);
   }
   $form_state['redirect'] = 'admin/build/block';
   return;
}

/**
 * Delete block.
 */
function fundraiser_goals_block_admin_delete(&$form_state, $delta = 0) {

  $title = db_result(db_query("SELECT title FROM {blocks} WHERE module = 'fundraiser_goals' AND delta = '%s'", $delta));
  $form = array();
  $form['delta'] = array('#type' => 'hidden', '#value' => $delta);
  return confirm_form($form, t('Are you sure you want to delete the "%name" block?', array('%name' => $title)), 'admin/build/block', NULL, t('Delete'), t('Cancel'));
}

/**
 * Submit handler for delete block form.
 */
function fundraiser_goals_block_admin_delete_submit($form, &$form_state) {
  global $user;
  db_query("DELETE FROM {fundraiser_goals} WHERE delta = '%s'", $form_state['values']['delta']);
  $bid = db_result(db_query("SELECT bid FROM {blocks} WHERE module = 'fundraiser_goals' AND delta = '%s'", $form_state['values']['delta']));
  $info = db_result(db_query('SELECT info FROM {fundraiser_goals_blocks} WHERE bid=%d', $bid));
  db_query('DELETE FROM {blocks} WHERE bid = %d', $bid);
  db_query("DELETE FROM {blocks_roles} WHERE module = 'fundraiser_goals' AND delta = '%s'", $form_state['values']['delta']);
  db_query('DELETE FROM {fundraiser_goals_blocks} WHERE bid = %d', $bid);
  watchdog('fundraiser_goals', 'The @info advanced goal block has been deleted by @user.', array('@info' => $info, '@user' => $user->name));
  $form_state['redirect'] = 'admin/build/block';
  return;
}

function fundraiser_goals_block_admin_configure($edit, $delta = 0) {

  $goal = db_fetch_object(db_query("SELECT * FROM {fundraiser_goals} WHERE delta = '%s'", $delta));
  $block_info = db_fetch_object(db_query("SELECT b.title, b.custom, b.visibility, b.pages, fb.info FROM {blocks} b INNER JOIN {fundraiser_goals_blocks} fb ON b.bid = fb.bid WHERE b.module = 'fundraiser_goals' AND b.delta = '%s'", $delta));

      $edit['values'] = array(
        'block_title' => $block_info->title,
        'visibility' => $block_info->visibility,
        'pages' => $block_info->pages,
        'block_description' => $block_info->info,
        'fundraiser_goal_parent_item' => $goal->pid,
        'fundraiser_goal_delta' => $goal->delta,
        'fundraiser_goal_type' => $goal->type,
        'fundraiser_goal_target' => $goal->target,
        'fundraiser_goal_target_cid' => $goal->target_cid,
        'fundraiser_goal_display_threshold' => $goal->display_threshold,
        'fundraiser_goal_template' => $goal->template,
        'fundraise_goal' => array('goals_wrapper' => array('template' => $goal->template_format)),
        'fundraiser_goal_default_message' => $goal->default_message,
        'custom' => $block_info->custom,
        'visibility' => $block_info->visibility,
      );
      $edit['values']['submissions'] = is_numeric((string) $goal->pid) ? 0 : 1;
 ;
      return fundraiser_goals_block_admin($edit, TRUE);
}
/**
 * #AHAH callback for blck admin form.
 */
function fundraiser_goals_admin_ahah() {
  module_load_include('module', 'fundraiser');

  $form_state = array('storage' => NULL, 'submitted' => FALSE, 'rebuild' => TRUE);
  $form_build_id = $_POST['form_build_id'];
  $form = form_get_cache($form_build_id, $form_state);
  
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // Disable validation
  $form['#validate'] = NULL;
  $form_state['validate_handlers'] = NULL;
  fundraiser_ahah_helper_disable_validation($form);
  
  drupal_process_form($form_id, $form, $form_state);
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);
  $goal_form = $form['submissions_wrapper'];
  unset($goal_form['#prefix'], $amount_form['#suffix']);
 
  $output = theme('status_messages') . drupal_render($goal_form);

  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  drupal_json(array('status' => TRUE, 'data' => $output));
}