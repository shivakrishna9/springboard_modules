<?php
/**
 * @file
 * Config functions for Springboard Group.
 */

/**
 * Create the groups admin page.
 *
 * @return array
 *   A page render array.
 */
function springboard_group_settings_page() {

  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Choose which content types and entity types are eligible to be grouped.') . '</p>',
      '#prefix' => '<div class="springboard-groups-settings-page">',
      '#suffix' => '</div>',
    ),
    //'springboard_group_system_settings_form' => drupal_get_form('springboard_group_system_settings_form'),
    'springboard_group_node_settings_form' => drupal_get_form('springboard_group_node_settings_form'),
    'springboard_group_entity_settings_form' => drupal_get_form('springboard_group_entity_settings_form'),

  );
  return $build;
}

/**
 * Build a form listing group-eligible node types.
 *
 * @return array
 *   The eligible node type form.
 */
function springboard_group_system_settings_form() {
  $form = array();
  $form['springboard_group_system_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
    '#collapsible' => TRUE,
    '#description' => '',
  );

  $form['springboard_group_system_settings']['springboard_group_name'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('springboard_group_name', t('Springboard Groups')),
    '#title' => t('Group designation'),
    '#description' => 'Enter the string used to refer to groups in the Admin UI; e.g. "Chapters", "Affiliates", "Groups"',
  );

  $form['springboard_group_system_settings']['submit_settings'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
    '#submit' => array('system_settings_form_submit'),
  );

  return $form;
}

/**
 * Build a form listing group-eligible node types.
 *
 * @return array
 *   The eligible node type form.
 */
function springboard_group_node_settings_form() {
  $form = array();

  $form['#attached']['css'][] = drupal_get_path('module', 'springboard_group') . '/css/springboard-og-admin.css';

  $form['springboard_group_node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content types'),
    '#collapsible' => TRUE,
    '#description' => t('Make these content types eligible for grouping'),
    '#tree' => TRUE,
  );

  // Get all node types. Remove the springboard_group node type from the list.
  $content_type_options = [];
  $options = node_type_get_types();
  foreach ($options as $key => $type) {
    if ($type->type != 'springboard_group') {
      $content_type_options[$type->type] = $type->name;
    }
  }

  // Get the already groups-enabled node types.
  $groups_enabled_types = springboard_group_get_grouped_types();
  $node_defaults = !empty($groups_enabled_types['node']) ? array_keys($groups_enabled_types['node']) : [];
  // Build the checkboxes.
  foreach ($content_type_options as $type => $name) {
    $form['springboard_group_node'][$type][$type] = array(
      '#title' => $name,
      '#type' => 'checkbox',
      '#default_value' => in_array($type, $node_defaults),
    );
    $disallowed = array('sba_message_action', 'sba_social_action');
    if (!in_array($type, $disallowed)) {
      $form['springboard_group_node'][$type]['og_' . $type . '_multiple'] = array(
        '#title' => 'Allow multiple group selections.',
        '#type' => 'checkbox',
        '#default_value' => variable_get('og_' . $type . '_multiple', 0),
        '#attributes' => array('class' => array('multiple')),
        '#states' => array(
          'visible' => array(
            ':input[name*="' . $type . ']"]' => array('checked' => TRUE),
          ),
        ),
      );
    }
    // Disable the checkboxes on the already enabled node types and
    // add a link to remove group field.
    if (in_array($type, $node_defaults)) {
      $og_field = $groups_enabled_types['node'][$type][0];
      $form['springboard_group_node'][$type][$type]['#disabled'] = TRUE;
      $form['springboard_group_node'][$type][$type]['#description'] = l(t('Remove from groups'), "admin/config/group/fields/node/$type/" . $og_field . "/delete", array('query' => array('destination' => 'admin/springboard/groups/settings')));
    }
  }

  $form['springboard_group_node']['submit_node'] = array(
    '#type' => 'submit',
    '#value' => 'Add',
    '#submit' => array('springboard_group_settings_form_submit'),
  );

  return $form;
}

/**
 * Build a form listing group-eligible entites.
 *
 * @return array
 *   The eligible entity and node type forms.
 */
