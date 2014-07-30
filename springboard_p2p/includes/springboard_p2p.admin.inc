<?php

/**
 * @file
 * Admin functions for peer to peer.
 */

/**
 * Page callback for the p2p dashboard.
 */
function springboard_p2p_dashboard() {

  if (springboard_p2p_no_content_types_enabled()) {
    drupal_set_message('No content types have been enabled for peer to peer.  Please visit the settings and configure them.', 'error');
  }

  $output = array();

  $output['campaigns'] = array();

  $output['campaigns']['heading'] = array(
    '#markup' => '<h2>Active peer to peer campaigns</h2>',
  );

  $output['campaigns']['campaigns'] = array(
    '#markup' => views_embed_view('p2p_campaigns', 'block'),
  );

  $output['campaigns']['links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'add-campaign' => array(
        'title' => t('Create a new campaign'),
        'href' => 'node/add/p2p-campaign',
      ),
      'view-campaigns' => array(
        'title' => t('See all campaigns'),
        'href' => 'admin/springboard/p2p/campaign',
      ),
    ),
  );

  $output['categories'] = array();

  $output['categories']['heading'] = array(
    '#markup' => '<h2>Campaign categories</h2>',
  );

  $output['categories']['categories'] = array(
    '#markup' => views_embed_view('p2p_categories', 'block'),
  );

  $output['categories']['links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'add-category' => array(
        'title' => t('Create a new category'),
        'href' => 'node/add/p2p-category',
      ),
      'view-categories' => array(
        'title' => t('See all categories'),
        'href' => 'admin/springboard/p2p/category',
      ),
    ),
  );

  $output['user_approval'] = array();

  $output['user_approval']['heading'] = array(
    '#markup' => '<h2>Users awaiting approval</h2>',
  );

  // Need to use the page display because the views form won't submit otherwise.
  $output['user_approval']['user_approval'] = array(
    '#markup' => views_embed_view('p2p_user_approval', 'dashboard_page'),
  );

  $output['user_approval']['links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'user_approval' => array(
        'title' => t('Open approval queue'),
        'href' => 'admin/springboard/p2p/approval/user',
      ),
    ),
  );

  $output['personal_campaigns'] = array();

  $output['personal_campaigns']['heading'] = array(
    '#markup' => '<h2>Personal campaigns</h2>',
  );

  $output['personal_campaigns']['personal_campaigns'] = array(
    '#markup' => views_embed_view('p2p_personal_campaigns_admin', 'page'),
  );

  $output['landing_pages'] = array();

  $output['landing_pages']['heading'] = array(
    '#markup' => '<h2>Landing pages</h2>',
  );

  $output['landing_pages']['landing_pages'] = array(
    '#markup' => views_embed_view('p2p_landing_pages', 'block'),
  );

  $output['landing_pages']['links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'landing_pages' => array(
        'title' => t('View all landing pages.'),
        'href' => 'admin/springboard/p2p/landing-pages',
      ),
    ),
  );

  return $output;
}

/**
 * Form callback for the p2p global admin settings.
 */
