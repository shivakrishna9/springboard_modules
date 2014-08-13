<?php
/**
 * @file
 * This template renders a petition enabled node in 2 columns.
 */

?>

<div id="webform-form-column-wrapper" class="form-layout-container form-right">
  <div class="form-row">
    <div id="left" class="form-col">
      <h1 class="form-layout-title"><?php print render($element['title']); ?></h1>
      <?php print render($element['body']); ?>
      <?php $view = views_get_view('recent_petiton_signers'); $view->set_display('signers_block'); ?>
      <h2><?php print $view->get_title(); ?></h2>
      <?php print $view->preview('signers_block'); ?>
    </div>
    <div id="right" class="form-col">
      <?php print render($element['webform']); ?>
      <div id="petition-quick-sign">
        <?php print render($element['springboard_petition_quicksign']); ?>
      </div>
    </div>
  </div>
</div>
