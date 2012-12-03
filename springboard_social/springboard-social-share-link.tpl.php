<script src="http://connect.facebook.net/en_US/all.js"></script>
<script>
FB.init({
 appId  : '<?php print $app_id ?>',
 status : false, // check login status
 cookie : true, // enable cookies to allow the server to access the session
 xfbml  : true// parse XFBML
 });
 FB.Canvas.setAutoResize(7);
 </script>

<script type="text/javascript">
$(document).ready(function(){
  $('#share_button').click(function(e){
    e.preventDefault();
    FB.ui({
        method: 'feed',
        name: '<?php print $title ?>',
        link: '<?php print $link_url ?>',
        picture: '<?php print $share_logo ?>',
        caption: '',
        description: '<?php print $description ?>',
        message: '<?php print $message ?>'
    });
  });
});
</script>

<p><div id="share_button">&nbsp;</div></p>
