<?php
/**
 * @file
 * Customize form report for the modal output.
 *
 * Available variables:
 * - $status: The Salesforce connection status.
 * - $updated_time: The last time the WSDL was updated.
 * - $batch_status: The status of the latest batch.
 */
?>
<div class="springboard-pane" id="salesforce-status-pane">
  <p><?php print isset($variables['status']) ? t('Salesforce Connected') : t('Salesforce not connected'); ?></p>
  <p>Last WSDL Update: <?php print isset($variables['updated_time']) ? date('Y-m-d g:i:sA', $updated_time) : t('Unknown'); ?></p>
  <p><?php print isset($variables['batch_status']) ? $variables['batch_status'] : ''; ?></p>
  <div id="springboard-sync"><span>Run Springboard Sync</span>
  <?php print l('<img src="' . base_path() . drupal_get_path('module', 'springboard') . '/images/sync.png">',
    'springboard_sync', array('attributes' => array('class' => 'anchor-class'), 'html' => TRUE)); ?> 
  </div>
</div>
