(function ($) {
  Drupal.behaviors.sb_social_email = {
    attach: function (context, settings) {
      // set up custom configuration for email subject and message
      //window.addthis_share = {
      //  email_vars: {
      //    subject: Drupal.settings.sb_social.email_subject,
      //    message: Drupal.settings.sb_social.email_message
      //  }
      //};
      // sets the contents of the [note] email template variable in the AddThis email template.
      window.addthis_config = {
        ui_email_note: Drupal.settings.sb_social.email_message
      }
    }
  };
})(jQuery);

