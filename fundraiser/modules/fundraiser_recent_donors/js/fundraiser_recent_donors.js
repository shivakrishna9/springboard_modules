/**
 * Position currency suffix/prefix inline with the "Only show donations greater than" field.
 */
Drupal.behaviors.fundraiserRecentDonorsBehavior = {
  attach: function(context) { (function($) {
    $('.form-item-show-recent-donation-greater-than input#edit-show-recent-donation-greater-than').after($('span.recent-donors-currency-symbol-after'));
    $('.form-item-show-recent-donation-greater-than input#edit-show-recent-donation-greater-than').before($('span.recent-donors-currency-symbol-before'));
  })(jQuery); }
}
