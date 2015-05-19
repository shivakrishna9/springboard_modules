<?php

/**
 * @file
 * Admin functions for Springboard advocacy.
 */

/**
 * Page callback for the admin form.
 */

function springboard_advocacy_settings_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for Springboard advocacy.') .
        '</p><p>' .
        t('Saving this form will also force a rebuild of the advocacy subscription settings and renew the Oauth Access Token.') .
        '</p>',
      '#prefix' => '<div class="springboard-advocacy-settings-page">',
      '#suffix' => '</div>',
    ),
    'springboard_advocacy_settings_form' => drupal_get_form('springboard_advocacy_settings_form'),
  );
  return $build;
}

/**
 * Configuration settings form for Springboard advocacy.
 */
function springboard_advocacy_settings_form() {
  $form = array();

  $form['transaction'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advocacy Server Configuration'),
  );

  $form['transaction']['springboard_advocacy_server_url'] = array(
    '#title' => t('Springboard advocacy server url'),
    '#description' => t('The url of the Springboard advocacy server.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('springboard_advocacy_server_url', ''),
  );

  $form['transaction']['springboard_advocacy_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Transaction server Oauth Client ID.'),
    '#description' => t('Client ID for legislator lookup'),
    '#default_value' => variable_get('springboard_advocacy_client_id', ''),
  );

  $form['transaction']['springboard_advocacy_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Transaction server Oauth Client Secret.'),
    '#description' => t('Client Secret for legislator lookup'),
    '#default_value' => variable_get('springboard_advocacy_client_secret', ''),
  );

  $form['geo'] = array(
    '#type' => 'fieldset',
    '#title' => t('Geocoding configuration'),
  );

  $form['geo']['springboard_advocacy_geocoder'] = array(
    '#type' => 'hidden',
    '#value' => 'smartystreets_rest',
  );

  $form['geo']['smarty'] = array(
    '#type' => 'fieldset',
    '#title' => t('Smarty Streets configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['geo']['smarty']['springboard_advocacy_smarty_authid'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets Auth ID'),
    '#description' => t('Required for the REST API'),
    '#default_value' => variable_get('springboard_advocacy_smarty_authid', ''),
  );

  $form['geo']['smarty']['springboard_advocacy_smarty_authtoken'] = array(
    '#type' => 'textfield',
    '#title' => t('Smarty Streets Auth Token'),
    '#description' => t('Required for the REST AP'),
    '#default_value' => variable_get('springboard_advocacy_smarty_authtoken', ''),
  );

  return system_settings_form($form);

}

/**
 * Page callback for the advocacy dashboard.
 *
 * Embeds and themes various views landing pages.
 */
