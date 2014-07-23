<?php
/**
 * @file
 * This template renders a webform node in 2 columns.
 */

?>

<div id="donation-form-column-wrapper" class="form-layout-container">
  <div class="form-row">
    <div id="left" class="form-col">
      <?php print render($element['webform']); ?>
    </div>
    <div id="right" class="form-col">
      <?php print render($element['body']); ?>
    </div>
  </div>
</div>
