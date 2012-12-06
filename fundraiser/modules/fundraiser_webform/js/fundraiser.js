Drupal.behaviors.fundraiserCheckCode = {
  attach: function(context) { (function($) {

  // Turn autocomplete off on CC and CVV form elements.
  $('#edit-submitted-credit-card-information-card-number, #edit-submitted-credit-card-information-card-cvv').attr('autocomplete','off');

  // Make sure cc number and cvv are numeric.
  $('#edit-submitted-credit-card-information-card-number').numeric();
  $('#edit-submitted-credit-card-information-card-cvv').numeric();
  $('#edit-submitted-credit-card-information-card-expiration-date-day').hide();

  // If quantity changes, update the total. Where needed.
  $('#edit-submitted-donation-quantity').change(function() {
    $('#total').empty();
    var total = this.value * $('#webform-component-amount input:radio:checked').val();
    $('#edit-submitted-donation-quantity').after('<p id="total">Total: $' + total + '</p>'); // TODO check money symbols.
  });

  // When other amount changes, make sure the amount radio is correctly set.
  $('#edit-submitted-donation-other-amount').change(function() {
    if ($(this).val() != '') {
      $('#webform-component-donation--amount :radio[value=other]').attr('checked', true);
    }
    // If the amount has been set to empty, or if the amount changed and set the radio box to other.
    // And the alert is visible, remove it.
    $('#fundraiser-amount-error').remove();
  });

  // When the radio button changes, if it was set to other and other still has a value in it.
  // Make sure the user picks one or the other.
  $('#webform-component-donation--amount input[name="submitted[donation][amount]"]').change(function() {
    var other_amount = $('#edit-submitted-donation-other-amount').val();
    if ($(this).val() != 'other' && other_amount !== 'undefined' &&  other_amount.length > 0) {
      // Throw up a message.
      $(this).parents('#webform-component-donation--amount').after('<div id="fundraiser-amount-error" class="messages error">' +
        'You have entered a custom amount and selected a set amount. Please choose the "Other" button if you ' +
        'intend to donate a custom amount, or clear the "Other Amount" text field to give a set amount.' +
        '</div>');
    }
    else {
      $('#fundraiser-amount-error').remove();
    }
  });

  // On submission hide the button and replace it with a new value.
  // Wrap the click in a once trigger to be sure that we bind it the one time.
  $('.node .webform-client-form #edit-submit').once(function() {
    $('.node .webform-client-form #edit-submit').click(function() {
      $(this).hide();
      $('.fundraiser_submit_message').hide();
      $(this).after('<div class="donation-processing-wrapper">' +
        '<p class="donation-thank-you">Thank you.</p>' +
        '<p class="donation-processing">Your donation is being processed.</p>' +
        '<div class="donation-processing-spinner"></div>' +
        '</div>');
    });
  });

  })(jQuery); }
} 