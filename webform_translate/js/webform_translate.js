/**
 * @file
 * js for Webform Translate
 */

Drupal.behaviors.webformTranslateManagerBehavior = {
  attach: function(context) { (function($) {
    // Expand fields on focus:
    $('#webform-translate-translation-manager-form .form-item.form-type-textfield input').each(function () {
      $(this).focus(function () {
        $(this).addClass('focus-active');
      });
      $(this).blur(function () {
        $(this).removeClass('focus-active');
      });
    });

    // Alter the language filter so the user's current langauge is selected and undesired options are removed:
    var targetLanguage = $('.view-webform-translate-translation-manager .view-filters #edit-language').attr('default_lang_code');
    $('.view-webform-translate-translation-manager #edit-language option').each(function () {
      var targetLanguageFound = false;
      if ($(this).val() == 'All'
        || $(this).val() == 'en'
        || $(this).val() == '***CURRENT_LANGUAGE***'
        || $(this).val() == '***DEFAULT_LANGUAGE***'
        || $(this).val() == 'und') {
        if ($(this).attr('selected') == 'selected') {
          targetLanguageFound = true;
        }
        $(this).remove();
      }
      if (targetLanguageFound == true && $(this).val() == targetLanguage) {
        $(this).attr('selected', 'selected');
      }
    });
  })(jQuery); }
  
}
