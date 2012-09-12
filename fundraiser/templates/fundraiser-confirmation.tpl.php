<?php

/**
 * @file
 * Customize confirmation screen after successful donation.
 *
 * This file may be renamed "fundraiser-confirmation-[nid].tpl.php" to target a
 * specific donation form on your site. Or you can leave it
 * "fundraiser-confirmation.tpl.php" to affect all donation confirmations on your
 * site.
 *
 * Available variables:
 * - $node: The node object for this donation form.
 * - $confirmation_message: The confirmation message input by the donation form author.
 * - $sid: The unique submission ID of this submission.
 */
?>

<?php if ($confirmation_message_prefix): ?>
<div class="fundraiser-confirmation-prefix">
  <?php print $confirmation_message_prefix ?>
</div>
<?php endif; ?>
<div class="fundraiser-confirmation">
  <?php if ($confirmation_message): ?>
    <?php print $confirmation_message ?>
  <?php else: ?>
    <p><?php print t('Thank you, your submission has been received.'); ?></p>
  <?php endif; ?>
</div>
<?php if ($confirmation_message_suffix): ?>
<div class="fundraiser-confirmation-suffix">
  <?php print $confirmation_message_suffix; ?>
</div>
<?php endif; ?>
<div class="share">
  <?php if ($springboard_social_share) { print $springboard_social_share; } ?>
</div>
<div class="links">
  <a href="<?php print url('node/'. $node->nid) ?>"><?php print t('Go back to the form') ?></a>
</div>