var addthis_config = {
  pubid: ''
};

var addthis_share = {
  'url' : 'http://www.google.com'
};


(function ($) {
  //$(document).ready(function() {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {
      // AddThis requires some global configuration objects be populated
      // set up the AddThis account ID from admin settings
      window.addthis_config = {
        pubid: Drupal.settings.sb_social.pubid
      };
      // event listener logs share event
      if (typeof window.addthis !== "undefined") {
        window.addthis.addEventListener('addthis.menu.share', shareResponse);
      }

      // Of course Facebook requires special handling, how could it be otherwise?
      $('.social-share-link.facebook').click(function() {
          $elem = $(this);
          $url = '/sb_social/share_event/facebook/';
          $url = $url + Drupal.settings.sb_social.id + '/';
          $url = $url + Drupal.settings.sb_social.id_type + '/';
          $url = $url + Drupal.settings.sb_social.market_source + '/';

          // Register share event and provide share URL to AddThis.
          jQuery.ajax({
              url: $url,
              success:function(response) {
                // Overwrite default url with share url (including share id and market source values)
                $elem.attr('addthis:url', response);
                $elem.context.conf.url = response;
                $elem.context.share.url = response;
              },
              async: false
          });
      });
    }
  };
  //});
})(jQuery);

// Register share event and provide share URL to AddThis.
function shareResponse(event) {
  // Facebook share buttons have to be handled earlier in execution to ensure
  // AddThis picks up the correct URL.
  if (event.data.service == 'facebook') {
    return;
  }

  $url = '/sb_social/share_event/';
  $url = $url + event.data.service + '/';
  $url = $url + Drupal.settings.sb_social.id + '/';
  $url = $url + Drupal.settings.sb_social.id_type + '/';
  $url = $url + Drupal.settings.sb_social.market_source + '/';

  jQuery.ajax({
       url: $url,
       success:function(response){

         // overwrite existing placeholder url with one
         // that contains the share event id
         // email tracks on this pair
         event.data.element.conf.url = response;
         event.data.element.share.url = response;
         // twitter appears to track on these two
         event.data.element.share.trackurl = response;
         event.data.url = response;
       },
      async: false
  });
}
