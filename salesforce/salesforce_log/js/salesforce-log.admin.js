(function ($) {
  Drupal.behaviors.salesforce_log_expander = {
    attach: function (context, settings) {
      // override default options (also overrides global overrides)
      $('td.expander').expander({
        slicePoint:       80,
        preserveWords:    false,
        expandPrefix:     ' ',
        expandText:       '[...]',
        userCollapseText: '[^]'
      });
    }
  }
})(jQuery);