function springboard_p2p_admin_settings() {
  $form = array();

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic settings'),
    '#weight' => -50,
  );

  $current_base_path = variable_get('springboard_p2p_base_path', 'p2p');

  $form['basic']['current_base_path'] = array(
    '#type' => 'value',
    '#value' => $current_base_path,
  );

  $form['basic']['base_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Base path for personal campaign pages'),
    '#default_value' => $current_base_path,
    '#size' => 40,
    '#required' => TRUE,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  $form['basic']['base_path_personal_campaigns'] = array(
    '#type' => 'radios',
    '#title' => t('What to do with existing personal campaigns'),
    '#description' => t('If changing the base path, what should happen to existing personal campaign paths?'),
    '#default_value' => 'none',
    '#options' => array(
      'none' => t('Leave existing paths unchanged.'),
    ),
  );

  if (module_exists('redirect')) {
    $form['basic']['base_path_personal_campaigns']['#options']['add_redirects'] = t('Update existing paths with the new base path and add an automatic redirect. This option will cause all requests to the old path to automatically redirect to the new path.');
  }
  else {
    $form['basic']['base_path_personal_campaigns']['#options']['update_paths'] = t('Update existing paths with the new base path. <span class="error"><strong>Warning:</strong> by changing the base path, existing URLs will not redirect and will not be accessible (404 pages). If you would like to create redirects, you must first enable the redirect module on your site.</span>');
  }

  $form['help_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Messages'),
    '#weight' => 225,
  );

  // Login message text area.
  // We set a var here, otherwise drupal complains about htmlspecialchars.
  // This is a known bug with text_format FAPI.
  $textformat = variable_get('springboard_p2p_login_message_area', array('value' => '', 'format' => NULL));
  // Now render the form item.
  $form['help_message']['login_message_area'] = array(
    '#type' => 'text_format',
    '#title' => t('P2P Login message'),
    '#required' => TRUE,
    '#rows' => 5,
    '#format' => $textformat['format'],
    '#default_value' => $textformat['value'],
    '#description' => t('Use this area to set a P2P Login message that will appear on the registration page for your users located at /p2p/register.'),
  );

  // Help message text area.
  // We set a var here, otherwise drupal complains about htmlspecialchars.
  // This is a known bug with text_format FAPI.
  $textformat = variable_get('springboard_p2p_help_message_area', array('value' => '', 'format' => NULL));
  // Now render the form item.
  $form['help_message']['help_message_area'] = array(
    '#type' => 'text_format',
    '#title' => t('P2P help message'),
    '#required' => TRUE,
    '#rows' => 5,
    '#format' => $textformat['format'],
    '#default_value' => $textformat['value'],
    '#description' => t('This creates a block with a help message.'),
  );

  $form['form_types'] = array(
    '#type' => 'fieldset',
    '#title' => 'Peer to peer enabled form types',
    '#weight' => -40,
  );

  $headings = array(
    'fundraiser' => '<h2>Fundraising enabled types</h2>',
    'webform_user' => '<h2>Other form types</h2>',
  );

  $content_types = springboard_p2p_fields_get_sorted_content_types();
  foreach (array('fundraiser', 'webform_user') as $form_type) {
    $form['form_types'][$form_type . '_items'] = array(
      '#tree' => TRUE,
    );

    if (count($content_types[$form_type])) {
      $defaults = variable_get('springboard_p2p_' . $form_type . '_items', array());
      $form['form_types'][$form_type . '_items']['#prefix'] = $headings[$form_type];

      foreach ($content_types[$form_type] as $key => $name) {
        $node_options = springboard_p2p_fields_get_node_options_by_types($key);
        $formatted_node_options = springboard_p2p_fields_format_node_options($node_options);
        $form['form_types'][$form_type . '_items'][$key] = array(
          'enabled' => array(
            '#type' => 'checkbox',
            '#title' => $name,
            '#default_value' => isset($defaults[$key]['enabled']) ? $defaults[$key]['enabled'] : 0,
          ),
          'nid' => array(
            '#type' => 'select',
            '#options' => $formatted_node_options,
            '#default_value' => isset($defaults[$key]['nid']) ? $defaults[$key]['nid'] : '',
            '#empty_value' => '',
          ),
        );
      }
    }
  }

  $form['registration_fields_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Registration fields',
    '#weight' => -30,
  );

  $form['registration_fields_fieldset']['registration_fields'] = array(
    '#tree' => TRUE,
  );

  $fields = entity_get_all_property_info('user', 'user');

  $defaults = variable_get('springboard_p2p_registration_fields', array());
  $sorted_keys = springboard_p2p_sort_keys_by_weight($defaults);
  $sorted_fields = array();
  foreach ($sorted_keys as $key) {
    $sorted_fields[$key] = $fields[$key];
    unset($fields[$key]);
  }
  // Add in any leftover fields to the end.
  $sorted_fields += $fields;

  foreach ($sorted_fields as $key => $field) {
    if (springboard_p2p_is_blacklisted_registration_field($key)) {
      continue;
    }

    if (springboard_p2p_is_locked_registration_field($key)) {
      $enabled_default_value = 1;
      $enabled_disabled = TRUE;
      $required_default_value = 1;
      $required_disabled = TRUE;
    }
    else {
      $enabled_default_value = isset($defaults[$key]['enabled']) ? $defaults[$key]['enabled'] : 0;
      $enabled_disabled = FALSE;
      $required_default_value = isset($defaults[$key]['required']) ? $defaults[$key]['required'] : 0;
      $required_disabled = FALSE;
    }

    $weight_default_value = isset($defaults[$key]['weight']) ? $defaults[$key]['weight'] : 0;

    $form['registration_fields_fieldset']['registration_fields'][$key] = array(
      'name' => array(
        '#markup' => check_plain($field['label']),
      ),
      'enabled' => array(
        '#type' => 'checkbox',
        '#default_value' => $enabled_default_value,
        '#disabled' => $enabled_disabled,
      ),
      'required' => array(
        '#type' => 'checkbox',
        '#default_value' => $required_default_value,
        '#disabled' => $required_disabled,
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#delta' => 20,
        '#title_display' => 'invisible',
        '#default_value' => $weight_default_value,
      ),
    );
  }

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 250,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Changes'),
  );

  return $form;
}

