<?php
/**
 * @file
 * This renders a message action form in 2 columns.
 */
?>


<div id="message-action-form-2col-column-wrapper" class="container-fluid action-message-left">
  <div class="row-fluid">
    <?php if(!empty($image)): ?>
      <div id="call-to-action" class="span8">
      <?php print $body; ?>
    </div>
      <div id="image" class = "span4">
        <?php print $image; ?>
      </div>
    <?php else: ?>
    <div id="call-to-action" class="span12">
        <?php print $body; ?>
      </div>
    <?php endif; ?>
  </div>
  <?php print $take_action_open; ?>

  <div class="row-fluid">
    <div id="left" class="span7 message-column">
      <?php print $your_message; ?>
    </div>
    <div id="right" class="span5 message-column">
          <?php print $your_info ?>
      <?php print drupal_render_children($form); ?>
      <p><?php print $social ?></p>
    </div>
    </div>
  <?php print $take_action_close; ?>
</div>
