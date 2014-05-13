<?php

/**
 * @file
 */

/**
 * Admin settings form for ACLU Confirmation Alias.
 *
 * Provides interface to bulk add aliases to existing webforms.
 */
function webform_confirmation_alias_admin_settings() {
  // set page title.
  $form['description'] = array(
    '#type' => 'markup',
    '#value' => '<p>' . t('This module generates url aliases for webform confirmation urls. Any webforms with configured confirmation urls that existed before this
      module was installed will not have aliases created by this module. Click on the button below to automatically generate aliases for all existing webforms.') . '</p>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate aliases'),
  );
  $form['#submit'][] = 'webform_confirmation_alias_generate_aliases';
  return $form;
}

/**
 * Bulk-add url aliases for webform confirmation redirect urls.
 * Get list of webforms that need url aliases for confirmation pages.
 * Criteria:
 * - webform is set to redirect to a custom url.
 * - redirect url ends with /thank-you.
 * - there is currently no alias for this redirect url.
 */
function webform_confirmation_alias_generate_aliases() {
  
  $results = db_query("
    SELECT
      nid,
      redirect_url
    FROM {webform}
    WHERE
      redirect_url LIKE '%thank-you'
    AND redirect_url NOT IN (
      SELECT src FROM url_alias WHERE src like '%thank-you'
   )");
   
  $results = db_select('webform', 'w')
    ->fields('w', array('nid', 'redirect_url'))
    ->condition(db_and()
      ->condition('redirect_url', db_like('%thank-you'), 'LIKE')
      ->condition()
  	)
    
  // Generate alias for redirect url for each webform in the list.
  while ($webform = db_fetch_object($results)) {
    $src = 'node/' . $webform->nid;
    // Crib off of existing url alias. If none exists for this node, skip it.
    $base_alias = db_result(db_query("SELECT dst FROM {url_alias} WHERE src = '%s'", $src));
    if ($base_alias) {
      $new_dst = $base_alias . '/thank-you';
      $settings = array(
        'pid' => '',
        'src' => $src . '/thank-you',
        'dst' => $new_dst,
        'language' => 'en',
      );
      drupal_write_record('url_alias', $settings);
    }
  }
  drupal_set_message(t('URL aliases have been added for existing webform redirect urls.'));
}
