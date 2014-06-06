/**
 * @file
 * Misc JQuery P2P scripts in this file.
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.p2pJS = {
    attach: function (context, settings) {
// End drupal calls.

      // Add a class to the parent for better theming.
      $('#edit-mail').parent('.control-group').addClass('email-wrapper');

    // Get rid of inline styles.
   $('.panel-display p').attr('style', '');

/* P2P donation form ***********/

      // Set a media width to react on.
      var jRes = jRespond([
        {
          label: 'narrow',
          enter: 0,
          exit: 980
        }
      ]);

      // set a var for the p2p donation form page type.
      var p2pd = '.p2p-donation-form';

      // Initialize the jres function to swap divs on breakpoints.
      jRes.addFunc({
        breakpoint: 'narrow',
        enter: function() {
          $(p2pd  + " " +  '.panel-col-last').insertBefore(p2pd  + " " + '.panel-col-first');
        },
        exit: function() {
          $(p2pd  + " " +  '.panel-col-first').insertBefore(p2pd  + " " + '.panel-col-last');
        }
      });

      // Add meaningful classes to webform radio buttons & labels.
      $('input[type=checkbox]').addClass('checkbox-input');
      $('input[type=checkbox]').next('label').addClass('checkbox-label');

      $('.webform-client-form input[type="radio"]').each(function() {
        $(this).next("label").addClass($(this).attr("type") + '-' + $(this).attr("value"));
        $(this).addClass('radio-button-input');
        $(this).next("label").addClass('radio-label');
      });

      // checkboxes checked / not checked.
      $('input[type=checkbox]').click(function () {
        if ($(this).is(':checked')) {
          $(this).next('label').toggleClass('checked', this.checked).removeClass('not-checked');
        }
        else {
          $(this).next('label').addClass('not-checked').removeClass('checked');
        }
      });

      // Generic checked / unchecked for radio.
      var radioChecked = $('.single-radio input').is(':checked');
      $('.single-radio input').click(function() {
        radioChecked = !radioChecked;
        $(this).attr('checked', radioChecked);
      });

      // Test for radio onclick.
      var $radioButtons = $('.webform-client-form input[type="radio"]');
      $radioButtons.click(function() {
        $radioButtons.each(function() {
          $(this).next('label').toggleClass('radio-checked', this.checked);
        });
      });

      // Remove the padlock icon, we replace with font-awesome.
      $('.fundraiser_submit_message img ').remove();

      // Show / hide other amount input box when other amount is clicked / touched.
      $('.webform-client-form .option.radio-other.radio-label').click(function() {
        $('#other-wrapper').css('display', 'inline-block');
      });

      $('.webform-client-form .option.radio-label:not(".radio-other")').click(function () {
          $('#other-wrapper').css('display', 'none');
      });

      // end jquery.
    }};
})
(jQuery);