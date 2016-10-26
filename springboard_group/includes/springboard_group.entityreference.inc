<?php

require_once drupal_get_path('module', 'entityreference') . '/plugins/selection/EntityReference_SelectionHandler_Generic.class.php';


/**
 * A generic Entity handler.
 *
 * The generic base implementation has a variety of overrides to workaround
 * core's largely deficient entity handling.
 */
class SpringboardGroup_EntityReference_SelectionHandler_Generic extends EntityReference_SelectionHandler_Generic {

  /**
   * Implements EntityReferenceHandler::getInstance().
   */
  public static function getInstance($field, $instance = NULL, $entity_type = NULL, $entity = NULL) {
    $target_entity_type = $field['settings']['target_type'];
    // Check if the entity type does exist and has a base table.
    $entity_info = entity_get_info($target_entity_type);
    if (empty($entity_info['base table'])) {
      return EntityReference_SelectionHandler_Broken::getInstance($field, $instance);
    }

    // Springboard Group:
    // Skip the generic node processor that prevents unpusblished nodes
    // from being referenced.
    if ($field['field_name'] == 'field_sba_action_id') {
      return new EntityReference_SelectionHandler_Generic($field, $instance, $entity_type, $entity);
    }
    // Otherwise resume standard entity reference behavior.
    elseif (class_exists($class_name = 'EntityReference_SelectionHandler_Generic_' . $target_entity_type)) {
      return new $class_name($field, $instance, $entity_type, $entity);
    }
    else {
      return new EntityReference_SelectionHandler_Generic($field, $instance, $entity_type, $entity);
    }
  }
}


