<?php
/**
 * @file
 * springboard_p2p_default_rules.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function springboard_p2p_default_rules_default_rules_configuration() {
  $items = array();
  $items['rules_p2p_admin_email_private_campaign_approval'] = entity_import('rules_config', '{ "rules_p2p_admin_email_private_campaign_approval" : {
      "LABEL" : "Send admin email about requesting approval to a private campaign",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Springboard P2P" ],
      "REQUIRES" : [ "rules", "entity" ],
      "ON" : [ "commerce_cardonfile_delete" ],
      "DO" : [
        { "mail" : {
            "to" : [ "site:mail" ],
            "subject" : "User requesting approval to a private campaign",
            "message" : "A user has requested approval to a private campaign.",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  $items['rules_p2p_user_email_private_campaign_approval_request'] = entity_import('rules_config', '{ "rules_p2p_user_email_private_campaign_approval_request" : {
      "LABEL" : "Send user email about requesting approval to a private P2P campaign",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Springboard P2P" ],
      "REQUIRES" : [ "rules", "springboard_p2p" ],
      "ON" : [ "springboard_p2p_user_requests_approval" ],
      "DO" : [
        { "mail" : {
            "to" : [ "account:mail" ],
            "subject" : "Thanks for requesting approval for a private campaign.",
            "message" : "Thanks for requesting approval for a private campaign.  You will receive an email when you are approved.",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  $items['rules_p2p_user_email_private_campaign_approved'] = entity_import('rules_config', '{ "rules_p2p_user_email_private_campaign_approved" : {
      "LABEL" : "Send user email about being approved for a private campaign",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Springboard P2P" ],
      "REQUIRES" : [ "rules", "springboard_p2p" ],
      "ON" : [ "springboard_p2p_user_is_approved" ],
      "DO" : [
        { "mail" : {
            "to" : [ "account:mail" ],
            "subject" : "You have been approved",
            "message" : "You have been approved for a private campaign.",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  $items['rules_p2p_user_email_private_campaign_rejected'] = entity_import('rules_config', '{ "rules_p2p_user_email_private_campaign_rejected" : {
      "LABEL" : "Send user email about being rejected for a private campaign",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Springboard P2P" ],
      "REQUIRES" : [ "rules", "springboard_p2p" ],
      "ON" : [ "springboard_p2p_user_is_rejected" ],
      "DO" : [
        { "mail" : {
            "to" : [ "account:mail" ],
            "subject" : "You have been rejected",
            "message" : "You have been rejected for a private campaign.",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  $items['rules_p2p_user_email_registration'] = entity_import('rules_config', '{ "rules_p2p_user_email_registration" : {
      "LABEL" : "Send user email about new P2P registration",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "Springboard P2P" ],
      "REQUIRES" : [ "rules", "springboard_p2p" ],
      "ON" : [ "springboard_p2p_user_register" ],
      "DO" : [
        { "mail" : {
            "to" : [ "account:mail" ],
            "subject" : "Welcome to P2P",
            "message" : "Welcome to P2P. You can now log in and create campaigns.",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  return $items;
}