function springboard_advocacy_dashboard($path, $arg = NULL) {

  // If the API is not configured, intercept some displays and
  // show an error message.
  if($path != 'actions' && $path != 'petitions/all' && $path != 'actions/all') {
    $unconfigured = springboard_advocacy_api_unconfigured('springboard');
    if(!empty($unconfigured)) {
      $items = implode(', ', $unconfigured);
      $message = t('Please <a href="!url">configure</a> the Springboard Advocacy API settings. There are empty values in :items. This feature will not function without required configuration.', array('!url' => url('admin/config/services/advocacy'), ':items' => $items));
      $content = springboard_advocacy_dashboard_error('error', 'Configuration required', $message);
      return theme('springboard_advocacy_dashboard', array('views' => $content));
    }
  }

  global $user;
  // Prepare pages based on path.

  switch($path) {

    //Custom Target edit form page
    case 'add-custom-targets':
    case 'edit-custom-targets':
      $content = springboard_advocacy_custom_targets_admin_form($path, $arg);
      break;

    // Custom Targets landing page view
    case 'custom-targets':
      // Get the view
      $targets = springboard_advocacy_prepare_dashboard_view('sba_custom_targets', 'block_1');
      //The view is not found or the display is not found
      if(!$targets) {
        $message = t('Target view or view display "block_1" not found.');
        $content = springboard_advocacy_dashboard_error('sba_targets', 'Custom Targets', $message);
        break;
      }

      $content = array(
        'sba_targets' => array(
          'header' => t('Custom Targets'),
          'content' => $targets->preview(),
          'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
        ),
      );
      if (user_access('create targets')) {
        $content['sba_targets']['create_link'] = array('title' => t('Create a new target'), 'href' => 'admin/springboard/advocacy/targets/custom-targets/add');
      };
      break;

    // Actions landing page view. Includes Actions and Petitions.
    case 'actions':

      // Actions
      $actions = springboard_advocacy_prepare_dashboard_view('sba_actions', 'block_1', array('sba_message_action'));
      if ($actions && !empty($actions->result)) {
        $content = array(
          'sba_actions' => array(
            'header' => t('Message Actions'),
            'content' => $actions->preview(),
            'create_link' => array('title' => t('Create a new message action'), 'href' => 'node/add/sba-message-action'),
            'viewmore_link' => array('title' => t('View All Message Actions'), 'href' => 'admin/springboard/advocacy/message-actions/all'),
            'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
          ),
        );
        if (module_exists('springboard_target') && springboard_target_user_can_access_targets()) {
          $content['sba_actions']['manage_link'] = array('title' => t('Manage Custom Targets'), 'href' => 'admin/springboard/advocacy/targets/custom-targets');
        }
      }

      $petitions = springboard_advocacy_prepare_dashboard_view('sba_actions', 'block_1', array('springboard_petition'));
      if ($petitions && !empty($petitions->result)) {
        $content['sba_petitions'] = array(
          'header' => t('Petitions'),
          'content' => $petitions->preview(), //views_embed_view('sbv_forms', 'block_1', 'springboard_petition'),
          'create_link' => array('title' => t('Create a new petition'), 'href' => 'node/add/springboard-petition'),
          'viewmore_link' => array('title' => t('View All Petitions'), 'href' => 'admin/springboard/advocacy/petitions/all'),
          'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
        );
      }
      break;

    // All message actions view page
    case 'message-actions/all':
      $actions_all = springboard_advocacy_prepare_dashboard_view('sba_actions', 'block_2', array('sba_message_action'));
      if(!$actions_all) {
        $content = springboard_advocacy_dashboard_error('sba_actions', 'Custom Targets', 'Message Actions view or view display "block_2" not found.');
        break;
      }

      $content = array(
        'sba_actions' => array(
          'header' => t('All Message Actions'),
          'content' => $actions_all->preview(),
          'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
        ),
      );

      if (user_access('create sba_message_action content')) {
        $content['sba_actions']['create_link'] = array('title' => t('Create a new message action'), 'href' => 'node/add/sba-message-action');
      }
      if (springboard_target_user_can_access_targets()) {
        $content['sba_actions']['manage_link'] = array('title' => t('Manage Custom Targets'), 'href' => 'admin/springboard/advocacy/targets/custom-targets');
      }

      break;

    // All Petitions view page
    case 'petitions/all':
      $petitions_all = springboard_advocacy_prepare_dashboard_view('sba_actions', 'block_2', array('springboard_petition'));
      if($petitions_all) {
        $content = array(
          'sba_actions' => array(
            'header' => t('All Petitions'),
            'content' => $petitions_all->preview(),
            'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
          ),
        );
        if (user_access('create springboard_petition content')) {
          $content['sba_actions']['create_link'] = array('title' => t('Create a new petition'), 'href' => 'node/add/springboard-petition');
        }
      }
      else {
        $content = springboard_advocacy_dashboard_error('sba_actions', 'Custom Targets', 'Petitions view or view display "block_2" not found.');
      }
      break;

    // node/%/messages message landing page
    case ($path == arg(1)):

      $node = node_load($path);

      // Is this a message type?
      $message_types = variable_get('sba_message_node_types', array());
      if(!isset($message_types[$node->type])) {
        $content = springboard_advocacy_dashboard_error('sba_messages_node', 'Messages', 'There was a problem locating the message type for this node type.');
        break;
      }

      // Get the view
      $messages_view = springboard_advocacy_prepare_dashboard_view('sba_messages_node', 'block_1', array($path));

      // Couldn't load the correct view?
      if (!$messages_view) {
        $error_message = t('Message view or view display "block_1" not found');
        $content = springboard_advocacy_dashboard_error('sba_messages_node', 'Messages', $error_message);
        break;
      }

      // Get the messages for this node and count them
      if(isset($node->message_ids)) {
        $messages = entity_load('sba_message', $node->message_ids);
        // If there's only one message, and it's user-editable
        // prevent display of create message link.
        if(count($messages == 1)) {
          $message = array_pop($messages);
          $subject_editable = field_get_items('sba_message', $message, 'field_sba_subject_editable');
          $user_editable = field_get_items('sba_message', $message, 'field_sba_user_editable');
          if($subject_editable[0]['value'] != 'Not editable' || !empty($user_editable)) {
            $no_create = TRUE;
            drupal_set_message(t('This action\'s current message is user-editable, additional messages may not be created for this action until user editing is disabled.'), 'warning', FALSE);
          }
        }
      }

      // set the content
      $content = array(
        'sba_messages_node' => array(
          'header' => t('Messages'),
          'content' => $messages_view->preview(),
          'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
        ),
      );

      // Display/hide create link
      if((user_access('edit own sba_message_action content') && $user->uid == $node->uid) || user_access('edit any sba_message_action content')) {
        $content['sba_messages_node']['create_link'] = array('title' => t('Create a new message'), 'href' => 'node/' . arg(1) . '/messages/add/' . $message_types[$node->type]);
      }

      if (!empty($no_create)) {
        unset($content['sba_messages_node']['create_link']);
      }
      break;

    default:
      $content = array();
  }

  return theme('springboard_advocacy_dashboard', array('views' => $content));
}

