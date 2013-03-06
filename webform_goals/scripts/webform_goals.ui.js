(function ($) {

  Drupal.behaviors.webform_goals = {
    attach: function (context, settings) {
      $('.colorpicker').jPicker({
        window:
        {
          expandable: true
        }
      });
     
    }
  };

})(jQuery);
