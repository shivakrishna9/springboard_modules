<a href="sustainer/lightbox" rel="lightmodal" id='upsell-click' style="display: none;">Show Content</a>
<div id='sustainer-upsell-lightbox' style="display: none;">
  <div style="padding: 20px;">
    <h3>Become a</h3>
    <h2>MONTHLY DONOR</h2>
    <?php print $message; ?>
    <?php print $donor_form; ?>
    <input type='button' name='submit' value='Yes, Sign Me Up!' id='upsell-signup-btn'>
    <input type='button' name='submit' value='No Thanks' id='upsell-close-btn'>
  </div>
</div>
<script>
$(function() {
  $('#upsell-click').click();
  $('#upsell-signup-btn').click(function() {
    $('#sustainer-upsell-donation-form').submit();
  });
  $('#upsell-close-btn').click(function() {
    <?php
      if(variable_get('sustainer_upsell_rejection_enabled',0) == 1):
        ?>
          $.cookie("sustainer_upsell_rejection", 1, { expires : <?php print variable_get('sustainer_upsell_rejection_lifetime',0) ?> });
        <?
      endif;
    ?>
    tb_remove();
  });
});
</script>