 <?php

/**
 * @file
 * Example tpl file for theming a single sba_message-specific theme
 *
 * Available variables:
 * - $status: The variable to theme (while only show if you tick status)
 * 
 * Helper variables:
 * - $sba_message: The SBA Message object this status is derived from
 */
?>

<div class="sba-message-status">
  <?php print '<strong>SBA Message Sample Data:</strong> ' . $sba_message_sample_data = ($sba_message_sample_data) ? 'Switch On' : 'Switch Off' ?>
</div>