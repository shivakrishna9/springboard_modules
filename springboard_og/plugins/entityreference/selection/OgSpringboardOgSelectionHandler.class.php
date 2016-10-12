<?php


/**
 * @file
 * OG example selection handler.
 */

class OgSpringboardOgSelectionHandler extends OgSelectionHandler {
  /**
   * Overrides OgSelectionHandler::getInstance().
   */
  public static function getInstance($field, $instance = NULL, $entity_type = NULL, $entity = NULL) {
    return new OgSpringboardOgSelectionHandler($field, $instance, $entity_type, $entity);
  }

  /**
   * Overrides OgSelectionHandler::buildEntityFieldQuery().
   *
   */
  public function buildEntityFieldQuery($match = NULL, $match_operator = 'CONTAINS') {

    global $user;
    $query =  parent::buildEntityFieldQuery($match, $match_operator);
    if ($user->uid == 1 || user_access('administer group') || user_access('Administer all springboard group content')) {
      unset($query->propertyConditions[0]);
    }
    return $query;
  }

  /**
   * Get group IDs from URL or OG-context, with access to create group-content.
   *
   * @return
   *   Array with group IDs a user (member or non-member) is allowed to
   * create, or empty array.
   */
  private function getGidsForCreate() {
    if ($this->instance['entity_type'] != 'node') {
      return array();
    }

    if (!empty($this->entity->nid)) {
      // Existing node.
      return array();
    }

    if (!module_exists('entityreference_prepopulate') || empty($this->instance['settings']['behaviors']['prepopulate'])) {
      return array();
    }

    // Don't try to validate the IDs.
    if (!$ids = entityreference_prepopulate_get_values($this->field, $this->instance, FALSE)) {
      return array();
    }
    $node_type = $this->instance['bundle'];
    foreach ($ids as $delta => $id) {
      if (!is_numeric($id) || !$id || !og_user_access($this->field['settings']['target_type'], $id, "create $node_type content")) {
        unset($ids[$delta]);
      }
    }
    return $ids;
  }

  private function getDefaultIds($user_groups, $group_type, $user) {
    if ($user_groups && empty($this->instance) && $this->instance['entity_type'] == 'node') {
      // Determine which groups should be selectable.
      $node = $this->entity;
      $node_type = $this->instance['bundle'];
      $ids = array();
      foreach ($user_groups as $gid) {
        // Check if user has "create" permissions on those groups.
        // If the user doesn't have create permission, check if perhaps the
        // content already exists and the user has edit permission.
        if (og_user_access($group_type, $gid, "create $node_type content")) {
          $ids[] = $gid;
        }
        elseif (!empty($node->nid) && (og_user_access($group_type, $gid, "update any $node_type content") || ($user->uid == $node->uid && og_user_access($group_type, $gid, "update own $node_type content")))) {
          $node_groups = isset($node_groups) ? $node_groups : og_get_entity_groups('node', $node->nid);
          if (in_array($gid, $node_groups[$group_type])) {
            $ids[] = $gid;
          }
        }
      }
    }
    else {
      $ids = $user_groups;
    }
    return $ids;
  }


}
