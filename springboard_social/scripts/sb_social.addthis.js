var addthis_config = {
  pubid: ''
};

var addthis_share = {
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

      // hijack click event for share links, apply share url provided by share tracker
      $('.social-share-link').click(function() {
          $elem = $(this);
          if ($(this).hasClass('facebook')) {
            $url = '/sb_social/share_event/facebook/';
          }
          if ($(this).hasClass('twitter')) {
             $url = '/sb_social/share_event/twitter/';
          }
          if ($(this).hasClass('email')) {
              $url = '/sb_social/share_event/email/';
          }
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

