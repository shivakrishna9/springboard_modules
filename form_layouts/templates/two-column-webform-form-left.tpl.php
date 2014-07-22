<?php
/**
 * @file
 * This template renders a webform node in 2 columns.
 */

?>

<div id="donation-form-column-wrapper" class="container-fluid form-container">
  <div class="row-fluid form-row">
    <div id="left" class="span6 form-col">
      <?php print render($element['webform']); ?>

    </div>
    <div id="right" class="span6 form-col">
      <?php print render($element['body']); ?>
    </div>
  </div>
</div>
