<div class="mytube" style="width: <?php print $width; ?>px;">
  <div class="mytubetrigger">
    <img width="<?php print $width; ?>" height="<?php print $height; ?>" class="mytubethumb" alt="mytubethumb" src="<?php print str_replace('?q=', '', $thumb); ?>" />
    <img src="<?php print $play_icon; ?>" class="mytubeplay" alt="play" style="top: <?php print (int)$height / 2 - 30; ?>px; left: <?php print (int)$width / 2 - 30; ?>px;" />
    <div class="mytubeembedcode"><?php print $escaped_embed; ?></div>
  </div><!--mytubetrigger-->
  <div class="mytubetext">
    <?php print l(t('Privacy info.'), $privacy_url) . filter_xss_admin(" $privacy_text"); ?>
  </div>
</div>
