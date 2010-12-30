$(document).ready(function(){

	// make sure cc number and cvv are numeric
	$('#edit-submitted-credit-card-information-card-number').numeric();
	$('#edit-submitted-credit-card-information-card-cvv').numeric();
	$('#edit-submitted-credit-card-information-card-expiration-date-day').hide();

	$('#edit-submitted-donation-quantity').change(function() {
		$('#total').empty();
		var total = this.value * $('#webform-component-amount input:radio:checked').val();
		$('#edit-submitted-donation-quantity').after('<p id="total">Total: $' + total + '</p>');
	});
	
	$('#edit-submitted-donation-other-amount').change(function() {
		if ($(this).val() != '') {
			$(':radio[value=other]').attr('checked', true);
		}
	});
	
	$('.node .webform-client-form #edit-submit').click(function() {
	  $(this).hide();
	  $('.fundraiser_submit_message').hide();
    var path = Drupal.settings.basePath + 'sites/all/modules/springboard/fundraiser/';
    $(this).after('<div style="clear: right; float: right; padding-right: 167px; height: 16px; width: 16px; background: url(' + path + 'loader.gif) no-repeat left top;"></style>').after('<p style="clear: right; float: right; margin: 0; padding: 0 75px 0.5em 0; text-align: center; width: 200px;">Thank you.</p><p style="clear: right; float: right; margin: 0; padding: 0 75px 1em 0; text-align: center; width: 200px;">Your donation is being processed.</p>');
	});
});