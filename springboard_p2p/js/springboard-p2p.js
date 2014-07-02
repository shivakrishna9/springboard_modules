/**
 * @file
 * Misc JQuery P2P scripts in this file.
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.p2pJS = {
    attach: function (context, settings) {
// End drupal calls.

      // Add a class to the parent for better theming.
      $('#edit-mail').parent('.control-group').addClass('email-wrapper');
      $('#edit-mail').parent('.form-item').addClass('email-wrapper');

    // Get rid of inline styles.
      $('p').removeAttr("style");

      // Add uniform to selects
      $("select").uniform();

      // Add uniform to file upload.
      $('input[type=file]').uniform();

      // end jquery.
    }};
})
(jQuery);