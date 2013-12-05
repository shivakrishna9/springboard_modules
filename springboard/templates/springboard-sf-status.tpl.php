<?php
/**
 * @file
 * Customize form report for the modal output.
 *
 * Available variables:
 * - $status: The Salesforce connection status.
 * - $updated_time: The last time the WSDL was updated.
 * - $batch_status: The status of the latest batch.
 * - $identity: Array of Salesforce identity information. Only populated if $status is TRUE.
 */
?>
  <p><?php print $variables['status'] ? '<span class="sf-connected">' . t('Salesforce connected') . '</span>' : '<span class="sf-notconncected">' .
    t('Salesforce not connected ') . l(t('Update authentication'), 'admin/config/salesforce/authorize') . '</span>'; ?></p>
  <?php if ($status): ?>
  <p>Connected as: <?php print $identity['username'];?></p>
  <p>Organization Id: <?php print $identity['organization_id'];?></p>
  <?php endif; ?>
  <p><?php print isset($variables['batch_status']) ? $variables['batch_status'] : ''; ?></p>
  <p><?php print render($queue); ?></p>
