/**
 * @file
 * Misc js for 2 col form with buttons.
 */

(function ($, Drupal) {
  Drupal.behaviors.twocolbtnMisc = {
    attach: function (context) {

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

      $('.webform-client-form input[type="radio"]').each(function () {
        $(this).next("label").addClass($(this).attr("type") + '-' + $(this).attr("value"));
        $(this).addClass('radio-button-input');
        $(this).next("label").addClass('radio-label');
      });

      // Test for which radio checked onload.
      var $radioButtonsLoad = $('.webform-client-form input[type="radio"]');
      $radioButtonsLoad.each(function () {
        $(this).next("label").toggleClass('radio-checked', this.checked);
      });

      // Test for radio onclick.
      var $radioButtons = $('.webform-client-form input[type="radio"]');
      $radioButtons.click(function () {
        $radioButtons.each(function () {
          $(this).next('label').toggleClass('radio-checked', this.checked);
        });
      });

      // Show / hide other amount input box when other amount is clicked / touched.
      $('.webform-client-form .option.radio-other.radio-label').click(function () {
        $('#webform-component-donation--other-amount').css('display', 'inline-block');
      });

      // Now hide it if something else in the group is clicked.
      $('.webform-client-form #webform-component-donation--amount .option.radio-label:not(".radio-other")').click(function () {
          $('#webform-component-donation--other-amount').css('display', 'none');
      });


    }
  };


})(jQuery, Drupal);