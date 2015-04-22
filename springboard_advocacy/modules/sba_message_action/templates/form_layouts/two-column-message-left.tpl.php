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
      <div id="call-to-action-call"><?php print $call; ?></div>
      <div id="call-to-action-body"> <?php print $body; ?></div>
      </div>
    <?php endif; ?>
  </div>
  <fieldset id="take-action-wrapper" class = "form-wrapper"><legend><?php print $form_legend ?></legend>
    <div class = "fieldset-wrapper">
  <div class="row-fluid">
    <div id="left" class="span7 message-column">
      <?php print $message_fieldset; ?>
    </div>
    <div id="right" class="span5 message-column">
      <fieldset id="user-information-wrapper" class = "form-wrapper"><legend><?php print $user_legend ?></legend>
        <div class = "fieldset-wrapper">
          <p><?php print $webform ?></p>
        </div>
      </fieldset>
      <?php print drupal_render_children($form); ?>
      <p><?php print $social ?></p>
    </div>
    </div>
  </div>
</fieldset>
</div>
