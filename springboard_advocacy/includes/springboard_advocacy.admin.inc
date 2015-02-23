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
      '#markup' => '<p>' . t('Configure settings for Springboard advocacy.') . '</p>',
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

  //we may need this commented section if we offer a melissa option

  // $options = array(
  //   //'smartystreets' => 'SmartyStreets Javascript API (Pay, API key is required)',
  //   'smartystreets_rest' => 'SmartyStreets REST API (Pay, API key is required)',
  // );

  // $form['geo']['springboard_advocacy_geocoder'] = array(
  //   '#type' => 'radios',
  //   '#title' => t('Active Geocoding service'),
  //   '#options' => $options,
  //   '#description' => t('Choose the geocoding service to use. All but Google Javascript API require an API Key'),
  //   '#default_value' => variable_get('springboard_advocacy_geocoder', ''),
  // );
  
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

  // $form['geo']['smarty']['springboard_advocacy_smarty_htmltoken'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Smarty Streets HTML token'),
  //   '#description' => t('Required for the Javascript API only'),
  //   '#default_value' => variable_get('springboard_advocacy_smarty_htmltoken', ''),
  // );

  return system_settings_form($form);

}

/**
 * Page callback for the advocacy dashboard.
 *
 * Embeds and themes various views landing pages.
 */
function springboard_advocacy_dashboard($path, $arg = NULL) {

  switch($path) {
    case 'add-custom-targets':
        module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets_create');
        drupal_add_css(drupal_get_path('module', 'springboard_target') . '/css/sba_target.css');
        $form = drupal_get_form('springboard_target_create_custom_targets_form');
        $views = array(
          'add-targets' => array(
            'header' => t('Create Target'),
            'content' => drupal_render($form),
            'manage_link' => array('title' => t('Return to targets list'), 'href' => 'admin/springboard/advocacy/targets/custom-targets'),
            'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
          ),
        );
      break;
    case 'edit-custom-targets':
      module_load_include('inc', 'springboard_target', 'includes/springboard_target.custom_targets_edit');
      drupal_add_css(drupal_get_path('module', 'springboard_target') . '/css/sba_target.css');
      $form = drupal_get_form('springboard_target_edit_custom_targets_form', $arg);
      $views = array(
        'add-targets' => array(
          'header' => t('Edit Target'),
          'content' => drupal_render($form),
          'create_link' => array('title' => t('Create a new target'), 'href' => 'admin/springboard/advocacy/targets/custom-targets/add'),
          'manage_link' => array('title' => t('Return to targets list'), 'href' => 'admin/springboard/advocacy/targets/custom-targets'),
          'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
        ),
      );
      break;

    case 'alerts':
      $view = views_get_view('sba_alerts');
      if($view && isset($view->display['block_1'])) {
        $views = array(
          'sba_alerts' => array(
            'header' => t('Site alerts'),
            'content' => views_embed_view('sba_alerts', 'block_1'),
            'create_link' => array('title' => t('Create a new alert'), 'href' => 'node/add/sba-message-action'),
            'manage_link' => array('title' => t('Manage Custom Targets'), 'href' => 'admin/springboard/advocacy/targets/custom-targets'),
            'viewmore_link' => array('title' => t('View all'), 'href' => 'admin/springboard/advocacy/alerts/all'),
            'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
          ),
        );
      }
      else {
        $views = array(
          'sba_alerts' => array(
            'header' => t('Site alerts'),
            'content' => '<div>Alerts view or view display "block_1" not found.</div>',
          ),
        );
      }
      break;

    case 'alerts/all':
      $view = views_get_view('sba_alerts');
      if($view && isset($view->display['block_2'])) {
        $views = array(
          'sba_alerts' => array(
            'header' => t('All Site alerts'),
            'content' => views_embed_view('sba_alerts', 'block_2'),
            'create_link' => array('title' => t('Create a new alert'), 'href' => 'node/add/sba-message-action'),
            'manage_link' => array('title' => t('Manage Custom Targets'), 'href' => 'admin/springboard/advocacy/targets/custom-targets'),
            'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',

          ),
        );
      }
      else {
        $views = array(
          'sba_alerts' => array(
            'header' => t('All Site alerts'),
            'content' => '<div>Alerts view or view display "block_2" not found.</div>',
          ),
        );
      }
      break;

    case 'custom-targets':
      $view = views_get_view('targets');
      if($view && isset($view->display['block_1'])) {
      $views = array(
        'sba_targets' => array(
          'header' => t('Custom Targets'),
          'content' => views_embed_view('targets', 'block_1'),
          'create_link' => array('title' => t('Create a new target'), 'href' => 'admin/springboard/advocacy/targets/custom-targets/add'),
          'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
        ),
      );
      }
      else {
        $views = array(
          'sba_targets' => array(
            'header' => t('Custom Targets'),
            'content' => '<div>Target view or view display "block_1" not found.</div>',
          ),
        );
      }

      break;

    case ($path == arg(1)):
      $message_types = variable_get('sba_message_node_types', array());
      $node = node_load($path);
      if(isset($message_types[$node->type])) {
        $view = views_get_view('sba_messages_node');
        if ($view && isset($view->display['block_1'])) {
          $views = array(
            'sba_messages_node' => array(
              'header' => t('Messages'),
              'content' => views_embed_view('sba_messages_node', 'block_1', $path),
              'create_link' => array('title' => t('Create a new message'), 'href' => 'node/' . arg(1) . '/messages/add/' . $message_types[$node->type]),
              'css' => drupal_get_path('module', 'springboard_advocacy') . '/css/sba.css',
            ),
          );
        } else {
          $views = array(
            'sba_targets' => array(
              'header' => t('Messages'),
              'content' => '<div>Message view or view display "block_1" not found.</div>',
            ),
          );
        }
      }
      else {
        $views = array(
          'sba_targets' => array(
            'header' => t('Messages'),
            'content' => '<div>There was a problem locating the message type for this node type.</div>',
          ),
        );
      }
      break;

    default:  
  }
  return theme('springboard_advocacy_dashboard', array('views' => $views));

}