/**
 * Validation handler for the p2p global admin settings.
 */
function springboard_p2p_admin_settings_validate($form, &$form_state) {
  foreach (array('fundraiser_items', 'webform_user_items') as $type) {
    if (isset($form_state['values'][$type])) {
      foreach ($form_state['values'][$type] as $content_type => $item) {
        if ($item['enabled'] && $item['nid'] == '') {
          $name = $form['form_types'][$type][$content_type]['enabled']['#title'];
          $error = t('A default form must be selected for the enabled content type %content_type.', array('%content_type' => $name));
          form_set_error($type . '][' . $content_type . '][nid', $error);
        }
      }
    }
  }
}

/**
 * Submit handler for the p2p global admin settings.
 */
function springboard_p2p_admin_settings_submit($form, &$form_state) {
  $keys = array(
    'base_path',
    'fundraiser_items',
    'webform_user_items',
    'registration_fields',
    'login_message_area',
    'help_message_area',
  );

  foreach ($keys as $key) {
    $value = isset($form_state['values'][$key]) ? $form_state['values'][$key] : array();
    variable_set('springboard_p2p_' . $key, $value);
  }

  // If the current base path is different from the new base path, save it to the
  // pathauto variable for this node type. This changes the Pathauto settings,
  // which means that for each campaign that gets saved, Pathauto can handle the
  // path updates; this module doesn't have to do it.
  if (module_exists('pathauto')) {
    if ($form_state['values']['current_base_path'] != $form_state['values']['base_path']) {
      variable_set('pathauto_node_p2p_personal_campaign_pattern', $form_state['values']['base_path'] . '/[node:field-p2p-personal-campaign-url]');
    }
  }

  springboard_p2p_modify_base_path(
    $form_state['values']['current_base_path'],
    $form_state['values']['base_path'],
    $form_state['values']['base_path_personal_campaigns']
  );

  drupal_set_message('Settings saved.');
}

/**
 * Theme function for the p2p global admin settings.
 */
function theme_springboard_p2p_admin_settings($variables) {
  $form = $variables['form'];

  $output = '';
  $output .= drupal_render($form['basic']);

  // The content types tables.
  $header = array(t('Enabled'), t('Default form'));
  foreach (array('fundraiser_items', 'webform_user_items') as $type) {
    $rows = springboard_p2p_fields_form_types_table_rows($form, $type);
    if (count($rows)) {
      $form['form_types'][$type]['#markup'] = theme('table', array(
          'header' => $header,
          'rows' => $rows,
        ));
    }
  }

  // The registration fields table and tabledrag.
  $weight_class = 'registration-field-weight';
  $table_id = 'registration-fields-table';
  $rows = springboard_p2p_registration_fields_table_rows($form, $weight_class);
  $header = array(
    t('Name'),
    t('Show on registration form'),
    t('Required'),
    t('Weight'),
  );

  $form['registration_fields_fieldset']['registration_fields']['#markup'] = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => $table_id),
    ));

  drupal_add_tabledrag($table_id, 'order', 'sibling', $weight_class);

  // Everything else.
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Generates a rows array for the registration fields table.
 *
 * @param array $form
 *   The admin settings form api array.
 * @param string $weight_class
 *   The class to add to the weight column for tabledrag.
 *
 * @return array
 *   An array of rows for the registration fields table.
 */
