(function($) {
  Drupal.behaviors.springboardForms = {
    attach: function (context, settings) {

      $('.webform-client-form #edit-submit').once(function() {
        $('.webform-client-form #edit-submit').click(function() {
          // Disable the button. This prevents double/triple mouse clicking.
          $(this).hide();
          $('.fundraiser_submit_message').hide();
          $(this).after('<div class="webform-user-wrapper">' +
          '<p class="webform-user-thank-you">Thank you, your submission is processing.</p>' +
          '<div class="webform-user-spinner"></div>' +
          '</div>');
        });
      });

    } // attach.function
  } // drupal.behaviors
})(jQuery);