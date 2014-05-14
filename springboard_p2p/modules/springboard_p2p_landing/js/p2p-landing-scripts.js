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
        $(window).load(function () {
          $('.flexslider').once(function () {
            $(this).flexslider({prevText: "", nextText: "", controlNav: false, pauseOnHover: true, slideshowSpeed: 6000});
          });
        });
      }

      // Responsive equal heights for boxes in a grid.
      equalheight = function (container) {
        var currentTallest = 0,
          currentRowStart = 0,
          rowDivs = new Array(),
          $el,
          topPosition = 0;
        $(container).each(function () {

          $el = $(this);
          $($el).height('auto')
          topPostion = $el.position().top;

          if (currentRowStart != topPostion) {
            for (currentDiv = 0; currentDiv < rowDivs.length; currentDiv++) {
              rowDivs[currentDiv].height(currentTallest);
            }
            rowDivs.length = 0; // empty the array
            currentRowStart = topPostion;
            currentTallest = $el.height();
            rowDivs.push($el);
          } else {
            rowDivs.push($el);
            currentTallest = (currentTallest < $el.height()) ? ($el.height()) : (currentTallest);
          }
          for (currentDiv = 0; currentDiv < rowDivs.length; currentDiv++) {
            rowDivs[currentDiv].height(currentTallest);
          }
        });
      }

      // Now set the equal height on the thumb grid.
      $(window).load(function () {
        equalheight('.view-content .campaign-landing-grid li');
      });

      $(window).resize(function () {
        equalheight('.view-content .campaign-landing-grid li');
      });


      // end jquery.
    }};
})
(jQuery);
