(function($) {
  Drupal.behaviors.offlineCheck = {
    attach: function(context) {
      $('textarea[name$="[offline_check][message]"]').siblings('label').children('span.form-required').remove();
      $('input[name$="[offline_check][check_number]"]').siblings('label').children('span.form-required').remove();

    }
  };
})(jQuery);


