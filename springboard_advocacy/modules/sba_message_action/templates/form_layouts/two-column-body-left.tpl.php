<?php
/**
 * @file
 * This renders a message action form in 2 columns.
 */
?>


<div id="message-action-form-2col-column-wrapper" class="container-fluid">

  <div class="row-fluid">
    <div id="left" class="span5">
      <div id="image">
        <?php print $image; ?>
      </div>
      <div id="call-to-action">
        <?php print $body; ?>
        <p><?php print $social ?></p>
      </div>

    </div>
    <div id="right" class="span7">
      <fieldset id="take-action-wrapper" class = "form-wrapper"><legend><?php print $form_legend ?></legend>
        <div class = "fieldset-wrapper">
          <?php print $message_fieldset; ?>
          <fieldset id="user-information-wrapper" class = "form-wrapper"><legend><?php print $user_legend ?></legend>
            <div class = "fieldset-wrapper">
          <p><?php print $webform ?></p>
        </div>
      </fieldset>
      <?php print drupal_render_children($form); ?>
    </div>
    </div>
    </fieldset>
    </div>
</div>
