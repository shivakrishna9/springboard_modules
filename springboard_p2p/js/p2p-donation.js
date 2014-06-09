/**
 * @file
 * JQuery P2P Donation form.
 */
(function ($) {
  // Add drupal 7 code.
  Drupal.behaviors.p2pDonation = {
    attach: function (context, settings) {
// End drupal calls.

      // set a var for the p2p donation form page type.
      var p2pd = '.p2p-donation-form';

      // Instanstiate jquery mediacheck.
      mediaCheck({
        media: '(max-width: 980px)',
        entry: function() {
          $(p2pd  + " " +  '.panel-col-last').insertBefore(p2pd  + " " + '.panel-col-first');
        },
        exit: function() {
          $(p2pd  + " " +  '.panel-col-first').insertBefore(p2pd  + " " + '.panel-col-last');
        }
      });

      // Move the ssl cert over to the sidebar.
      $('#ssl-cert').appendTo('.panel-panel.panel-col-last .inside');

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

      // Add classes to enhance default forms that don't have bootstrap classes.
      $('.fundraiser-donation-form').validate({
        onfocusout: function (element) {
          $(element).valid();
        },
        highlight: function(element) {
          $(element).closest('.form-item').removeClass('success').addClass('error');
        },
        success: function(element) {
          element.text('OK').addClass('valid').closest('.form-item').removeClass('error').addClass('success');
        }
      });

      // end jquery.
    }};
})
  (jQuery);