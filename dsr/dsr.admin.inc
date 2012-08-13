<?php
/**
 * @file
 * Admin functions for the DSR module.
 */
 
/** 
 * DSR settings form.
 */
function dsr_settings() {
  $form['dsr_reconciliation_summary_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Reconciliation summary report email'),
    '#description' => t('Enter the email address where the DSR reconciliation summary report should be sent.'),
    '#default_value' => variable_get('dsr_reconciliation_summary_email', ''),
    '#weight' => -3,
    '#maxlength' => 256,
  );
  
  return system_settings_form($form);
}