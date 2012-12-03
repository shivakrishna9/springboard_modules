<?php
/**
 * @file
 * List of tokens available for the confirmation token selector WYSIWYG tool.
 *
 * Available variables:
 * - $tokens: All available confirmation page tokens.
 */
?>

<style type="text/css">
  #fundraiser-confirmation-tokens {
    background-color: #FFF;
    margin: 20px;
    padding: 10px;
  }

  #fundraiser-confirmation-tokens select {
    width: 350px;
  }

  #fundraiser-confirmation-tokens h2 {
    margin-top: 10px;
  }

  #fundraiser-confirmation-tokens em {
    font-size: .8em;
  }
</style>

<script type="text/javascript">
  $(document).ready(function(){
    $('#webform-tokens').change(function(e) {
      window.returnValue = $(this).val();
      window.close();
    });

    $('#order-tokens').change(function(e) {
      window.returnValue = $(this).val();
      window.close();
    });
  });
</script>

<div id="fundraiser-confirmation-tokens">
  <h1>Insert tokens</h1>
  <p>Select the token you wish to insert into the confirmation message.</p>
  <h2>Webform component tokens</h2>
  <select id="webform-tokens">
    <?php foreach($tokens['webform'] as $k => $v) { ?>
      <option value="<?php print $k;?>"><?php print $v;?></option>
    <?php } ?>
  </select>

  <h2>Order tokens</h2>
  <select id="order-tokens">
    <?php foreach($tokens['order'] as $k => $v) { ?>
      <option value="<?php print $k;?>"><?php print $v;?></option>
    <?php } ?>
  </select>
  <p><em>After you make your selection this dialog window will close and the token will be inserted at the cursor's current position.</em></p>
</div>

