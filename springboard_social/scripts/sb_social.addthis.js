
(function ($) {

  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {

      $('.social-share-link').each(function () {
          var link = window.location.href;
          // Add a class to ensure the click event handler is only attached
          // a single time regardless of how many times attach is triggered.
          $(this).click(function (e) {
            var $elem = $(this);
            // account for subdirectory install
            var $url = Drupal.settings.basePath;
            var service;
            if ($(this).hasClass('facebook')) {
              $url += 'sb_social/share_event/facebook/';
              service = 'facebook';
            }
            if ($(this).hasClass('twitter')) {
              $url += 'sb_social/share_event/twitter/';
              service = 'twitter';
            }
            if ($(this).hasClass('email')) {
              $url += 'sb_social/share_event/email/';
              service = 'email';
            }
            $url = $url + Drupal.settings.sb_social.id + '/';
            $url = $url + Drupal.settings.sb_social.id_type + '/';
            $url = $url + Drupal.settings.sb_social.market_source + '/';

            // conditionally add webform submission id if present
            if (typeof Drupal.settings.sb_social.submission_id !== 'undefined') {
              $url = $url + '?sid=' + Drupal.settings.sb_social.submission_id;
            }

            if (!$(this).hasClass('social-processed')) {
              // Register share event and provide share URL to AddThis.
              jQuery.ajax({
                url: $url,
                success: function (response) {
                  if (response.indexOf("http") === -1) {
                    response = 'https://' + response;
                  }
                  link = encodeURIComponent(response);
                },
                async: false
              });
            }

            $(this).addClass('social-processed');

            var settings = Drupal.settings.sb_social;
            switch (service) {
              case 'twitter':
                t = socialPopup(this, 'https://twitter.com/intent/tweet?text=' + settings.twitter_message + '&url=' + link);
                e.returnValue = false;
                e.preventDefault();
                break;
              case  'facebook':
                socialPopup(this, 'https://facebook.com/sharer.php?u=' + link);
                e.returnValue = false;
                e.preventDefault();
                break;
              case 'email':
                $elem.attr('href', ' mailto:?subject=' + settings.email_subject + '&body=' + settings.email_message + "\n\n" + link);
                break;
            }

          });

      });
    }
  };

  var socialPopup = function(item, path) {

    if (!path) {
      console.log("path must not be empty");
    }
    var options = $.extend({
      windowName: 'sbSocial',
      windowOptions: 'width=550,height=420',
      callback: function(item){
        $(item).removeClass('social-processed');
      }
    }, options);

    if (window.sbSocialPopup) {
      window.sbSocialPopup.close();
    }
    window.sbSocialPopup = window.open(path, options.windowName, options.windowOptions);
    window.sbSocialPopup.focus();

     //If the window is closed, re-enable the social share click event
     //that was disabled previously to prevent double clicks.
    var x = 0;
    var oauthInterval = window.setInterval(function(){
      x++;
      if (window.sbSocialPopup.closed || x === 60) {
        window.clearInterval(oauthInterval);
        options.callback(item);
      }
    }, 1000, x);
  };

})(jQuery);

