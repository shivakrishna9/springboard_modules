/**
 * @file
 * Misc JQuery scripts in this file
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.misc_p2pPersonal = {
    attach: function (context, settings) {
      // End drupal calls.

      // Fitvids for responsive video.
      $(document).ready(function() {
        $(".embedded-video").fitVids();
      });

      $(".panel-separator").remove();

      // end jquery.
    }};
})
(jQuery);
