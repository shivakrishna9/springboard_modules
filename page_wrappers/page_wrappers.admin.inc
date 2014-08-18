<?php

/**
 * @file
 * Administration functions for page wrappers module.
 *
 * Provides admin functionality for the page wrappers module.
 */

/**
 * Admin configuration form for page wrappers.
 */
function page_wrappers_settings_form($form, $form_state) {
  $form['#attached']['js'][] = drupal_get_path('module', 'page_wrappers') . '/js/page_wrappers.admin.js';
  
  // Get node types for the checkbox field, but exclude page_wrapper, obviously.
  // Re-sort the array alphabetically.
  $types = node_type_get_types();
  ksort(&$types);
  unset($types['page_wrapper']);
  $options = array();
  foreach ($types as $db_name => $type) {
    $options[$db_name] = $type->name;
  }
  
  $form['page_wrapper_enabled_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Page wrapper-enabled content types'),
    '#options' => $options,
    '#default_value' => variable_get('page_wrapper_enabled_node_types', array('donation_form')),
    '#description' => t('A list of page wrappers will be available on these content types.'),
  );

  $wrappers = _page_wrappers_available_wrappers();
  $rulesets = _page_wrapper_available_rulesets();
  
  // Get enabled node types, and sort alphabetically.
  $enabled_node_types = variable_get('page_wrapper_enabled_node_types', array('donation_form'));
  ksort(&$enabled_node_types);
  $form['#page_wrapper_config_parent'] = 'page_wrappers_registration_form_settings';
  $form['#add_more_button_text'] = t('Add Another');
  
  foreach ($enabled_node_types as $type => $value) {
    if ($type === $value) {
      $type = $types[$type];

      // Set up field keys
      $fieldset = 'page_wrappers_settings_' . $type->type;
      $field_page_wrappers_more_key = 'page_wrappers_more_' . $type->type;
      $field_key = 'page_wrapper_default_config_' . $type->type;
      $field_id = 'page-wrappers-' . $type->type . '-assignments';
      
      // To use the same ajax function as the node form we check for the node value.
      // Get the default values for the node type. Add fallback page wrapper settings
      // if nothing has yet been set for this node type.
      if (!empty($form_state['node']->page_wrappers[$field_key])) {
        $configurations = $form_state['node']->page_wrappers[$field_key];
      }
      else {
        $configurations = array_values(variable_get($field_key, array()));
        if (empty($configurations)) {
          $configurations = array_values(variable_get('page_wrapper_default_node_config', array()));
        }
      }
                      
      // Get a count of the wrappers, the wrappers_count value is passed from the ajax function
      if (isset($form_state['wrappers_count'][$field_key])) {
        $wrappers_count = $form_state['wrappers_count'][$field_key];
      }
      else {
        $wrappers_count = empty($configurations) ? 1 : count($configurations);
      }
            
      // Set up the fieldsets and fields now.
      $form[$fieldset] = array(
        '#type' => 'fieldset',
        '#title' => t('Default !node_type Configuration', array('!node_type' => $type->name)),
        '#description' => t('Configure the Page Wrapper settings for new <strong>!node_type</strong> nodes.', array('!node_type' => $type->name)),
        '#tree' => FALSE,
      );
      // Container for just the poll choices.
      $form[$fieldset][$field_key] = array(
        '#prefix' => '<div id="' . $field_id . '">',
        '#suffix' => '</div>',
        '#theme' => 'page_wrappers_node_assignments',
      );
      // Add existing wrapper configurations
      $delta = 0;
      $weight = 0;
      if (!empty($configurations)) {
        $delta = count($configurations);
        foreach ($configurations as $pwid => $config) {
          $key = 'pwid:' . $pwid;
          $form[$fieldset][$field_key][$key] = _page_wrappers_assignment_fields($key, $config, $wrappers, $rulesets, $config['weight'], $wrappers_count, $field_key);
          $weight = max($config['weight'], $weight);
        }
      }
    
      // Add more
      $existing_delta = $delta;
      for ($delta; $delta < $wrappers_count; $delta++) {
        $key = 'new:' . ($delta - $existing_delta);
        $weight++;
        $form[$fieldset][$field_key][$key] = _page_wrappers_assignment_fields($key, array(), $wrappers, $rulesets, $weight, $wrappers_count, $field_key);
      }
      
      // If we have configured rulesets add the add more button
      if (!empty($rulesets)) {
        $form[$fieldset][$field_page_wrappers_more_key] = array(
          '#type' => 'submit',
          '#value' => t('Add !node_type Option', array('!node_type' => $type->name)),
          '#weight' => 1,
          '#limit_validation_errors' => array(
            'fieldset' => array($fieldset), 
            'field_key' => array($field_key)
          ),
          '#submit' => array('page_wrapper_add_assignment_submit'),
          '#ajax' => array(
            'callback' => '_page_wrappers_add_js',
            'wrapper' => $field_id,
            'effect' => 'fade',
          ),
        );
        // Add a flag to the page wrapper fieldset that we have rulesets
        $form[$fieldset][$field_key]['#has_rulesets'] = TRUE;
      }
    }
  }

  if (module_exists('og')) {
    $form['page_wrappers_filter_by_og'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter available Page Wrappers by group'),
      '#description' => t('When checked, the list of available page wrappers displayed on node edit forms will be limited to page wrappers that are assigned to one or more groups the user is enrolled in.'),
      '#default_value' => variable_get('page_wrappers_filter_by_og', FALSE),
    );
  }

  $form['#validate'][] = 'page_wrappers_settings_form_validate';
    
  return system_settings_form($form);
}

/**
 * Validate handler for the admin form to remove empty page wrapper default node configurations.
 *
 * Iterate through all submitted fields. If the field is an array, iterate through them
 * and unset() any empty field value (with an empty 'wrapper_nid'.
 */
function page_wrappers_settings_form_validate($form, &$form_state) {
  foreach ($form_state['values'] as $field_key => $field_values) {
    if (is_array($field_values)) {
      foreach ($field_values as $key => $values) {
        if (is_array($values) && isset($values['wrapper_nid']) && (empty($values['wrapper_nid']) || $values['wrapper_nid'] == 0)) {
          unset($form_state['values'][$field_key][$key]);
        }
      }
    } 
  }
}
