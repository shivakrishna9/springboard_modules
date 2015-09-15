/**
 * @file 
 * This jQuery disables or enables the "Number of days before entries are cleared from the expired table"
 * form field when the "Pre-populated tokens never expire" form field is toggled.
 */

(function ($) {
  $(document).ready(function () {
    $('#edit-secure-prepopulate-toggle-expiration').click(function () {
      var expireDaysField = $('#edit-secure-prepopulate-expired-date-limit');
      if ($(this).attr('checked')) {
        expireDaysField.attr('disabled', 'disabled');
        expireDaysField.parent().addClass('form-disabled');
      }
      else {
        expireDaysField.removeAttr('disabled');
        expireDaysField.parent().removeClass('form-disabled');
      }
    });
  });
})(jQuery);
