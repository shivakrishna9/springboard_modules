<?php
/**
 * @file
 * This renders a message action form in 2 columns.
 */
?>


<div id="message-action-form-2col-right-column-wrapper" class="container-fluid">
  <div class="row-fluid">
    <div id="call-to-action" class="span8">
      <?php print $body; ?>
    </div>
    <div id="image" class="span4">
      <?php print $image; ?>
    </div>
  </div>
  <fieldset id="take-action-wrapper" class = "form-wrapper"><legend><?php print $form_legend ?></legend>
    <div class = "fieldset-wrapper">
  <div class="row-fluid">
    <div id="left" class="span5">
      <fieldset id="take-action-wrapper" class = "form-wrapper"><legend><?php print $user_legend ?></legend>
        <div class = "fieldset-wrapper">

        <p><?php print $webform ?></p>
        </div>
      </fieldset>
      <?php print drupal_render_children($form); ?>

      <p><?php print $social ?></p>
    </div>
    <div id="right" class="span7">
      <?php print $message_fieldset; ?>
    </div>
    </div>
  </div>
</fieldset>
</div>
