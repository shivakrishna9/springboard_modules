(function ($) {
  $(document).ready(function() {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {
      // set up custom configuration for twitter message and share URL
      window.addthis_share = {
        passthrough: {
          twitter: {
            text: Drupal.settings.sb_social.twitter_message
          }
        },
      };
    }
  };
  });
})(jQuery);

