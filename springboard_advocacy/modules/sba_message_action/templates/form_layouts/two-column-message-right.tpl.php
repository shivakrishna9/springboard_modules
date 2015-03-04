<?php
/**
 * @file
 * This renders a message action form in 2 columns.
 */

?>

<?php print drupal_render($form['submitted']); ?>

<div id="message-action-form-column-wrapper" class="container-fluid">
  <div class="row-fluid">
    <div id="call-to-action" class="span8">
      <?php print $message_call_to_action; ?>
    </div>
    <div id="image" class="span4">
      <?php print $message_image; ?>
    </div>
  </div>
  <div class="row-fluid">
    <div id="left" class="span6">
      <?php print drupal_render_children($form); ?>
    </div>
    <div id="right" class="span6">
      <?php print $message_fieldset; ?>
    </div>
  </div>
</div>
