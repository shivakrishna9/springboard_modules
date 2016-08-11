(function ($) {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {
      // hijack click event for share links, apply share url provided by share tracker
      $('.social-share-link').each(function() {
         if (!$(this).hasClass('social-processed')) {
           // Add a class to ensure the click event handler is only attached
           // a single time regardless of how many times attach is triggered.
           $(this).addClass('social-processed');
           $(this).click(function() {
               var $elem = $(this);
               // account for subdirectory install
               var $url = Drupal.settings.basePath;
               var service;
               if ($(this).hasClass('facebook')) {
                 $url += 'sb_social/share_event/facebook/';
                 service =  'facebook';
               }
               if ($(this).hasClass('twitter')) {
                 $url += 'sb_social/share_event/twitter/';
                 service =  'twitter';
               }
               if ($(this).hasClass('email')) {
                 $url += 'sb_social/share_event/email/';
                 service =  'email';
               }
               $url = $url + Drupal.settings.sb_social.id + '/';
               $url = $url + Drupal.settings.sb_social.id_type + '/';
               $url = $url + Drupal.settings.sb_social.market_source + '/';

               // conditionally add webform submission id if present
               if (typeof Drupal.settings.sb_social.submission_id !== 'undefined') {
                   $url = $url + '?sid=' + Drupal.settings.sb_social.submission_id;
               }

               var link = window.location.href;
             // Register share event and provide share URL to AddThis.
               jQuery.ajax({
                   url: $url,
                   success:function(response) {
                     // Shorten module can be configured to use 'www' instead of 'http'
                     // which breaks twitter.
                     if (link.indexOf("http") === -1) {
                       link = 'https://'. link;
                     }
                     link = encodeURIComponent(response);
                   },
                   async: false
               });
               var settings = Drupal.settings.sb_social;
               switch (service) {
                 case 'twitter':
                   socialPopup('https://twitter.com/intent/tweet?text=' + settings.twitter_message + '&url=' + link);
                   return false;
                 case  'facebook':
                   socialPopup('https://facebook.com/sharer.php?u=' + link);
                   return false;
                 case 'email':
                   $elem.attr('href', ' mailto:?subject=' + settings.email_subject + '&body=' + settings.email_message + "\n\n" + link);
                   break;
               }
           });
         }
      });
    }
  };

  var socialPopup = function(path) {
    if (!path) {
      console.log("path must not be empty");
    }
    var options = $.extend({
      windowName: 'sbSocialPopup',
      windowOptions: 'width=550,height=420',
    }, options);

    var sbSocialPopup = window.open(path, options.windowName, options.windowOptions);
    sbSocialPopup.focus();
  };

})(jQuery);

