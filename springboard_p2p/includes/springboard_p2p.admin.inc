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

  $form['form_types']['fundraiser_items'] = array(
    '#tree' => TRUE,
    '#prefix' => '<h2>Fundraising enabled types</h2>',
  );

  $fundraiser_enabled_content_types = array(
    'blah1' => array(
      'label' => 'Fundraiser enabled content type 1',
    ),
    'blah2' => array(
      'label' => 'Fundraiser enabled content type 2',
    ),
  );

  foreach ($fundraiser_enabled_content_types as $key => $type) {
    $form['form_types']['fundraiser_items'][$key] = array(
      'enabled' => array(
        '#type' => 'checkbox',
        '#title' => $type['label'],
      ),
      'default_form' => array(
        '#type' => 'select',
        '#options' => array(
          'anode' => 'A fundraiser node',
          'anothernode' => 'Another fundraiser node',
        ),
      ),
    );
  }

  $form['form_types']['webform_user_items'] = array(
    '#tree' => TRUE,
    '#prefix' => '<h2>Other form types</h2>',
  );

  $webform_user_enabled_content_types = array(
    'blah1' => array(
      'label' => 'Webform user content type 1',
    ),
    'blah2' => array(
      'label' => 'Webform user content type 2',
    ),
  );

  foreach ($webform_user_enabled_content_types as $key => $type) {
    $form['form_types']['webform_user_items'][$key] = array(
      'enabled' => array(
        '#type' => 'checkbox',
        '#title' => $type['label'],
      ),
      'default_form' => array(
        '#type' => 'select',
        '#options' => array(
          'anode' => 'A webform user node',
          'anothernode' => 'Another webform user node',
        ),
      ),
    );
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
    $form['registration_fields']['registration_field_items'][$key] = array(
      'name' => array(
        '#markup' => check_plain($field['label']),
      ),
      'show' => array(
        '#type' => 'checkbox',
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
  $weight_class = 'registration-field-weight';
  $table_id = 'registration-fields-table';

  $output = '';
  $output .= drupal_render($form['basic']);

  $header = array(t('Type'), t('Default form'));
  foreach (array('fundraiser_items', 'webform_user_items') as $type) {
    $form['form_types'][$type]['#markup'] = theme('table', array(
        'header' => $header,
        'rows' => springboard_p2p_form_types_table_rows($form, $type),
      ));
  }

  $rows = array();
  // Iterate over each element in our $form['example_items'] array.
  foreach (element_children($form['registration_fields']['registration_field_items']) as $id) {
    $form['registration_fields']['items'][$id]['weight']['#attributes']['class'] = array($weight_class);
    $rows[] = array(
      'data' => array(
        drupal_render($form['registration_fields']['registration_field_items'][$id]['name']),
        drupal_render($form['registration_fields']['registration_field_items'][$id]['show']),
        drupal_render($form['registration_fields']['registration_field_items'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $header = array(t('Name'), t('Show on registration form'), t('Weight'));
  $form['registration_fields']['registration_field_items']['#markup'] = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => $table_id),
    ));

  $output .= drupal_render_children($form);

  drupal_add_tabledrag($table_id, 'order', 'sibling', $weight_class);

  return $output;
}

function springboard_p2p_form_types_table_rows($form, $type) {
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
