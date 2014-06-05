/**
 * @file
 * Misc JQuery scripts in this file
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.p2pJS = {
    attach: function (context, settings) {
// End drupal calls.

      // Add a class to the parent for better theming.
      $('#edit-mail').parent('.control-group').addClass('email-wrapper');

    // Get rid of inline styles.
   $('.panel-display p').attr('style', '');

/* P2P donation form */

      // Add meaningful classes to webform radio buttons & labels.
      $('input[type=checkbox]').addClass('checkbox-input');
      $('input[type=checkbox]').next('label').addClass('checkbox-label');

      $('.webform-client-form input[type="radio"]').each(function() {
        $(this).next("label").addClass($(this).attr("type") + '-' + $(this).attr("value"));
        $(this).addClass('radio-button-input');
        $(this).next("label").addClass('radio-label');
      });

      // checkboxes checked / not checked.
      $('input[type=checkbox]').click(function () {
        if ($(this).is(':checked')) {
          $(this).next('label').toggleClass('checked', this.checked).removeClass('not-checked');
        }
        else {
          $(this).next('label').addClass('not-checked').removeClass('checked');
        }
      });

      // Generic checked / unchecked for radio.
      var radioChecked = $('.single-radio input').is(':checked');
      $('.single-radio input').click(function() {
        radioChecked = !radioChecked;
        $(this).attr('checked', radioChecked);
      });

      // Test for radio onclick.
      var $radioButtons = $('.webform-client-form input[type="radio"]');
      $radioButtons.click(function() {
        $radioButtons.each(function() {
          $(this).next('label').toggleClass('radio-checked', this.checked);
        });
      });


      // end jquery.
    }};
})
(jQuery);