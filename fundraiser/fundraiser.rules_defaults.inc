<?php
/**
 * Implements hook_default_rules_configuration().
 */
function fundraiser_default_rules_configuration() {
  $items = array();
  $items['rules_gateway_test_mode_alert'] = entity_import('rules_config', '{
      "rules_gateway_test_mode_alert" : {
      "LABEL" : "Gateway Test Mode Alert",
      "PLUGIN" : "reaction rule",
      "ACTIVE" : false,
      "TAGS" : [ "fundraiser" ],
      "REQUIRES" : [ "rules", "fundraiser" ],
      "ON" : [ "fundraiser_gateway_test_mode_detected" ],
      "DO" : [
        { "mail" : {
            "to" : "[site:mail]",
            "subject" : "Gateway test mode detected on [site:name]",
            "message" : "A test mode gateway configuration has been detected on a live donation form on [site:name].\r\n\r\nInformation as follows:\r\n\r\nLocation: [site:url]\r\nGateway ID: [gateway-id:value]\r\nPayment type: [gateway-method-name:value]\r\nCurrent status: [gateway-status:value]\r\n\r\nThis problem is affecting donations made on nodes with the following IDs: [gateway-node-id:value]",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  return $items;
}
