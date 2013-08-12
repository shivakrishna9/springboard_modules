(function ($) {
  $(document).ready(function() {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {
      // set up custom configuration for twitter message and share URL
      window.addthis_share = {
        email_vars: {
          subject: Drupal.settings.sb_social.email_subject,
          message: Drupal.settings.sb_social.email_message
        }
      };
    }
  };
  });
})(jQuery);