function springboard_p2p_registration_fields_table_rows(&$form, $weight_class) {
  $rows = array();
  foreach (element_children($form['registration_fields_fieldset']['registration_fields']) as $id) {
    $form['registration_fields_fieldset']['registration_fields'][$id]['weight']['#attributes']['class'] = array($weight_class);
    $rows[] = array(
      'data' => array(
        drupal_render($form['registration_fields_fieldset']['registration_fields'][$id]['name']),
        drupal_render($form['registration_fields_fieldset']['registration_fields'][$id]['enabled']),
        drupal_render($form['registration_fields_fieldset']['registration_fields'][$id]['required']),
        drupal_render($form['registration_fields_fieldset']['registration_fields'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  return $rows;
}

/**
 * Determine if a registration field should be not be shown on the settings.
 *
 * @param string $key
 *   The key of the registration field.
 *
 * @return bool
 *   TRUE if this field should not be displayed.
 */
function springboard_p2p_is_blacklisted_registration_field($key) {
  return in_array($key, array(
      'uid',
      'name',
      'url',
      'edit_url',
      'last_access',
      'last_login',
      'created',
      'roles',
      'status',
      'theme',
      'sbp_cid',
      'sbp_ms',
      'sbp_referrer',
      'sbp_initial_referrer',
      'sbp_search_engine',
      'sbp_search_string',
      'sbp_user_agent',
      'sbp_salesforce_account_id',
      'sbp_salesforce_contact_id',
  ));
}

/**
 * Determines if the user should not be allowed to disable a registration field.
 *
 * @param string $key
 *   The registration field key.
 *
 * @return bool
 *   TRUE if the field should not be disabled.
 */
function springboard_p2p_is_locked_registration_field($key) {
  return in_array($key, array(
      'sbp_first_name',
      'sbp_last_name',
      'mail',
  ));
}

/**
 * Sorts an array by weight.
 *
 * @param array $items
 *   Associative array of items, nested with a weight key.
 *
 * @return array
 *   The item keys ordered by weight.
 */
function springboard_p2p_sort_keys_by_weight($items) {
  $sort = array();
  foreach (array_keys($items) as $key) {
    $sort[$key] = $items[$key]['weight'];
  }
  asort($sort);

  return array_keys($sort);
}

/**
 * If the base path has changed, update existing campaigns based on the setting.
 *
 * @param string $old
 *   The old/current base path.
 * @param string $new
 *   The new base path.
 * @param string $update_existing
 *   How to handle existing personal campaigns.
 *   Can be 'none', 'update_paths', or 'add_redirects'.
 */
function springboard_p2p_modify_base_path($old, $new, $update_existing) {
  if ($old != $new) {
    switch ($update_existing) {
      case 'none':
        drupal_set_message('The base path was updated. No changes were made to existing personal campaigns.');
        break;

      case 'update_paths':
        springboard_p2p_change_peronsal_campaign_paths($new, FALSE);
        break;

      case 'add_redirects':

        springboard_p2p_change_peronsal_campaign_paths($new, TRUE);
        break;
    }
  }
}

/**
 * Changes the personal campaign paths for all personal campaigns.
 *
 * @param string $new_base_path
 *   The new base path to use.
 * @param bool $add_redirects
 *   Whether to add redirects for the old paths to the new paths.
 *   Requires the Redirect module.
 */
function springboard_p2p_change_peronsal_campaign_paths($new_base_path, $add_redirects = FALSE) {
  // Sanity check.  This really shouldn't happen because the option won't
  // exist if the module does not exist.
  if ($add_redirects && !module_exists('redirect')) {
    drupal_set_message('The Redirect module is not enabled.');
    return;
  }

  drupal_set_message('The following personal campaigns have been updated.');

  $nodes = springboard_p2p_get_all_personal_campaigns();

  // Temporarily change this variable so that Redirect will automaitcallly
  // add redirects when the path is changed.
  if ($add_redirects) {
    $current_auto_redirect = variable_get('redirect_auto_redirect', TRUE);
    variable_set('redirect_auto_redirect', TRUE);
  }

  foreach ($nodes as $node) {
    $internal_path = 'node/' . $node->nid;

    // Load the current path, change the alias, and save it.
    $path = path_load($internal_path);

    // If there's no existing path leave it alone.
    if ($path) {
      $path['original'] = $path;

      // Make sure we're only replacing the beginning of the path.
      $path_parts = explode('/', $path['alias']);
      $path_parts[0] = $new_base_path;
      $path['alias'] = implode('/', $path_parts);

      // Create a redirect using the redirect module.
      // When the auto redirect variable is on, we can control whether
      // Redirect makes a redirect by settings this value.
      $path['redirect'] = $add_redirects;

      path_save($path);

      drupal_set_message(l($node->title, $internal_path));
    }
  }

  // Set the auto redirect variable back to its previous value.
  if ($add_redirects) {
    variable_set('redirect_auto_redirect', $current_auto_redirect);
  }

}
