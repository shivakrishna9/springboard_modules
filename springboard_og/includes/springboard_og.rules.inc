<?php

/**
 * Implements hook_rules_configuration().
 *
 * A rule which enables builk adding of users to a group. Uses native OG rule
 * "og_group_content_add".
 */
function springboard_og_default_rules_configuration() {
  $items['rules_springboard_og_bulk_add'] = entity_import('rules_config', '{ "rules_springboard_og_bulk_add" : {
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
