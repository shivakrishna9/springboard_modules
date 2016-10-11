<?php

/**
 * @file
 * Administration page callbacks for the salesforce nonprofit starter pack module.
 */

/**
 * Page callback for the admin form.
 */
function salesforce_ngoc_admin_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'salesforce-ngoc-admin-helptext-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for NGO Connect.') . '</p>',
      '#prefix' => '<div class="ngoc-admin-helptext">',
      '#suffix' => '</div>',
    ),
    'salesforce_ngoc_admin_form' => drupal_get_form('salesforce_ngoc_admin_form'),
  );
  return $build;
}

/**
 * Configuration form for ngoc admin options.
 */
function salesforce_ngoc_admin_form($form, &$form_state) {
  $form = array();


  return $form;
}

/**
 * Validation callback for configuration form.
 */
function salesforce_ngoc_admin_form_validate($form, &$form_state) {

}

/**
 * Submit callback for the configuration form.
 */
function salesforce_ngoc_admin_form_submit($form, &$form_state) {

}
