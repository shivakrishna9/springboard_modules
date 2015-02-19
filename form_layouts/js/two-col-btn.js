/**
 * @file
 * Misc js for 2 col form with buttons.
 */

(function ($, Drupal) {
  Drupal.behaviors.twocolbtnMisc = {
    attach: function (context) {

      // Define vars.
      var radioButtons = $('.webform-client-form input[type="radio"]');
      var otherInput = $('#webform-component-donation--other-amount input');

      // Test for checkboxes checked / not checked.
      $('input[type=checkbox]').click(function () {
        if ($(this).is(':checked')) {
          $(this).next('label').toggleClass('checked', this.checked).removeClass('not-checked');
        }
        else {
          $(this).next('label').addClass('not-checked').removeClass('checked');
        }
      });

      // Add meaningful classes to webform radio buttons & labels.
      $('input[type=checkbox]').addClass('checkbox-input');
      $('input[type=checkbox]').next('label').addClass('checkbox-label');

      $(radioButtons).each(function () {
        $(this).next("label").addClass($(this).attr("type") + '-' + $(this).attr("value"));
        $(this).addClass('radio-button-input');
        $(this).next("label").addClass('radio-label');
      });

      // Test for which radio checked onload.
      radioButtons.each(function () {
        $(this).next("label").toggleClass('radio-checked', this.checked);
      });

      // Test for radio onclick.
      radioButtons.click(function () {
        radioButtons.each(function () {
          $(this).next('label').toggleClass('radio-checked', this.checked);
        });
      });

      // Add class to top level amount button wrappers.
      $('#edit-submitted-donation-amount .control-group').each(function() {
        var radio = $(this).find('input[type="radio"]');
        // Add special classes to "other" option.
        if (radio.val() == "other") {
          $(this).addClass('other-option');
        }
        else {
          $(this).addClass('reg-option');
        }
      });

      // Add / remove classes depending if other gets focus or not.
      $(otherInput).blur(function() {
        $('.reg-option').addClass('not-selected').removeClass('selected');
      });

      $(otherInput).focus(function() {
        $('.reg-option').addClass('selected').removeClass('not-selected');
      });

      // Move the other amount field next to the main buttons for better theming.
      $('#webform-component-donation--other-amount').insertAfter('#edit-submitted-donation-amount');

    }
  };

})(jQuery, Drupal);
