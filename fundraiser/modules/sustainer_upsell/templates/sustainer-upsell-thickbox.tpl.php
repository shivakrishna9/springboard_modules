<div id='sustainer-upsell-lightbox'>
  <div style="padding: 20px;">
    <h3>Become a</h3>
    <h2>MONTHLY DONOR</h2>
    <?php print $message; ?>
    <?php print $match; ?>
    <?php print $content; ?>
  </div>
</div>
<script>
$(function() {
  $('#edit-close').click(function() {
    <?php
      if(variable_get('sustainer_upsell_rejection_enabled',0) == 1):
        ?>
          $.cookie("sustainer_upsell_rejection", 1, { expires : <?php print variable_get('sustainer_upsell_rejection_lifetime',0) ?> });
        <?
      endif;
    ?>
    window.parent.tb_remove();
    return false;
  });
});
</script>