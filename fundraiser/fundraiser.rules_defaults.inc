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
      "TAGS" : [ "fundraiser" ],
      "REQUIRES" : [ "rules", "fundraiser" ],
      "ON" : [ "fundraiser_gateway_test_mode_detected" ],
      "DO" : [
        { "mail" : {
            "to" : "[site:mail]",
            "subject" : "Gateway test mode detected on [site:name]",
            "message" : "A test mode configuration has been detected on a live doantion form on [site:name].\r\n\r\nInformation as follows:\r\nLocation: [site:url] \/[node-id:value]\r\nGateway ID: [gateway-id:value]\r\nPayment type: [method-name:value]",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  return $items;
}
