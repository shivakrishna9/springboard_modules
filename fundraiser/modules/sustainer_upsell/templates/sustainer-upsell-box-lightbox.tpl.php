<div id="sustainer-upsell-lightbox" class="<?php print $classes; ?>">
  <div class="inner">
    <h3>Become a</h3>
    <h2>MONTHLY DONOR</h2>
    <div class="inner-content">
      <?php print $message; ?>    
      <?php print $match; ?>  
    </div>
    <?php print $content; ?>
    <div class="disclaimer">
      <p>By signing up to be a monthly donor, your credit card will be charged the monthly amount you specify beginning next month.</p>
    </div>
  </div>
</div>
<script>
$(function() {
  $('#edit-close').click(function() {
    <?php
      if(variable_get('sustainer_upsell_rejection_enabled',0) == 1):
        ?>
          $.cookie("sustainer_upsell_rejection", 1, { path : "/", expires : <?php print variable_get('sustainer_upsell_rejection_lifetime',0) ?> });
        <?php
      endif;
    ?>
    window.parent.Lightbox.end();
    return false;
  });
});
</script>
