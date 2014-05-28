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

      // end jquery.
    }};
})
(jQuery);