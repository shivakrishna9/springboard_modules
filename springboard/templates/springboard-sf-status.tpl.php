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
  <p><?php print $variables['status'] ? '<span class="sf-connected">' . t('Salesforce connected') . '</span>' : '<span class="sf-notconncected">' .
    t('Salesforce not connected ') . l(t('Update authentication'), 'admin/config/services/salesforce') . '</span>'; ?></p>
  <p><?php print isset($variables['batch_status']) ? $variables['batch_status'] : ''; ?></p>
  <p><?php print render($queue); ?></p>
</div>
