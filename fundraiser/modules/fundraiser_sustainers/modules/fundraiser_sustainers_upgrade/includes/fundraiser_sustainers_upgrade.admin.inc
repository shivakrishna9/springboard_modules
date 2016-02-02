<?php

/**
 * Page callback for the admin form.
 */

function fundraiser_sustainers_upgrade_settings_page() {
  $build = array();
  $build['content'] = array(
    'page_description' => array(
      '#id' => 'springboard-advocacy-settings-page',
      '#type' => 'markup',
      '#markup' => '<p>' . t('Configure settings for Fundraiser Sustainers Upgrade') . '</p>',
      '#prefix' => '<div class="springboard-advocacy-settings-page">',
      '#suffix' => '</div>',
    ),
    'fundraiser_sustainers_upgrade_settings_form' => drupal_get_form('fundraiser_sustainers_upgrade_settings_form'),
  );
  return $build;
}

/**
 * Configuration settings form for Springboard advocacy.
 */
function fundraiser_sustainers_upgrade_settings_form() {
  $form = array();
  return system_settings_form($form);
}