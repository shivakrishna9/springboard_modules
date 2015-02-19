<?php 
  /** @TODO documentation here **/

?>

<?php foreach ($views as $view): ?>
  <?php if (isset($view['css'])):
    drupal_add_css($view['css']); ?>
  <?php endif; ?>
  <div class="types-wrapper p2p-views">
    <?php if (isset($view['header'])): ?>
      <h2 class="types"><?php print $view['header'];?></h2>
    <?php endif; ?>
    <div class="buttons-wrapper">
    <?php if (isset($view['manage_link'])): ?>
      <div class="sba-button"><?php print l($view['manage_link']['title'], $view['manage_link']['href'], array('attributes' => array('class' => array('button', 'more-button')))); ?></div>
    <?php endif; ?>
    <?php if (isset($view['create_link'])): ?>
      <div class="sba-button"><?php print l($view['create_link']['title'], $view['create_link']['href'], array('attributes' => array('class' => array('button', 'sba-add-button')))); ?></div>
    <?php endif; ?>
   </div>
    <?php if (isset($view['content'])): ?>
      <?php print $view['content']; ?>
      <?php if (isset($view['viewmore_link'])): print l($view['viewmore_link']['title'], $view['viewmore_link']['href'], array('attributes' => array('class' => array('button', 'more-button')))); endif; ?>
    <?php endif; ?>
  </div>
<?php endforeach; ?>