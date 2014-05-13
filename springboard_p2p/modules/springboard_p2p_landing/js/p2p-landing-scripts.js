/**
 * @file
 * Misc JQuery scripts in this file
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.misc_p2pLanding = {
    attach: function (context, settings) {
      // End drupal calls.

      // Instantiate Flexslider.
      if ($().flexslider) {
        $('.flexslider').once(function () {
          $(this).flexslider({
            animation: "slide",
            controlNav: false,
            pauseOnHover: true,
            slideshowSpeed: 336000,
            animationSpeed: 200,
            prevText: "",           //String: Set the text for the "previous" directionNav item
            nextText: ""              //String: Set the text for the "next" directionNav item
          });
        });
      }

      // end jquery.
    }};
})
(jQuery);
