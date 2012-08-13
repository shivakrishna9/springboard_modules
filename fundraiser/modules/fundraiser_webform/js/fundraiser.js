Drupal.behaviors.fundraiserCheckCode = {
  attach: function(context) { (function($) {
  
  $(document).ready(function(){

	// make sure cc number and cvv are numeric
	$('#edit-submitted-credit-card-information-card-number').numeric();
	$('#edit-submitted-credit-card-information-card-cvv').numeric();
	$('#edit-submitted-credit-card-information-card-expiration-date-day').hide();

	$('#edit-submitted-donation-quantity').change(function() {
		$('#total').empty();
		var total = this.value * $('#webform-component-amount input:radio:checked').val();
		$('#edit-submitted-donation-quantity').after('<p id="total">Total: $' + total + '</p>'); // TODO check money symbols.
	});
	
	$('#edit-submitted-donation-other-amount').change(function() {
		if ($(this).val() != '') {
			$(':radio[value=other]').attr('checked', true);
		}
	});
	
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