/**
 * Advocacy dashboard Custom Targets form page helper
 *
 * @param $path
 * @param null $arg
 * @return array
 */
function springboard_advocacy_custom_targets_admin_form($path, $arg = NULL) {

  if(!module_exists("springboard_target")) {
    $content = springboard_advocacy_dashboard_error('sba_target', 'Custom Targets', 'Springboard Target Module is not enabled.');
    return $content;
  }

  drupal_add_css(drupal_get_path('module', 'springboard_target') . '/css/sba_target.css');

  if(isset($arg)) {
    module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets_edit');
    $form = drupal_get_form('springboard_target_edit_custom_targets_form', $arg);
    $header = t('Edit Target');
  }
  else {
    module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets_create');
    $form = drupal_get_form('springboard_target_create_custom_targets_form');
    $header = t('Create Target');
  }

  $content = array(
    'add-targets' => array(
      'header' => $header,
      'content' => drupal_render($form),
      'manage_link' => array('title' => t('Return to targets list'), 'href' => 'admin/springboard/advocacy/targets/custom-targets'),
      'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
    ),
  );
  if (user_access('create targets') && isset($arg)) {
    $content['add-targets']['create_link'] = array('title' => t('Create a new target'), 'href' => 'admin/springboard/advocacy/targets/custom-targets/add');
  };

  return $content;
}

/**
 * Advocacy dashboard views constructor.
 * Will return false if view or view display is missing.
 * @param $name
 * @param $display
 * @param array $args
 * @return bool|view
 */
function springboard_advocacy_prepare_dashboard_view($name, $display, $args = array()) {
  $view = views_get_view($name);

  if(empty($view)) {
    return FALSE;
  }

  if(isset($view->display[$display])) {
    $view->set_display($display);
  }
  else {
    return FALSE;
  }

  if(!empty($args)) {
    $view->set_arguments($args);
  }

  $view->pre_execute();
  $view->execute();

  if($view->access($display)) {
    return $view;
  }
  else {
    return FALSE;
  }
}

/**
 * Format a dashboard error message.
 * @param $view
 * @param $title
 * @param $message
 * @return array
 */
function springboard_advocacy_dashboard_error($view, $title, $message) {
  $content = array(
    'error' => array(
      'header' => t($title),
      'content' => '<div>' . t($message) . '</div>',
    ),
  );
  return $content;
}
