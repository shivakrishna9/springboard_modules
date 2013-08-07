<div class = "addthis_toolbox addthis_default_style addthis_16x16_style">
<?php
if (in_array('facebook', $services)) : ?>
<a class="addthis_button_facebook social-share-link facebook" addthis:url="<?php print $services['facebook_url']; ?>"></a>
<?php
endif;
 ?>
<?php if (in_array('twitter', $services)) : ?>
<a class="addthis_button_twitter social-share-link twitter" addthis:url="<?php print $services['twitter_url']; ?>"></a>
<?php
endif;
?>
<?php if (in_array('email', $services)) : ?>
<a class="addthis_button_email social-share-link email" addthis:url="<?php print $services['email_url']; ?>"</a>
<?php
endif;
?>
</div>
