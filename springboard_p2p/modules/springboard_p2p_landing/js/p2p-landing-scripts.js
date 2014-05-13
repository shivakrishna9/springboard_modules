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
      if($().flexslider) {
        $(window).load(function () {
          $('.flexslider').once(function () {
            $(this).flexslider({prevText: "", nextText: "", controlNav: false,  pauseOnHover: true, slideshowSpeed: 6000});
          });
        });
      }

      // end jquery.
    }};
})
(jQuery);
