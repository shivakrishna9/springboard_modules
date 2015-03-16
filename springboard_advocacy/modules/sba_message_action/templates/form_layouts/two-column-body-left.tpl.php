<?php
/**
 * @file
 * This renders a message action form in 2 columns.
 */
?>


<div id="message-action-form-2col-column-wrapper" class="container-fluid action-body-left">
  <?php if(!empty($goal)): ?>
    <div class="row-fluid">
      <?php print $goal; ?>
    </div>
  <?php endif; ?>

  <div class="row-fluid">
    <div id="left" class="span5 message-column">
      <?php if(!empty($image)): ?>
      <div id="image">
        <?php print $image; ?>
      </div>
      <?php endif; ?>

      <div id="call-to-action">
        <?php print $body; ?>
        <p><?php print $social ?></p>
      </div>
    </div>

    <div id="right" class="span7 message-column">
      <?php print $take_action_open; ?>
      <?php print $your_message; ?>
      <?php print $your_info ?><
      <?php print drupal_render_children($form); ?>
      <?php print $take_action_close; ?>
    </div>


  </div>
</div>
