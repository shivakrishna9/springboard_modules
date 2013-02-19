<script type="text/javascript">
(function ($) {
  $url = '<?php print $file_url; ?>';

  $(document).ready(function(){

    $.get($url, function(data) {
      $('#goal-<?php print $gid ?>').html(data);
    });
  });
})(jQuery);
</script>
<h2>Goal:</h2>
<div id="goal-<?php print $gid; ?>">
</div>
