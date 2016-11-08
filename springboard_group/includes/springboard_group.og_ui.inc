<?php

/**
 * Implements hook_og_ui_get_group_admin().
 *
 * Remove the permissions and roles links for the group node's group tab.
 */
function springboard_group_og_ui_get_group_admin_alter(&$data, $context) {
  if ($context['entity_type'] == 'node') {
    $node = node_load($context['etid']);
    if ($node->type == 'springboard_group') {
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Change the default text on og_group_ref field deletion confirm page. That is
 * when making an entity un-groupable.
 */
function springboard_group_form_og_ui_field_delete_form_alter(&$form, &$form_state) {
  if (isset($_GET['destination']) && $_GET['destination'] == 'admin/springboard/groups/settings') {
    $form['actions']['cancel']['#markup'] = l(t('cancel'), 'admin/springboard/groups/settings');
    $form['description']['#markup'] = t('All group associations for the @type content type will be deleted. This cannot be undone.', array('@type' => arg(5)));
    $form['#submit'][] = 'springboard_group_form_og_ui_field_delete_submit';
  }
}

/**
 * Delete multiple choice variable when deleting its owner field.
 */
function springboard_group_form_og_ui_field_delete_submit($form, $form_state) {
  if ($_GET['destination'] == 'admin/springboard/groups/settings') {
    variable_del('og_' . $form_state['values']['bundle'] . '_multiple');
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Remove the "request" field from the add user form. Not needed.
 */
function springboard_group_form_og_ui_add_users_alter(&$form, $form_state) {
  $form['membership_fields']['og_membership_request']['#access'] = FALSE;;
}

/**
 * Replace OG UI's default landing page with our own.
 *
 * If the group bundle is "springboard_group" replace the OG UI-provided
 * landing page (a set of links to the user management function) with the user
 * management page itself. Additional management links are converted to
 * secondary menu tabs in springboard_group_menu().
 *
 * @param string $entity_type
 *   The bundle type.
 * @param int $etid
 *   The node id.
 *
 * @return array
 *   The page render array.
 */
function springboard_group_og_ui_group_admin_overview($entity_type, $etid) {
  $content = '';
  og_set_breadcrumb($entity_type, $etid);
  $items = og_ui_get_group_admin($entity_type, $etid);
  if ($entity_type == 'node') {
    $node = node_load($etid);
  }
  // Not a springboard group, proceed as normal.
  if (empty($node) || $node->type != 'springboard_group') {
    foreach ($items as &$item) {
      $item['href'] = "group/$entity_type/$etid/" . $item['href'];
      $item['localized_options'] = array();
    }
    $form['admin_block_content'] = array(
      '#theme' => 'admin_block_content',
      '#content' => $items,
    );
    $content = $form;
  }
  // Springboard group, replace the page with user management.
  elseif (og_ui_user_access_group('manage members', $entity_type, $etid)) {
    $content = og_ui_admin_account($entity_type, $etid);
  }
  return $content;
}
