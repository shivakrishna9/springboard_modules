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
FB.ui(
{
method: 'feed',
name: '<?php print $title ?>',
link: '<?php print $link_url ?>',
picture: '<?php print $logo ?>',
caption: '',
description: 'Your tax deductible donation to the International Rescue Committee (IRC) helps us provide lifesaving care and life-changing assistance to refugees fleeing war and disaster.',
message: 'I just donated to help the IRC rescue and rebuild lives around the world. Will you join me?'
});
});
});
</script>

<p><div id="share_button">Share this</div></p>
