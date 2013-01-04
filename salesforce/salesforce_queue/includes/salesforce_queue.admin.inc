<?php

/**
 * @file
 * Administration page callbacks for the salesforce queue module.
 */

function salesforce_queue_admin_settings() {
  $form['hiyai'] = array(
    '#markup' => 'Future home of Salesforce queue settings.',
  );

  return $form;
}
