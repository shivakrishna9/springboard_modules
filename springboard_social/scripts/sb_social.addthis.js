var addthis_config = {
  pubid: ''
};

var addthis_share = addthis_share || {}
addthis_share = {
  passthrough: {
    twitter: {
      text: 'test'
    }
  }
};

(function ($) {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {

      // AddThis requires some global configuration objects be populated
      // set up the AddThis account ID from admin settings
      window.addthis_config = {
        pubid: Drupal.settings.sb_social.pubid
      };
      // set up custom configuration for twitter message and share URL
      window.addthis_share = {
        passthrough: {
          twitter: {
            text: Drupal.settings.sb_social.twitter_message,
          }
        }
      };
      // event listener logs share event
      window.addthis.addEventListener('addthis.menu.share', shareResponse);
    }
  };

})(jQuery);

function shareResponse(data) {
  console.log(data);
  console.log(Drupal.settings);
  $url = '/sb_social/share_event/' + data.data.service + '/' + Drupal.settings.sb_social.sid;

  // todo: fire ajax event that logs share event.
  jQuery.ajax({
       url: $url,
       type: "GET",
       success:function(response){
         console.log(response);
       },
  });
}
