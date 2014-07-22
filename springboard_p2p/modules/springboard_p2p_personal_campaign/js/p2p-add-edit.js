/**
 * @file
 * Misc JQuery P2P scripts in this file.
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.p2pAddEdit = {
    attach: function (context, settings) {
// End drupal calls.

      $('.image-widget.form-managed-file').each(function() {
      $(this).parent('.control-group').addClass('image-widget-wrapper');
      });

      // Remove the panel seperator.
      $(".panel-separator").remove();

      // add the missing required date label.
      $('.date-padding label').once(function() {
      $(this).append('<span class="form-required" title="This field is required.">*</span>');
      });

      // Add uniform to selects
      $("select").uniform();

      // Add uniform to file upload.
      $('input[type=file]').uniform();

      // end jquery.
    }};
})
  (jQuery);