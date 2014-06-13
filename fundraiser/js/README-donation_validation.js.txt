
Fundraiser Validation:

Basic form validation can be found within fundraiser/js/donation_validation.js. The vaidation leverages the jQuery Validate library and is attached to the Drupal.behaviors.springboardForms JavaScript object on page load.

At the beginning of this JS file there is a helper method called .clearEle() that has been added to jQuery. This method clears out the validation for an input element, when called on that element, and reset's any classes that have been added by validation.

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

Alongside the built-in jQuery Validation methods, we have added two additional methods: the 'creditcard' and 'amount' methods. 

The 'creditcard' validator method uses a regex to check that the credit card field number entered is a valid AMEX, VISA, MASTERCARD, DISCOVER, Diner's Club or JCB credit card. This validator does not include Australian Bankcard, Dankort (PBS) cards or Switch/Solo (Paymentech) validation checking. At this time there is no way to customize the regex validator without changing this file directly.

The 'amount' validator is a regex that checks the state of the other_input field when a donor has chosen a custom donation amount. This validator insures that the value entered is in the correct format before submission.

The form uses the built-in jQuery Validation 'number' method for the Zipcode (5 characters) and CVV (3 or 4 characters).

The Zipcode validator below also overrides the default messages that display upon each condition:

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

At the bottom of donation_validation.js there is a list that allows you to set the default validator message for different condition. However, this will be overridden by setting a specific message in the case where you declare a custom message directly on the element (as shown in the snippet directly above).

