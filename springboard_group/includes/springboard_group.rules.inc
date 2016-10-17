<?php
/**
 * @file
 * Rule definitions for Springboard Group.
 */

/**
 * Implements hook_rules_configuration().
 *
 * A rule which enables bulk adding of users to a group. Uses native OG rule
 * "og_group_content_add".
 */
function springboard_group_default_rules_configuration() {
  $items['rules_springboard_group_bulk_add'] = entity_import('rules_config', '{ "rules_springboard_group_bulk_add" : {
    "LABEL" : "add_og_membership",
    "PLUGIN" : "action set",
    "REQUIRES" : [ "og", "rules" ],
    "ACCESS_EXPOSED" : "1",
    "USES VARIABLES" : { "user" : { "label" : "user", "type" : "user" } },
    "ACTION SET" : [
      { "og_group_content_add" : { "entity" : [ "user" ], "group" : [ "site:og-context--node" ] } },
      { "drupal_message" : { "message" : "User [user:name] added to group" } }
    ]
  }
}');

  return $items;
}

/**
 * Implements hook_rules_config_access().
 */
function springboard_group_rules_config_access($op, $rules_config = NULL, $account = NULL) {

  if ($op == 'update') {
    $group = og_get_entity_groups('rules_config', $rules_config);
    if (!empty($group)) {
      $gid = reset($group['node']);
      $group_context = springboard_group_get_group_context();
      if ($group_context != $gid && !user_access('administer group')) {
        return FALSE;
      }
    }
  }
}


/**
 * Implements hook_form_FORMID_alter().
 *
 * Contortions to get field api working on rules config.
 */
function springboard_group_form_rules_ui_form_edit_rules_config_alter(&$form, &$form_state, $form_id) {
  // We need the rule name to proceed.
  if (empty($form['settings']['name']['#default_value'])) {
    return;
  }

  // Get the rule config entity.
  $rule_name = $form['settings']['name']['#default_value'];
  $rule = rules_config_load($rule_name);
  if (empty($rule)) {
    // It's a custom UI managed rule.
    $rule = rules_config_load('rules_' . $rule_name);
  }

  // Add the rule enoty to the form so we can use it in validate and submit.
  if (!empty($rule) && $rule->module == 'commerce_payment' || in_array('commerce_payment', $rule->dependencies)) {

    // Rules config entities are fiedable but rules module does not attach
    // fields, yet...
    field_attach_form('rules_config', $rule, $form, $form_state);

    // Check that this entity has a groups field.
    $childs = element_children($form);
    $grouped = FALSE;
    foreach ($childs as $child) {
      if (strpos($child, 'og_group_ref') !== FALSE) {
        $grouped = TRUE;
      }
    }

    // No groups field, do not proceed.
    if (!$grouped) {
      return;
    }

    $form['rule_entity'] = array(
      '#type' => 'value',
      '#value' => $rule,
    );

    $fields = og_get_group_audience_fields('rules_config', 'rules_config', 'node', 'springboard_group');

    if (!empty($fields)) {
      springboard_group_set_single_selector($form, $fields);
    }

    $form['#validate'][] = 'springboard_group_validate_payment';
    $form['#submit'][] = 'springboard_group_submit_payment';
  }
}

/**
 * Run field attach validation.
 *
 * @param $form
 * @param $form_state
 */
function springboard_group_validate_payment($form, &$form_state) {
  field_attach_form_validate('rules_config', $form_state['values']['rule_entity'], $form, $form_state);
}

/**
 * Run field attach save functions.
 *
 * @param $form
 * @param $form_state
 */
function springboard_group_submit_payment($form, &$form_state) {
  $rule = $form_state['values']['rule_entity'];
  field_attach_submit('rules_config', $rule, $form, $form_state);

  // In function og_group() we find
  // $id = $wrapper->getIdentifier();
  // But rules.core overrides entity api's getIdentifier method and returns the
  // rule name instead!
  //$rule->name = $rule->id;
  // THis should be ok, not change the actual entity.
  field_attach_insert('rules_config', $rule);
}

