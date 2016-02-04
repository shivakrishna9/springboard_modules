<?php
/**
 * @file
 * Provides integration with the rules module.
 */

/**
 * Implements hook_rules_action_info().
 */
function silverpop_transact_rules_action_info() {

  return array(
    'silverpop_transact_create_mailing' => array(
      'label' => t('Create Silverpop Transact mailing'),
      'group' => t('Silverpop Transact'),
      'parameter' => array(
        'campaign_id' => array(
          'type' => 'text',
          'label' => t('The Silverpop Campaign ID'),
          'description' => t('Enter the numeric ID for the Silverpop Automated Messages group.'),
        ),
        'recipient_email' => array(
          'type' => 'text',
          'label' => t('Recipient Email'),
          'description' => t('The recipient email.'),
        ),
        'personalization' => array(
          'type' => 'text',
          'label' => t('Personalization Tokens'),
          'description' => t('Enter a list of key|value pairs for values to be replaced in the Silverpop Transact template.'),
        ),
        'save_columns' => array(
          'type' => 'boolean',
          'label' => t('Save Columns'),
          'description' => t('Save personalization tokens to the Engage database.'),
        ),
        'html_body' => array(
          'type' => 'text',
          'label' => t('HTML Body'),
          'description' => t('Include HTML for the body of the email. This will be accessed in Silverpop via the token %%INBOUND_HTML%%'),
          'optional' => TRUE,
        ),
      ),
    ),
  );
}
