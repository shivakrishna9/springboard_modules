<?php
/**
 * @file
 * This template renders a webform node in 2 columns.
 */

?>

<div id="donation-form-column-wrapper" class="container-fluid">
  <div class="row-fluid">
    <div id="left" class="span6">
      <?php print render($element['body']); ?>
    </div>
    <div id="right" class="span6">
      <?php print render($element['webform']); ?>
    </div>
  </div>
</div>
