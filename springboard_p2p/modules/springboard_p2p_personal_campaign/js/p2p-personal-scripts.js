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

      // Remove the panel seperator.
      $(".panel-separator").remove();

      // Get rid of label colons.
      $('.field-label').each(function() {
        var text = $(this).html();
        $(this).html( text.replace(':', '') );
      });

      // personal campaign slideshow.
      $('#slide-show').bjqs({
        height : 400,
        animspeed : 511000,
        responsive  : true,
        nexttext : '',
        prevtext : '',
        usecaptions : true,
        showmarkers : false
      });

      // end jquery.
    }};
})
(jQuery);
