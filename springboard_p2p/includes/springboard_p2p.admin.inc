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

  $form['registration_fields'] = array(
    '#type' => 'fieldset',
    '#title' => 'Registration fields',
    '#weight' => -30,
  );

  $form['registration_fields']['items'] = array(
    '#tree' => TRUE,
  );

  $fields = entity_get_all_property_info('user', 'user');
  foreach ($fields as $key => $field) {
    $form['registration_fields']['items'][$key] = array(
      'name' => array(
        '#markup' => check_plain($field['label']),
      ),
      'show' => array(
        '#type' => 'checkbox',
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#delta' => 10,
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
  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.
  foreach ($form_state['values']['example_items'] as $id => $item) {
//    db_query("UPDATE {tabledrag_example} SET weight = :weight WHERE id = :id", array(':weight' => $item['weight'], ':id' => $id));
  }
}

function theme_springboard_p2p_admin_settings($variables) {
  $form = $variables['form'];
  $rows = array();
  $weight_class = 'registration-field-weight';
  $header = array(t('Name'), t('Show on registration form'), t('Weight'));
  $table_id = 'registration-fields-table';

  $output = '';

  $output .= drupal_render($form['basic']);
  $output .= drupal_render($form['form_types']);

  // Iterate over each element in our $form['example_items'] array.
  foreach (element_children($form['registration_fields']['items']) as $id) {

    $form['registration_fields']['items'][$id]['weight']['#attributes']['class'] = array($weight_class);

    $rows[] = array(
      'data' => array(
        drupal_render($form['registration_fields']['items'][$id]['name']),
        drupal_render($form['registration_fields']['items'][$id]['show']),
        drupal_render($form['registration_fields']['items'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }

  $form['registration_fields']['items']['#markup'] = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => $table_id),
    ));

  $output .= drupal_render_children($form);

  drupal_add_tabledrag($table_id, 'order', 'sibling', $weight_class);

  return $output;
}
