(function ($) {
  Drupal.behaviors.m2t = {
    attach: function (context, settings) {

      var addressFields = 'input[name$="address]"], input[name*="city"], select[name*="state"], input[name*="zip"]';
      $('form.geocode-form #webform-component-springboard-m2t-message').after('<div class = "legislators"></div>');
      $('form.geocode-form').on('blur', addressFields, function(){
        parseAddressFields();
      });

      $('form.geocode-form').on('change', 'select[name*="state"]', function(){
        parseAddressFields();
      });

      function parseAddressFields() {
        var addressObj = {};
        $(addressFields,'form.geocode-form').each(function(){
           if(this.value === '') {
            return false;
           }
           else {
            addressObj[this.name] = this.value;
           }
           if(this.name == 'submitted[sbp_zip]' && this.value !== '')  {
              lookupGeo(addressObj);
           }
        });
      }

      function lookupGeo(addressObj) {
        activeGeocoder = Drupal.settings.springboard_m2t.geocoder;
        if (activeGeocoder == 'smartystreets_rest') {
          data = {address: addressObj};
          console.log(data);
          url = '/smartylookup';
        }
        // else if (activeGeocoder == 'melissa') {
        //   data = {onelineaddress: address};
        //   url = '/lookup';
        //   console.log(address);
        // }

        $.ajax({
          type: "POST",
          url: url,
          data: data,
          dataType: 'json',
          success: function(data) {
            console.log(data['response']);
            response = $.parseJSON(data['response']);
            text = '<P>';
            $(response.legislators).each(function(){

              text += '<P>' + this[0].LegalName + '<BR />';
              text += this[0].DistrictDesc  + '<BR />';

            });
          $('form.geocode-form div.legislators').html(text);
          },
          error: function(xhr, textStatus, error){
            console.log(error);
          }
        });
      }


      (function($){
        $.fn.clearEle = function() {
          return this.each(function() {
            $(this).removeClass('valid');
            $(this).next('label').remove();
            $(this).parents('.success').removeClass('success');
            $(this).parents('.error').removeClass('error');
            this.value = '';
          });
        };
      })(jQuery);
      $(window).ready(function(){
        // Custom zipcode validation
        $.validator.addMethod('zipcode', function(value, element) {
          // U.S. or Canadian Zip codes
          return this.optional(element) || /(^\d{5}((-|\s)\d{4})?$)|(^[ABCEGHJKLMNPRSTVXY]{1}\d{1}[A-Z]{1} *\d{1}[A-Z]{1}\d{1}$)/i.test(value);
        }, "Enter a valid zipcode");

        jQuery.validator.addMethod("phoneUS", function(phone_number, element) {
          phone_number = phone_number.replace(/\s+/g, "");
          return this.optional(element) || phone_number.length > 9 && phone_number.match(/^(\+?1-?)?(\([2-9]\d{2}\)|[2-9]\d{2})-?[2-9]\d{2}-?\d{4}$/);
        }, "Please specify a valid phone number");

        // Instantiate Form Validation
        var m2tValidate = $('.geocode-form').validate({
          // Custom keyup function checking for tab key (9) and when value is empty
          onkeyup: function (element, event) {
            if ($(element).next('.error')[0]){
              if (event.which === 9 && element.value === "") {
                return;
              } else {
                this.element(element);
              }
            }
          },
          onfocusout: function (element) {
            // Callback for real-time onfocusout of form elements
            var isValid = $(element).valid();
            //
            if (typeof validateKeyCallback != "undefined" && isValid === 0) {
              // Set status to 0
              window.validateKeyCallback.status = 0;
              validateKeyCallback.error(element);
            } else if (typeof validateKeyCallback != "undefined" && isValid == 1) {
              // Set status to 1
              window.validateKeyCallback.status = 1;
              validateKeyCallback.success(element);
            }
          },
          highlight: function(element) {
            $(element).addClass('key-validate');
            $(element).closest('.control-group').removeClass('success').addClass('error');
          },
          success: function(element) {
            $(element).text('OK').addClass('valid').closest('.control-group').removeClass('error').addClass('success');
          }
        });
        $('input[name*="zip"]').rules("add", {
          required: true,
          zipcode: true,
        });
        $('input[name*="phone"]').rules("add", {
          required: true,
          phoneUS: true,
        });
        $('input[name*="mail"]').rules("add", {
          required: true,
          email: true,
        });
      });
    }
  };
})(jQuery);