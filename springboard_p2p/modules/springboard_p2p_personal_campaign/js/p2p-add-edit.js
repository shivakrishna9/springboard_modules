/**
 * @file
 * Misc JQuery P2P scripts in this file.
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.p2pAddEdit = {
    attach: function (context, settings) {
// End drupal calls.

      // Add a file icon to the upload button.
      $('input[type="file"]').once(function() {
        $(this).after('<i class="fa fa-file-text-o"></i>');
      });

      $('.image-widget.form-managed-file').each(function() {
      $(this).parent('.control-group').addClass('image-widget-wrapper');
      });

      // end jquery.
    }};
})
  (jQuery);