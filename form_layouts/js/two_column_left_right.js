Drupal.behaviors.formLayoutsTwoColLeftRight = {
  attach: function(context) { (function($) {

  // Add even/odd tags to fieldsets in webform forms.
  $('.webform-client-form .webform-component-fieldset').filter(':even').each(function() {
    $(this).addClass('even');
  });
  $('.webform-client-form .webform-component-fieldset').filter(':odd').each(function() {
    $(this).addClass('odd');
  });
  $('.webform-client-form .webform-component-fieldset').filter(':last').each(function() {
    $(this).addClass('last');
    $(this).after('<div style="clear:both"></div>');
  });

  })(jQuery); }
}
