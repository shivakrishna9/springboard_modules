<?php

function _fundraiser_sustainers_upgrade_add_example() {
  $node = new stdClass();
  $node->is_new = 1;
  $node->type = 'sustainers_upgrade_form';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->title = 'Default Sustainers Upgrade Form';
  $node->status = 1;
  $node->promote = 0;
  $node->body[LANGUAGE_NONE][0]['value'] = 'Thank you [donation:first_name] [donation:last_name]  [sustainer_upgrade:name_or_signout] for upgrading your previous donation of  [donation:currency:symbol][donation:amount].

Your original [donation:frequency] donation was made on [donation:payment-transaction:created] with your card ending in [donation:card_number].

You have [sustainer_upgrade:charges_remaining]  charges remaining in the amount of [sustainer_upgrade:next_charge_amount_formatted].

To upgrade your remaining recurring donations to [sustainer_upgrade:upgrade_amount_formatted], click confirm below.';
  $node->body[LANGUAGE_NONE][0]['format'] = 'filtered_html';
  $node->field_sustainers_upgrade_name[LANGUAGE_NONE][0]['value'] = "Default Sustainers Upgrade Form";
  $node->field_sustainers_upgrade_fail[LANGUAGE_NONE][0]['value'] = "Donation upgrade can not be completed.";
  $node->webform = array(
    'confirmation' => 'Hi [sustainer_upgrade:name_or_signout]',
    'confirmation_format' => 'filtered_html',
    'redirect_url' => '<confirmation>',
    'teaser' => 0,
    'block' => 0,
    'allow_draft' => 0,
    'auto_save' => 0,
    'submit_notice' => 1,
    'submit_text' => t('Confirm'),
    'submit_limit' => -1,
    'submit_interval' => -1,
    'total_submit_limit' => -1,
    'total_submit_interval' => -1,
    'status' => 1,
    'record_exists' => '',
    'roles' => array(
      '0' => 1,
      '1' => 2,
    ),
    'emails' => array(),
    'components' => array(),
  );
  $email_node = fundraiser_sustainers_upgrade_email_wrapper_add();
  variable_set('fundraiser_sustainers_upgrade_default_email_wrapper', $email_node->nid);

  $node->webform['emails'][1] = array(
    'email' => 'default',
    'template' => 'default',
    'subject' => $email_node->email_wrapper_subject[LANGUAGE_NONE][0]['value'],
    'from_name' => $email_node->email_wrapper_from_name[LANGUAGE_NONE][0]['value'],
    'from_address' => $email_node->email_wrapper_from_email[LANGUAGE_NONE][0]['value'],
    'html' => 0,
    'attachments' => 0,
    'excluded_components' => array(),
  );

  $node->is_webform_user = 0;
  $node->send_new_user_email = 0;
  node_save($node);
  $node = node_load($node->nid);
  variable_set('fundraiser_sustainers_upgrade_default_form', $node->nid);
  // Save confirmation details.
  $confirmation['nid'] = $node->nid;
  $confirmation['confirmation_page_title'] = 'xxx';
  _webform_confirmations_update($confirmation);
  // Save email wrapper details.
  foreach ($node->webform['components'] as $component) {
    if ($component['form_key'] == 'fsu_confirmation_email') {
      $cid = $component['cid'];
      $node->webform['emails'][1]['email'] = $cid;
    }
  }
  // One last save.
  node_save($node);

  drupal_set_message(t('A default sustainers upgrade form has been created.'));
}

function fundraiser_sustainers_upgrade_email_wrapper_add() {
  $node = new stdClass();
  $node->is_new = 1;
  $node->type = 'email_wrapper';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->title = 'Default Sustainers Upgrade Email Wrapper';
  $node->status = 0;
  $node->promote = 0;
  $node->email_wrapper_from_name[LANGUAGE_NONE][0] = array(
    'value' => 'Springboard',
    'format' => '',
    'safe_value' => 'Springboard',
  );
  $node->email_wrapper_from_email[LANGUAGE_NONE][0] = array(
    'value' => 'example-from@example.com',
    'format' => '',
    'safe_value' => 'example-from@example.com',
  );
  $node->email_wrapper_reply_email[LANGUAGE_NONE][0] = array(
    'value' => 'example-to@example.com',
    'format' => '',
    'safe_value' => 'example-to@example.com',
  );
  $node->email_wrapper_subject[LANGUAGE_NONE][0] = array(
    'value' => 'Your sustaining donation upgrade',
    'format' => '',
    'safe_value' => 'Your sustaining donation upgrade',
  );
  $node->email_wrapper_html_template[LANGUAGE_NONE][0] = array(
    'value' => 'Thank you' . "\n" .
      "\n" .
      '%html_message' . "\n" .
      "\n" .
      '-----' . "\n" .
      'Springboard Example',
    'summary' => '',
    'format' => 'filtered_html',
    'safe_value' => '<p></p>' . "\n" .
      '<p>%html_message</p>' . "\n" .
      '<p>-----<br>' . "\n" .
      'Springboard Example</p>',
  );
  $node->email_wrapper_html_message[LANGUAGE_NONE][0] = array(
    'value' => 'Hi [sustainer_upgrade:name_or_signout]. Thank you for upgrading your sustaining donation by [sustainer_upgrade:upgrade_amount_formatted]',
    'summary' => '',
    'format' => 'filtered_html',
    'safe_value' => '<p>Hi [sustainer_upgrade:name_or_signout]. Thank you for upgrading your sustaining donation by [sustainer_upgrade:upgrade_amount_formatted]</p>',
  );
  $node->email_wrapper_text_template[LANGUAGE_NONE][0] = array(
    'value' => 'Thank You' . "\n" .
      "\n" .
      '%text_message' . "\n" .
      "\n" .
      '-----' . "\n" .
      'Springboard Example',
    'format' => '',
    'safe_value' => 'xxx' . "\n" .
      "\n" .
      '%text_message' . "\n" .
      "\n" .
      '-----' . "\n" .
      'Springboard Example',
  );
  $node->email_wrapper_text_message[LANGUAGE_NONE][0] = array(
    'value' => 'Hi [sustainer_upgrade:name_or_signout]. Thank you for upgrading your sustaining donation to [sustainer_upgrade:upgrade_amount_formatted]',
    'format' => 'filtered_html',
    'safe_value' => '<p>Hi [sustainer_upgrade:name_or_signout]. Thank you for upgrading your sustaining donation to [sustainer_upgrade:upgrade_amount_formatted]</p>',
  );
  node_save($node);
  // Save extras.
  drupal_set_message(t('A sustainers upgrade email wrapper has been created.'));
  return $node;
}
