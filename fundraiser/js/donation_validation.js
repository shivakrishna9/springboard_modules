(function($) {
  Drupal.behaviors.springboardForms = {
    attach: function (context, settings) {
      // jQuery clearEle definition: clears out validation classes
      (function($){
        $.fn.clearEle = function() {
          return this.each(function() {
            $(this).removeClass('valid');
            $(this).next('label').remove();
            $(this).parents('.success').removeClass('success');
            $(this).parents('.error').removeClass('error');
            this.value = '';
          });
        }
      })(jQuery);

      $(window).ready(function(){

        // Custom Validation Regex rules: AMEX, VISA, MASTERCARD, DISCOVER, Diner's Club, JCB
        $.validator.addMethod('creditcard', function(value, element) { 
          return this.optional(element) || /^(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|6(?:011|5[0-9][0-9])[0-9]{12}|3[47][0-9]{13}|3(?:0[0-5]|[68][0-9])[0-9]{11}|(?:2131|1800|35\d{3})\d{11})$/i.test(value);   
          // Doesn't work for Australian Bankcard, Dankort (PBS) cards or Switch/Solo (Paymentech)
          // Bankcard regexp below needs fixing
          //^5610\5[6-9]d{2}\d{4}\d{4}$
        }, "Enter a valid credit card number");
        
        // Custom amount validation
        $.validator.addMethod('amount', function(value, element) {
          // Add regexp      
          return this.optional(element) || /^[0-9]*(\.\d{1,3})*(,\d{1,3})?$/i.test(value);
        }, "Enter a valid amount");

        // Instantiate Form Validation
        $('.fundraiser-donation-form').validate({
            onfocusout: function (element) {
            $(element).valid();
          },
          highlight: function(element) {
            $(element).closest('.control-group').removeClass('success').addClass('error');
          },
          success: function(element) {
            element.text('OK').addClass('valid').closest('.control-group').removeClass('error').addClass('success');
          }
        });         

        // Check initial state of form
        $('.fundraiser-donation-form').once(function(){
          if ($(this).validate().checkForm()) {
            $('#edit-submit').removeClass('button_disabled').attr('disabled', false);
          } else {
            $('#edit-submit').addClass('button_disabled').attr('disabled', true);
          }
        });

        // Prevent submissions if errors are present
        $('.fundraiser-donation-form').bind('change keyup', function() {
          if ($(this).validate().checkForm()) {
            $('#edit-submit').removeClass('button_disabled').attr('disabled', false);
          } else {
            $('#edit-submit').addClass('button_disabled').attr('disabled', true);
          }
        });

         // Zipcode custom validation rule
        $('input[name~="zip"]').rules("add", {
          required: true,
          number: true,
          minlength:5,
          messages: {
            required: "This field is required",
            minlength: "Minimum of 5 characters"
          }
        });
          // CVV custom validation rule
        $('input[name~="card_cvv"]').rules("add", {
          required: true,
          number: true,
          minlength:3,
          maxlength:4,
          messages: {
            required: "This field is required",
            minlength: "Minimum of 3 characters",
            maxlength: "Maximum of 4 characters"
          }
        });
        // Credit Card custom validation rule
        $('input[name~="card_number"]').rules("add", {
          required: true,
          creditcard: true,
          messages: { 
            required: "This field is required",
            creditcard: "Enter a valid credit card number",
          }
        });
        // Other Amount
        $('input[name~="other_amount"]').rules("add", {
          required: {
            depends: function(element) {
              if ($('input[type="radio"][name$="[amount]"][value="other"]').is(":checked"))
                return true;
              else 
                return false;
            }
          }, 
            amount: true,
            messages: { 
            required: "This field is required",
            amount: "Enter a valid amount",
          }
        });
                  
        // Focus and Blur conditional functions
        $('input[type="radio"][name~="amount"]').change(function(){
          if ($(this).val() == 'other') {
            $('input[name~="other_amount"]').focus();
          } else {
            $('input[name~="other_amount"]').clearEle();
          }
        });
        $('input[name~="other_amount"]').focus(function(){
          $('input[type="radio"][name~="amount"][value="other"]').attr('checked', 'checked');
        })
        // Runs on Other Amount field
        $('input[name~="other_amount"]').blur(function(){
          var value = $(this).val();
          // Match 1-3 decimal/comma places and fix value
          if (value.match(/([\.,\-]\d{1}?)$/)) {
            $(this).val($(this).val().slice(0, -2));
          } else if (value.match(/([\.,\-]\d{2}?)$/)) {
            $(this).val($(this).val().slice(0, -3));
          } else if (value.match(/([\.,\-]\d{3}?)$/)) {
            $(this).val($(this).val().replace(/\D/g,''));
          }
        });
        // Ability to override the default message
        jQuery.extend(jQuery.validator.messages, {
          required: "This field is required",
          remote: "Please fix this field",
          email: "Enter a valid email address",
          url: "Enter a valid URL",
          date: "Enter a valid date",
          dateISO: "Enter a valid date (ISO)",
          number: "Must be a number", // changed 
          digits: "Enter only digits",
          creditcard: "Enter a valid credit card number",
          equalTo: "Enter the same value again",
          accept: "Enter a value with a valid extension",
          maxlength: jQuery.validator.format("Enter no more than {0} characters"),
          minlength: jQuery.validator.format("Enter at least {0} characters"),
          rangelength: jQuery.validator.format("Enter a value between {0} and {1} characters long"),
          range: jQuery.validator.format("Enter a value between {0} and {1}"),
          max: jQuery.validator.format("Enter a value less than or equal to {0}"),
          min: jQuery.validator.format("Enter a value greater than or equal to {0}")
          });
        // Small helper item
        $('select').each(function(){
          if ($(this).next().is('select')) {
            $(this).next().addClass('spacer');
          }
        });

      }); // window.ready
    } // attach.function
  } // drupal.behaviors
})(jQuery);