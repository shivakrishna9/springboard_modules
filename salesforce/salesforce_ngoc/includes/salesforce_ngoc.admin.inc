<?php

/**
 * @file
 * Administration page callbacks for the ngo connect module.
 */

/**
 * Configuration form for npsp admin options.
 */
function salesforce_ngoc_admin_form($form, &$form_state) {
  $form = array();

  return system_settings_form($form);
}
