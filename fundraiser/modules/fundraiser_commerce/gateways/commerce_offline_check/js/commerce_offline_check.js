(function($) {
  Drupal.behaviors.offlineCheck = {
    attach: function(context) {
      $('textarea[name$="[offline_check][message]"]').siblings('label').children('span.form-required').remove();
      $('input[name$="[offline_check][check_number]"]').siblings('label').children('span.form-required').remove();
      $(document).bind('state:required', function(e) {
        if (e.target.id.indexOf('offline-check') != -1) {
          $('textarea[name$="[offline_check][message]"]').siblings('label').children('span.form-required').remove();
          $('input[name$="[offline_check][check_number]"]').siblings('label').children('span.form-required').remove();
        }
      });
    }
  };
})(jQuery);