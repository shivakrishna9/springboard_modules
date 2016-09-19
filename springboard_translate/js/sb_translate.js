/**
 * @file
 * js for Webform Translate
 */

Drupal.behaviors.sbTranslateManagerBehavior = {
  attach: function(context) { (function($) {
    // Expand fields with a lot of text on focus:
    $('#sb-translate-translation-manager-form .form-item.form-type-textfield').each(function () {
      $(this).mouseover(function () {
        if ($(this).parent().hasClass('source-string')) {
          $(this).children('input').each(function() {
            if ($(this).val().length > 32) {
              $(this).addClass('focus-active');
            }
          });
        }
      });
      $(this).mouseout(function () {
        if ($(this).parent().hasClass('source-string')) {
          $(this).children('input').each(function() {
            $(this).removeClass('focus-active');
          });
        }
      });
    });
    $('#sb-translate-translation-manager-form .form-item.form-type-textfield input').each(function () {
      $(this).keydown(function () {
        if ($(this).val().length > 32) {
          $(this).addClass('focus-active');
        } else {
          $(this).removeClass('focus-active');
        }
      });
      $(this).focus(function () {
        if ($(this).val().length > 32) {
          $(this).addClass('focus-active');
        }
      });
      $(this).blur(function () {
        $(this).removeClass('focus-active');
      });
    });
    // Show or hide state options based on currently selected country:
    $('.view-webform-translate-translation-manager .trans-manager-state-selector').change(function () {
      _transManagerUpdateStateOptions($(this).val());
    });
    _transManagerUpdateStateOptions($('.view-webform-translate-translation-manager .trans-manager-state-selector').val());
    function _transManagerUpdateStateOptions(newCountry) {
      $('#sb-translation-manager .form-type-textfield input').each(function () {
        if ($(this).attr('country_code') != undefined) {
          if (newCountry != 'all-countries' && $(this).attr('country_code') != newCountry) {
            $(this).parent().parent().parent().hide();
          }
          else {
            $(this).parent().parent().parent().show();
          }
        }
      });
    }
    // Show or hide rows based on the selected textgroup:
    function _transManagerSelectTextgroup(textgroup) {
      $('#sb-translation-manager td').each(function () {
        if ($(this).hasClass(textgroup)) {
          $(this).parent().show();
        }
        else {
          $(this).parent().hide();
        }
      });
    }
    // Cause textgroups to update if a textgruop tab is clicked:
    $('#edit-translation-manager .sb-textgroup-tabs a').each(function () {
      $(this).click(function () {
        $('#edit-translation-manager .sb-textgroup-tabs a').each(function () {
          $(this).removeClass('active');
          $(this).parent().removeClass('active');
        });
        _transManagerSelectTextgroup($(this).attr('textgroup'));
        $(this).addClass('active');
        $(this).parent().addClass('active');
      });
    });
    $('#edit-translation-manager .sb-textgroup-tabs a:first').click();
  })(jQuery); }
  
}
