<?php


/**
 * @file
 * OG selection handler.
 *
 * Entity reference selection handlers are presented as configuration
 * options in the Field management UI.
 */


/**
 * Extend OgSelectionHandler.
 *
 * If a user has administer groups permissions, place the groups he is not
 * subscribed to in the default OG select widget instead of the admin select
 * widget. We only want to present one widget to the user, not the twin
 * selection fields that OG presents by default.
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
    // Show all groups to admins in the default widget.
    if ($user->uid == 1 || user_access('administer group')) {
      unset($query->propertyConditions[0]);
    }
    return $query;
  }
}
