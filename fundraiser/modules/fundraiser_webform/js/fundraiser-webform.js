/**
 * JavaScript behaviors for the front-end display of fundraiser webforms.
 */

(function ($) {

Drupal.behaviors.fundraiserWebform = Drupal.behaviors.fundraiserWebform || {};

Drupal.behaviors.fundraiserWebform.attach = function(context) {
  // Calendar datepicker behavior.
  Drupal.fundraiserWebform.fundraiserPaymentSelect(context);
};

Drupal.fundraiserWebform = Drupal.fundraiserWebform || {};

/**
 * Add a listener to our payment type field
 */
Drupal.fundraiserWebform.fundraiserPaymentSelect = function(context) {
  if (Drupal.settings.fundraiserWebform.components && Drupal.settings.fundraiserWebform.components.payment_method) {
    var selectElement = Drupal.settings.fundraiserWebform.components.payment_method.id,
      fieldsElement = Drupal.settings.fundraiserWebform.components.payment_fields.id;

    $('#' + fieldsElement + ' fieldset.fundraiser-payment-method:not(:first)').hide();
    $.each(Drupal.settings.fundraiserWebform.active_methods, function(key, field) {
      $('#' + selectElement + '-' + key).change(function() {
        $('#' + fieldsElement + ' fieldset.fundraiser-payment-method').hide();
        $('#' + fieldsElement + ' fieldset#fundraiser-payment-method-' + field).show();
      });
    });
  }
}

})(jQuery);