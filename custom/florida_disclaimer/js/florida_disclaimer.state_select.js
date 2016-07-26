(function ($) {

  Drupal.behaviors.floridaDisclaimer = {
    attach: function (context, settings) {
      settings = settings || Drupal.settings;
      $('#edit-submitted-billing-information-state').change(function() {
        if ($('#edit-submitted-billing-information-state').length && $('#edit-submitted-billing-information-state option:selected').text() == 'Florida') {
          $('p.donation_disclaimer').text(settings.florida_disclaimer.disclaimer_text);
        }
        else {
          $('p.donation_disclaimer').text('');
        }
      });


    }
  };

})(jQuery);
