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
      "REQUIRES" : [ "rules", "springboard_p2p" ],
      "ON" : [ "springboard_p2p_user_requests_approval" ],
      "DO" : [
        { "mail" : {
            "to" : [ "site:mail" ],
            "subject" : "User approval for private campaign",
            "message" : "A user, [account:name], [account:mail] has requested permission to participate in the following campaign: [campaign:title], [campaign:url].\\r\\n\\r\\nTo approve the account please visit the approval queue: https:\\/\\/p2p.qa.jacksonriverdev.com\\/admin\\/springboard\\/p2p\\/approval\\/user.",
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
            "subject" : "Thanks for your interest",
            "message" : "Dear [account:sbp-first-name]:\\r\\n\\r\\nThanks for your interest in participating in our campaign. We\\u0027ll be in touch soon. \\r\\n\\r\\nIn the meantime, access your dashboard and check out other campaigns here: http:\\/\\/springboard_p2p\\/dashboard\\/%uid",
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
            "message" : "Dear [account:sbp-first-name]:\\r\\n\\r\\nYour account has been approved and you can start building your peer to peer campaign. Click here to create you campaign: [node\\/add\\/p2p-personal-campaign?campaign=%nid] \\r\\n",
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
            "subject" : "Your registration",
            "message" : "Dear [account:sbp_first_name]:\\r\\n\\r\\nWe can\\u0027t accept your registration at this time, but check out [site:url] for other ways you can get involved. ",
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
            "subject" : "Thanks for registering",
            "message" : "Dear [account:sbp-first-name]:\\r\\n\\r\\nThanks for registering. You can go here to create you campaign:  [campaign:edit-url]\\r\\n",
            "language" : [ "" ]
          }
        }
      ]
    }
  }');
  return $items;
}
