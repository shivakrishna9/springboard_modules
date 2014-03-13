<?php

/**
 * @file
 * Admin functions for peer to peer.
 */

function springboard_p2p_dashboard() {
  $output = array();

  $output['campaigns'] = array(
  );

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

  $output['categories'] = array(
  );

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

  $output['personal_campaigns'] = array(
  );

  $output['personal_campaigns']['heading'] = array(
    '#markup' => '<h2>Personal campaigns awaiting approval</h2>',
  );

  $output['personal_campaigns']['links'] = array(
    '#theme' => 'links',
    '#links' => array(
      'personal-campaigns-approval-queue' => array(
        'title' => t('Open approval queue'),
        'href' => 'admin/springboard/p2p/approval_queue',
      ),
    ),
  );

  return $output;
}

function springboard_p2p_admin_settings() {
  $form = array();

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic settings'),
    '#weight' => -50,
  );

  $form['basic']['springboard_p2p_base_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Base path for personal campaign pages'),
    '#default_value' => variable_get('springboard_p2p_base_path', 'springboard_p2p'),
    '#size' => 40,
    '#required' => TRUE,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
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
  $content_types = springboard_p2p_get_sorted_content_types();
  foreach (array('fundraiser', 'webform_user') as $form_type) {
    $form['form_types'][$form_type . '_items'] = array(
      '#tree' => TRUE,
    );

    if (count($content_types[$form_type])) {
      $form['form_types'][$form_type . '_items']['#prefix'] = $headings[$form_type];

      foreach ($content_types[$form_type] as $key => $name) {
        $node_options = springboard_p2p_get_node_options_by_types($key);
        $formatted_node_options = springboard_p2p_format_node_options($node_options);
        $form['form_types'][$form_type . '_items'][$key] = array(
          'enabled' => array(
            '#type' => 'checkbox',
            '#title' => $name,
          ),
          'default_form' => array(
            '#type' => 'select',
            '#options' => $formatted_node_options,
          ),
        );
      }
    }
  }

  $form['registration_fields'] = array(
    '#type' => 'fieldset',
    '#title' => 'Registration fields',
    '#weight' => -30,
  );

  $form['registration_fields']['registration_field_items'] = array(
    '#tree' => TRUE,
  );

  $fields = entity_get_all_property_info('user', 'user');

  foreach ($fields as $key => $field) {
    if (springboard_p2p_is_blacklisted_registration_field($key)) {
      continue;
    }

    if (springboard_p2p_is_locked_registration_field($key)) {
      $default_value = 1;
      $disabled = TRUE;
    }
    else {
      // @todo
      $default_value = 0;
      $disabled = FALSE;
    }

    $form['registration_fields']['registration_field_items'][$key] = array(
      'name' => array(
        '#markup' => check_plain($field['label']),
      ),
      'show' => array(
        '#type' => 'checkbox',
        '#default_value' => $default_value,
        '#disabled' => $disabled,
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#delta' => 20,
        '#title_display' => 'invisible',
      ),
    );
  }

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 50,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Changes'),
  );

  return $form;
}

function springboard_p2p_admin_settings_submit($form, &$form_state) {
  dpm($form_state['values']);
}

function theme_springboard_p2p_admin_settings($variables) {
  $form = $variables['form'];

  $output = '';
  $output .= drupal_render($form['basic']);

  // The content types tables.
  $header = array(t('Type'), t('Default form'));
  foreach (array('fundraiser_items', 'webform_user_items') as $type) {
    $rows = springboard_p2p_form_types_table_rows($form, $type);
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
  $header = array(t('Name'), t('Show on registration form'), t('Weight'));
  $form['registration_fields']['registration_field_items']['#markup'] = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => $table_id),
    ));

  drupal_add_tabledrag($table_id, 'order', 'sibling', $weight_class);

  // Everything else.
  $output .= drupal_render_children($form);

  return $output;
}

function springboard_p2p_registration_fields_table_rows(&$form, $weight_class) {
  $rows = array();
  foreach (element_children($form['registration_fields']['registration_field_items']) as $id) {
    $form['registration_fields']['registration_field_items'][$id]['weight']['#attributes']['class'] = array($weight_class);
    $rows[] = array(
      'data' => array(
        drupal_render($form['registration_fields']['registration_field_items'][$id]['name']),
        drupal_render($form['registration_fields']['registration_field_items'][$id]['show']),
        drupal_render($form['registration_fields']['registration_field_items'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  return $rows;
}

function springboard_p2p_form_types_table_rows(&$form, $type) {
  $rows = array();
  foreach (element_children($form['form_types'][$type]) as $key) {
    $rows[] = array(
      'data' => array(
        drupal_render($form['form_types'][$type][$key]['enabled']),
        drupal_render($form['form_types'][$type][$key]['default_form']),
      ),
    );
  }
  return $rows;
}

function springboard_p2p_get_sorted_content_types() {
  $sorted_types = array(
    'fundraiser' => array(),
    'webform_user' => array(),
  );

  $node_types = node_type_get_names();

  foreach ($node_types as $key => $name) {
    if (fundraiser_is_donation_type($key)) {
      $sorted_types['fundraiser'][$key] = $name;
    }
    elseif (_webform_user_is_webform_user_node_type($key)) {
      $sorted_types['webform_user'][$key] = $name;
    }
  }

  return $sorted_types;
}

function springboard_p2p_get_node_options_by_types($type) {
  $result = db_select('node','n')
    ->fields('n', array('nid', 'title'))
    ->condition('type', $type, '=')
    ->condition('status', 0,'>')
    ->execute()
    ->fetchAllAssoc('nid');

  return $result;
}

function springboard_p2p_format_node_options($nodes) {
  $options = array();
  foreach ($nodes as $node) {
    $options[$node->nid] = $node->title;
  }

  return $options;
}

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

function springboard_p2p_is_locked_registration_field($key) {
  return in_array($key, array(
      'sbp_first_name',
      'sbp_last_name',
      'mail',
  ));
}
