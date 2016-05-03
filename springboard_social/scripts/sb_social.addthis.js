var addthis_config = {
  pubid: ''
};

var addthis_share = {
};

var addthis_trackingProtectionDetected = 1;

(function ($) {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {
      $(document).ready(function() {
        // Display a message if Firefox's Tracking Protection is preventing addthis icons from appearing or being clickable:
        $('.webform-confirmation').before('<iframe src="//googlesyndication.com" onload="addthis_trackingProtectionDetected = 0;" ' +
          'style="width: 1px !important; height: 1px !important; position: absolute !important; left: -10000px !important; ' +
          'top: -1000px !important;"></iframe>');
        function detectTrackingProtection() {
          if (addthis_trackingProtectionDetected == '1') {
            $('.webform-confirmation').before('<div class="social-share-popup-blocker-msg"><strong>' +
              'Please disable your browser\'s Tracking Protection to share!' + '</strong></div><br />');
          }
        }
        // Display a message if popup blocking is detected:
        function detectPopupBlocker() {
          if ($('.addthis_toolbox').length > ) {
            var popupBlockTest = window.open('about:blank', '', 'directories=no,height=100,width=100,menubar=no,' +
            'resizable=no,scrollbars=no,status=no,titlebar=no,top=0,location=no');
            if (popupBlockTest != null) {
              popupBlockTest.document.write('-.-.-');
            }
            if (popupBlockTest == null || !popupBlockTest || popupBlockTest.closed || typeof popupBlockTest == 'undefined' ||
              typeof popupBlockTest.closed == 'undefined' || typeof $(popupBlockTest.document.body).text() == 'undefined' ||
              $(popupBlockTest.document.body).text() != '-.-.-') {
              if ($('.social-share-popup-blocker-msg').length == 0) {
                $('.addthis_toolbox').before('<div class="social-share-popup-blocker-msg"><strong>' +
                  'Please disable your pop-up blocker to share!' + '</strong></div><br />');
              }
            }
            else {
              popupBlockTest.close();
            }
          }
        }
        setTimeout(function() {
          detectPopupBlocker();
          detectTrackingProtection();
        }, 1000);
      });
      
      // AddThis requires some global configuration objects be populated
      // set up the AddThis account ID from admin settings
      if (typeof window.addthis_config !== 'undefined') {
        settings = {
          pubid: Drupal.settings.sb_social.pubid
        };
        jQuery.extend(window.addthis_config, settings);
      }
      else {
        window.addthis_config = {
          pubid: Drupal.settings.sb_social.pubid
        }
      }
      // hijack click event for share links, apply share url provided by share tracker
      $('.social-share-link').each(function() {
        if (!$(this).hasClass('social-processed')) {
          // Add a class to ensure the click event handler is only attached
          // a single time regardless of how many times attach is triggered.
          $(this).addClass('social-processed');
          $(this).click(function(e) {
            // Prevent multi-clicking via timestamp:
            var lastClicklock = $(this).data('clicklock'); 
            $(this).data('clicklock', (new Date).getTime());
            if (typeof lastClicklock != 'undefined' && (new Date).getTime() - lastClicklock < 1000) {
              return;
            }

            $elem = $(this);
            // account for subdirectory install
            $url = Drupal.settings.basePath;
            if ($(this).hasClass('facebook')) {
              $url += 'sb_social/share_event/facebook/';
            }
            if ($(this).hasClass('twitter')) {
              $url += 'sb_social/share_event/twitter/';
            }
            if ($(this).hasClass('email')) {
              $url += 'sb_social/share_event/email/';
            }
            $url = $url + Drupal.settings.sb_social.id + '/';
            $url = $url + Drupal.settings.sb_social.id_type + '/';
            $url = $url + Drupal.settings.sb_social.market_source + '/';
 
            // conditionally add webform submission id if present
            if (typeof Drupal.settings.sb_social.submission_id !== 'undefined') {
              $url = $url + '?sid=' + Drupal.settings.sb_social.submission_id;
            }

            // Register share event and provide share URL to AddThis.
            jQuery.ajax({
              url: $url,
              success:function(response) {
                // Overwrite default url with share url (including share id and market source values)
                $elem.attr('addthis:url', response);
                if ($elem.hasClass('facebook')) {
                  $elem.attr('fb:like:href', response);
                }
                $elem.context.conf.url = response;
                $elem.context.share.url = response;
              },
              async: true 
            });
          });
        }
      });
    }
  };
})(jQuery);