function springboard_group_entity_settings_form() {

  $form = array();
  $form['#attached']['css'][] = drupal_get_path('module', 'springboard_group') . '/css/springboard-og-admin.css';

  // Get an array of fieldable entities.
  $fieldable_entities = array();
  foreach (entity_get_info() as $entity_type => $entity_info) {
    if (empty($entity_info['fieldable'])) {
      continue;
    }
    foreach ($entity_info['bundles'] as $bundle_name => $bundle) {
      // Prefix the bundle name with the entity type.
      $entity_name = check_plain("$entity_info[label] ($entity_type)");
      $fieldable_entities[$entity_name][$entity_type . ':' . $bundle_name] = filter_xss($bundle['label']);
    }
  }

  // Filter out some entities we don't want in the list.
  $entity_type_options = [];
  foreach ($fieldable_entities as $key => $type) {
    foreach ($type as $info => $name) {
      if (strpos($info, 'node') === FALSE && strpos($info, 'og_') === FALSE && strpos($info, 'line_item') === FALSE && strpos($info, 'user') === FALSE && strpos($info, 'user') === FALSE) {
        $entity_type_options[$info] = $name . ' (' . substr($info, 0, strpos($info, ':')) . ')';
      }
    }
  }

  $form['springboard_group_entities'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity types'),
    '#collapsible' => TRUE,
    '#description' => t('Make these entities eligible for grouping'),
    '#tree' => TRUE,
  );

  // Get the already-enabled entities.
  $groups_enabled_types = springboard_group_get_grouped_types();
  $entity_defaults = !empty($groups_enabled_types) ? $groups_enabled_types : [];
  $group_enabled_entities = [];
  foreach ($entity_defaults as $entity_type => $bundle_group) {
    if ($entity_type != 'node') {
      foreach ($bundle_group as $bundle => $field) {
        $group_enabled_entities[] = $entity_type . ':' . $bundle;
      }
    }
  }

  $disallowed = array(
    'sba_message:sba_message_action_message',
    'taxonomy_term:tags',
    'taxonomy_term:sba_legislative_issues',
    'taxonomy_term:sba_organization_issues',
    'commerce_cardonfile:commerce_cardonfile',
    'commerce_customer_profile:billing',
    'commerce_order:commerce_order',
  );

  foreach ($entity_type_options as $type => $name) {
    if (!in_array($type, $disallowed)) {
      $name = str_replace('Rules configuration', 'Payment Gateways', $name);
      $form['springboard_group_entities'][$type][$type] = array(
        '#title' => $name,
        '#type' => 'checkbox',
        '#default_value' => in_array($type, $group_enabled_entities),
      );
    }

    $disallowed_multiple = array(
      'rules_config:rules_config',
      'commerce_cardonfile:commerce_cardonfile',
      'commerce_customer_profile:billing',
      'commerce_order:commerce_order',
    );

    if (!in_array($type, $disallowed) && !in_array($type, $disallowed_multiple)) {
      list(, $bundle) = explode(':', $type);
      $form['springboard_group_entities'][$type]['og_' . $type . '_multiple'] = array(
        '#title' => 'Allow multiple group selections.',
        '#type' => 'checkbox',
        '#default_value' => variable_get('og_' . $bundle . '_multiple', 0),
        '#attributes' => array('class' => array('multiple')),
        '#states' => array(
          'visible' => array(
            ':input[name*="' . $type . ']"]' => array('checked' => TRUE),
          ),
        ),
      );
    }
  }

  // Disable the checkboxes on types that are already enabled.
  // Add a link to remove the group field.
  foreach ($entity_defaults as $entity_type => $bundle_group) {
    if ($entity_type != 'node') {
      foreach ($bundle_group as $bundle => $field) {
        $form['springboard_group_entities'][$entity_type . ':' . $bundle][$entity_type . ':' . $bundle]['#disabled'] = TRUE;
        $form['springboard_group_entities'][$entity_type . ':' . $bundle][$entity_type . ':' . $bundle]['#description'] = l(t('Remove from groups'), "admin/config/group/fields/" . $entity_type . "/" . $bundle . "/" . $field[0] . "/delete", array('query' => array('destination' => 'springboard/groups/settings')));
      }
    }
  }

  $form['springboard_group_entities']['actions'] = array(
    '#type' => 'submit',
    '#value' => 'Add',
    '#submit' => array('springboard_group_entities_settings_form_submit'),
  );

  return $form;
}

/**
 * Submits the groupable node type form.
 */
function springboard_group_settings_form_submit($form, $form_state) {
  foreach ($form_state['values']['springboard_group_node'] as $node_type => $status) {
    if ($node_type == 'submit_node') {
      continue;
    }
    if (!empty($status[$node_type]) && !empty($form_state['input']['springboard_group_node'][$node_type])) {
      springboard_group_add_group_reference_field($node_type, 'node');
    }
    if (isset($form_state['values']['springboard_group_node'][$node_type]['og_' . $node_type . '_multiple'])) {
      variable_set('og_' . $node_type . '_multiple', $form_state['values']['springboard_group_node'][$node_type]['og_' . $node_type . '_multiple']);
    }
  }
}

/**
 * Submits the groupable entity type form.
 */
function springboard_group_entities_settings_form_submit($form, $form_state) {
  foreach ($form_state['values']['springboard_group_entities'] as $key => $status) {
    if ($key == 'actions') {
      continue;
    }
    list($entity_type, $bundle) = explode(':', $key);
    if (!empty($status[$key]) && !empty($form_state['input']['springboard_group_entities'][$key])) {
      springboard_group_add_group_reference_field($bundle, $entity_type);
    }
    if ($key != 'actions') {
      variable_set('og_' . $bundle . '_multiple', $form_state['values']['springboard_group_entities'][$key]['og_' . $key . '_multiple']);
    }
  }
}

/**
 * Creates the og reference field.
 *
 * @param string $bundle
 *   The bundle name.
 * @param string $type
 *   The entity type name.
 */
function springboard_group_add_group_reference_field($bundle, $type) {

  $og_field = og_fields_info('og_group_ref');
  $og_field['field']['settings']['target_bundles'] = array('springboard_group' => 'springboard_group');
  $og_field['instance']['settings']['behaviors']['og_widget']['admin']['widget_type'] = 'options_select';
  $og_field['field']['settings']['handler'] = 'og_springboard_og';
  $og_field['instance']['display']['default']['type'] = 'hidden';
  $og_field['instance']['display']['default']['label'] = 'hidden';
  $og_field['instance']['display']['teaser']['type'] = 'hidden';
  $og_field['instance']['display']['teaser']['label'] = 'hidden';

  // Enable Entity reference prepopulate.
  $og_field['instance']['settings']['behaviors']['prepopulate'] = array(
    'status' => TRUE,
    'action' => 'none',
    'fallback' => 'none',
    'skip_perm' => FALSE,
  );
  og_create_field('og_group_ref_' . substr($bundle, 0, 19), $type, $bundle, $og_field);
  //@todo update og_role_permission.
}
