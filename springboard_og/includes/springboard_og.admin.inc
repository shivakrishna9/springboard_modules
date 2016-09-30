<?php


function springboard_og_settings_page() {



  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for Springboard Groups.') . '</p>',
      '#prefix' => '<div class="springboard-groups-settings-page"><H2>' . t('Springboard Groups Settings') . '</H2>',
      '#suffix' => '</div>',
    ),
    'springboard_og_node_settings_form' => drupal_get_form('springboard_og_node_settings_form'),
    'springboard_og_entity_settings_form' => drupal_get_form('springboard_og_entity_settings_form'),

  );
  return $build;
}


/**
 * Configuration settings form for Springboard advocacy.
 */
function springboard_og_node_settings_form() {
  $form = array();

  $form['#attached']['css'][] = drupal_get_path('module', 'springboard_og') . '/css/springboard-og-admin.css';

  $form['springboard_og_node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node types'),
    '#collapsible' => TRUE,
    '#description' => t('Make these content types eligible for grouping'),
  );

  $options = node_type_get_types();
  foreach ($options as $key => $type) {
    if ($type->type != 'springboard_group') {
      $content_type_options[$type->type] = $type->name;
    }
  }

  $groups_enabled = springboard_og_get_enabled_types();

  $form['springboard_og_node']['springboard_og_grouped_content'] = array(
    '#title' => t(''),
    '#type' => 'checkboxes',
    '#options' => $content_type_options,
    '#default_value' => array_keys($groups_enabled['node']),
  );

  foreach (array_keys($groups_enabled['node']) as $key) {
    $form['springboard_og_node']['springboard_og_grouped_content'][$key] = array(
      '#disabled' => TRUE,
      '#description' => l(t('Remove from groups'), "admin/config/group/fields/node/$key/og_group_ref/delete", array('query' => array('destination' => 'admin/config/springboard-groups'))),
    );
  }

  $form['springboard_og_node']['submit_node'] = array(
    '#type' => 'submit',
    '#value' => 'Add',
    '#submit' => array('springboard_og_settings_form_submit'),
  );
  return $form;
}

function springboard_og_entity_settings_form() {
  $form = array();
  $form['#attached']['css'][] = drupal_get_path('module', 'springboard_og') . '/css/springboard-og-admin.css';

  $options = array();
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (empty($entity_info['fieldable'])) {
      continue;
    }
    foreach ($entity_info['bundles'] as $bundle_name => $bundle) {
      // Prefix the bundle name with the entity type.
      $entity_name = check_plain("$entity_info[label] ($entity_type)");
      $options[$entity_name][$entity_type . ':' . $bundle_name] = filter_xss($bundle['label']);
    }
  }

  foreach ($options as $key => $type) {
    foreach ($type as $info => $name) {
      if (strpos($info, 'node') === FALSE && strpos($info, 'og_') === FALSE && strpos($info, 'line_item') === FALSE) {
        $entity_type_options[$info] = $name . ' (' . substr($info, 0, strpos($info, ':')) . ')';
      }
    }
  }

  $form['springboard_og_entities'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity types'),
    '#collapsible' => TRUE,
    '#description' => t('Make these entities eligible for grouping'),
  );

  $groups_enabled = springboard_og_get_enabled_types();

  $defaults = [];
  foreach ($groups_enabled as $entity_type => $bundle_group) {
    if ($entity_type != 'node') {
      foreach ($bundle_group as $bundle => $field) {
        $defaults[] = $entity_type . ':' . $bundle;
      }
    }
  }

  $form['springboard_og_entities']['springboard_og_grouped_entities'] = array(
    '#title' => t(''),
    '#type' => 'checkboxes',
    '#options' => $entity_type_options,
    '#default_value' => $defaults,
  );

  foreach ($groups_enabled as $entity_type => $bundle_group) {
    if ($entity_type != 'node') {
      foreach ($bundle_group as $bundle => $field) {
        $form['springboard_og_entities']['springboard_og_grouped_entities'][$entity_type . ':' . $bundle] = array(
          '#disabled' => TRUE,
          '#description' => l(t('Remove from groups'), "admin/config/group/fields/" . $entity_type . "/" . $bundle . "/og_group_ref/delete", array('query' => array('destination' => 'admin/config/springboard-groups'))),
        );
      }
    }
  }

  $form['springboard_og_entities']['actions'] = array(
    '#type' => 'submit',
    '#value' => 'Add',
    '#submit' => array('springboard_og_entities_settings_form_submit'),
  );

  return $form;
}

function springboard_og_settings_form_submit($form, $form_state) {
  foreach ($form_state['values']['springboard_og_grouped_content'] as $type => $status) {
    if (!empty($status)) {
      $og_field = og_fields_info('og_group_ref');
      og_create_field('og_group_ref', 'node', $type, $og_field);
    }
  }
}

function springboard_og_entities_settings_form_submit($form, $form_state) {
  foreach ($form_state['values']['springboard_og_grouped_entities'] as $key => $status) {
    if (!empty($status)) {
      list($entity_type, $bundle) = explode(':', $key);
      $og_field = og_fields_info('og_group_ref');
      og_create_field('og_group_ref', $entity_type, $bundle, $og_field);
    }
  }
}
