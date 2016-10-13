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
   */
  public function buildEntityFieldQuery($match = NULL, $match_operator = 'CONTAINS') {

    global $user;
    $query = parent::buildEntityFieldQuery($match, $match_operator);
    if ($user->uid == 1 || user_access('administer group') || user_access('assign content to any springboard group')) {
      unset($query->propertyConditions[0]);
    }
    return $query;
  }
}
