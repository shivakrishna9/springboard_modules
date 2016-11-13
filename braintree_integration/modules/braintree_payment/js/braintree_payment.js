;(function($) {
  var error = function() {
    return !!console && console.error.apply(this, arguments);
  };

  var BraintreePayment = function(settings) {
    this.settings = settings;
    this.$form = $('#' + settings.formId);
    this.$submit = this.$form.find('input[type=submit]');
    this.$nonce = this.$form.find('input[name=payment_method_nonce]');
    this.clientInstance = null;
    this.hostedFieldsInstance = null;
    this.paypalInstance = null;
    this.applePayInstance = null;
    this.$amount = this.$form.find('input[name="submitted[donation][amount]"]');
    this.amount = 0;
    this.deviceDataInstance = null;
    this.$deviceDataInput = $('<input name="device_data" type="hidden"/>');

    var parent = this;
    var processed = false;

    this.hostedFieldsCreated = false;
    this.paypalFieldsCreated = false;
    this.applepayFieldsCreated = false;

    this.updateAmount = function() {
      parent.amount = parent.$amount.filter(':checked').val();

      if (undefined == parent.amount) {
        return;
      }

      if (parent.amount == 'other') {
        $('input[name="submitted[donation][other_amount]"]').on('change', function() {
          parent.amount = $(this).val();
        });
      }
    };

    /**
     * Initializes the Braintree client.
     */
    this.bootstrap = function() {
      braintree.client.create({
        authorization: this.settings.clientToken
      }, function(error_message, clientInstance) {
        if (error_message) {
          error(error_message);
          return;
        }

        parent.setClientInstance(clientInstance);
        parent.resetDeviceData();
        parent.setCurrentPaymentMethod(parent.settings.currentPaymentMethod);
      });

      parent.updateAmount();
      parent.$amount.on('change', parent.updateAmount);

      return this;
    };

    this.resetDeviceData = function() {
      if (null !== parent.deviceDataInstance) {
        parent.deviceDataInstance.teardown();
      }

      parent.deviceDataInstance = null;
      parent.$deviceDataInput.val('');
      // Add fraud protection.
      if (parent.settings.currentPaymentMethod == 'paypal') {
        braintree.dataCollector.create({
          client: parent.clientInstance,
          paypal: true
        }, function (error_message, dataCollectorInstance) {
          if (error_message) {
            error(error_message);
            return;
          }
          // At this point, you should access the
          // dataCollectorInstance.deviceData value and provide it to your
          // server, e.g. by injecting it into your form as a hidden input.
          parent.deviceDataInstance = dataCollectorInstance;
          parent.$deviceDataInput.val(parent.deviceDataInstance.deviceData);
        });
      }
    }

    this.setCurrentPaymentMethod = function(paymentMethod) {
      if (parent.settings.currentPaymentMethod == paymentMethod && processed) {
        return this;
      }

      parent.settings.currentPaymentMethod = paymentMethod;
      if (paymentMethod == 'credit' && parent.creditEnabled()) {
        parent.createHostedFields(parent.clientInstance);
        parent.disablePaypalFieldsSubmit().disableApplePayFieldsSubmit().resetHostedFieldsSubmit();
      }
      else if (paymentMethod == 'paypal' && parent.paypalEnabled()) {
        parent.createPaypalFields(parent.clientInstance);
        parent.disableHostedFieldsSubmit().disableApplePayFieldsSubmit().resetPaypalFieldsSubmit();
      }
      else if (paymentMethod == 'applepay' && parent.applePayEnabled()) {
        parent.createApplePayFields(parent.clientInstance);
        parent.disableHostedFieldsSubmit().disablePaypalFieldsSubmit().resetApplePayFieldsSubmit();
      }

      parent.resetDeviceData();
      processed = true;
      return this;
    };

    this.setClientInstance = function(clientInstance) {
      this.clientInstance = clientInstance;
      return this;
    };

    /**
     * Helper function to determine if credit payment method is enabled.
     */
    this.creditEnabled = function() {
      return this.settings.enabledMethods.indexOf('credit') >= 0;
    };

    this.enableHostedFieldsEvents = function() {
      parent.hostedFieldsInstance.on('validityChange', function(event) {
        $(document).trigger('braintree.fieldEvent', [event]);
      });
      parent.hostedFieldsInstance.on('inputSubmitRequest', function(event) {
        parent.$submit.click();
      });
      return this;
    };

    this.disableHostedFieldsEvents = function() {
      parent.hostedFieldsInstance.off('validityChange');
      parent.hostedFieldsInstance.off('inputSubmitRequest');
      return this;
    };

    this.enableHostedFieldsSubmit = function() {
      parent.$form.on('submit.braintree_hosted', parent.submitHostedFields);
      return this;
    };

    this.disableHostedFieldsSubmit = function() {
      parent.$form.off('submit.braintree_hosted');
      return this;
    };

    this.resetHostedFieldsSubmit = function() {
      parent.disableHostedFieldsSubmit().enableHostedFieldsSubmit();
      return this;
    };

    this.createHostedFields = function(clientInstance) {
      parent.setClientInstance(clientInstance);
      if (parent.hostedFieldsCreated) {
        return this;
      }

      braintree.hostedFields.create({
        client: parent.clientInstance,
        styles: parent.settings.fieldsStyles,
        fields: parent.settings.hostedFields
      }, function(error_message, hostedFieldsInstance) {
        if (error_message) {
          error(error_message);
          return;
        }
        parent.hostedFieldsInstance = hostedFieldsInstance;
        parent.resetHostedFieldsSubmit();
        parent.enableHostedFieldsEvents();
        parent.hostedFieldsCreated = true;
      });

      return this;
    };

    this.submitHostedFields = function(event) {
      if (parent.settings.currentPaymentMethod != 'credit') {
        return;
      }

      if (!parent.$nonce.val().length) {
        event.preventDefault();
        var triggerBraintreeFieldEvent = function() {
          var state = parent.hostedFieldsInstance.getState();
          for (field in state.fields) {
            state.emittedBy = field;
            $(document).trigger('braintree.fieldEvent', [state]);
          }
        };
        parent.hostedFieldsInstance.tokenize(function(error_message, payload) {
          if (error_message) {
            switch (error_message.code) {
              case 'HOSTED_FIELDS_FIELDS_EMPTY':
                triggerBraintreeFieldEvent();
                error('All fields are empty! Please fill out the form.');
                break;
              case 'HOSTED_FIELDS_FIELDS_INVALID':
                triggerBraintreeFieldEvent();
                error('Some fields are invalid:', error_message.details.invalidFieldKeys);
                break;
              case 'HOSTED_FIELDS_FAILED_TOKENIZATION':
                error('Tokenization failed server side. Is the card valid?');
                break;
              case 'HOSTED_FIELDS_TOKENIZATION_NETWORK_ERROR':
                error('Network error occurred when tokenizing.');
                break;
              default:
                error('Something bad happened!', error_message);
            }
            parent.$submit.show().next('.donation-processing-wrapper').remove();
            return;
          }
          parent.$nonce.val(payload.nonce);
          parent.$form.off('submit.braintree_hosted').submit();
        });
      }

      return this;
    };

    this.paypalEnabled = function() {
      return this.settings.enabledMethods.indexOf('paypal') >= 0;
    };

    this.setPaypalInstance = function(paypalInstance) {
      this.paypalInstance = paypalInstance;
      return this;
    };

    this.enablePaypalEvents = function() {
      parent.paypalInstance.on('validityChange', function(event) {

      });
      return this;
    };

    this.disablePaypalEvents = function() {
      parent.paypalInstance.off('validityChange');
      return this;
    }

    this.enablePaypalFieldsSubmit = function() {
      parent.$submit.on('click.braintree_paypal', parent.submitPaypalFields);
      return this;
    };

    this.disablePaypalFieldsSubmit = function() {
      parent.$submit.off('click.braintree_paypal');
      return this;
    };

    this.resetPaypalFieldsSubmit = function() {
      parent.disablePaypalFieldsSubmit().enablePaypalFieldsSubmit();
      return this;
    };

    this.createPaypalFields = function(clientInstance) {
      parent.setClientInstance(clientInstance);
      if (parent.paypalFieldsCreated) {
        return this;
      }

      braintree.paypal.create({
        client: this.clientInstance
      }, function (error_message, paypalInstance) {
        if (error_message) {
          error(error_message);
          return;
        }

        parent.setPaypalInstance(paypalInstance);
        parent.resetPaypalFieldsSubmit();
        parent.paypalFieldsCreated = true;
      });

      return this;
    };

    this.submitPaypalFields = function(event) {
      if (parent.settings.currentPaymentMethod != 'paypal') {
        return;
      }

      if (parent.amount == undefined || parent.amount == '') {
        // Select the amount first.
        alert('Please select a donation amount.');
        event.preventDefault();
        return;
      }

      if (!parent.$nonce.val().length) {
        event.preventDefault();
        parent.paypalInstance.tokenize({
          flow: 'vault',
          amount: parent.amount,
          currency: 'USD',
          locale: 'en_us',
          enableShippingAddress: false
        }, function (error_message, payload) {
          if (error_message) {
            error(error_message);
            return;
          }

          // Tokenization succeeded!
          parent.$nonce.val(payload.nonce);
          $('#braintree-paypal-loggedin').show();
          $('#bt-pp-email').text(payload.details.email);

          // Bind cancel button to restore PayPal form.
          $('#bt-pp-cancel').on('click', function(event) {
            event.preventDefault();
            parent.reset();
          });

          var autofilled = parent.autofill(payload);
          // Auto-submit the form if no fields were auto-filled from the values
          // in the payload object.
          if (!autofilled) {
            parent.$form.submit();
          }

          parent.$form.off('submit.braintree_paypal');
        });
      }
    };

    /**
     * Helper function to determine if bank payment method is enabled.
     */
    this.applePayEnabled = function() {
      return this.settings.enabledMethods.indexOf('applepay') >= 0;
    };

    this.setApplePayInstance = function(applePayInstance) {
      this.applePayInstance = applePayInstance;
      return this;
    };

    this.enableApplePayFieldsSubmit = function() {
      parent.$form.on('submit.braintree_applepay', parent.submitApplePayFields);
      return this;
    };

    this.disableApplePayFieldsSubmit = function() {
      parent.$form.off('submit.braintree_applepay');
      return this;
    };

    this.resetApplePayFieldsSubmit = function() {
      parent.disableApplePayFieldsSubmit().enableApplePayFieldsSubmit();
      return this;
    };

    this.submitApplePayFields = function(event) {
      if (parent.settings.currentPaymentMethod != 'applepay') {
        return;
      }

      var paymentRequest = parent.applePayInstance.createPaymentRequest({
        total: {
          label: 'My Store',
          amount: parent.amount
        }
      });

      var session = new ApplePaySession(1, paymentRequest);

      session.onvalidatemerchant = function(event) {
        parent.applePayInstance.performValidation({
          validationURL: event.validationURL,
          displayName: 'My Store'
        }, function (validationErr, merchantSession) {
          if (validationErr) {
            // You should show an error to the user, e.g. 'Apple Pay failed to load.'
            error('Error validating merchant:', validationErr);
            session.abort();
            return;
          }
          session.completeMerchantValidation(merchantSession);
        });
      };

      session.onpaymentauthorized = function (event) {
        applePayInstance.tokenize({
          token: event.payment.token
        }, function(tokenizeErr, payload) {
          if (tokenizeErr) {
            error('Error tokenizing Apple Pay:', tokenizeErr);
            session.completePayment(ApplePaySession.STATUS_FAILURE);
            return;
          }
          session.completePayment(ApplePaySession.STATUS_SUCCESS);

          parent.$nonce.val(payload.nonce);
        });
      };

      session.begin();
    };

    this.createApplePayFields = function(clientInstance) {
      parent.setClientInstance(clientInstance);
      if (parent.applepayFieldsCreated) {
        return this;
      }

      braintree.applePay.create({
        client: parent.clientInstance
      }, function (error_message, applePayInstance) {
        if (error_message) {
          error(error_message);
          return;
        }

        parent.setApplePayInstance(applePayInstance);

        var promise = ApplePaySession.canMakePaymentsWithActiveCard(applePayInstance.merchantIdentifier);
        promise.then(function(canMakePaymentsWithActiveCard) {
          if (canMakePaymentsWithActiveCard) {
            parent.resetApplePayFieldsSubmit();
            parent.applepayFieldsCreated = true;
          }
        });
      });
      return this;
    };

    this.reset = function() {
      parent.$nonce.val('');

      // Reset submit button.
      this.$submit.removeAttr('disabled');

      // Remove device_data if added.
      var deviceDataInput = this.$form[0]['device_data'];
      if (undefined !== deviceDataInput) {
        this.$form.removeChild(deviceDataInput);
      }

      if (this.settings.currentPaymentMethod == 'paypal') {
        $('#braintree-paypal-loggedin').hide();
        $('#bt-pp-email').text('');
      }

      return this;
    };

    this.autofill = function(obj) {
      var autofill = this.settings.autofill;
      if (autofill == 'never') {
        return false;
      }

      var fieldsHaveBeenAutoFilled = false;
      var field_mapping = {
        'submitted[donor_information][first_name]': obj.details.firstName,
        'submitted[donor_information][last_name]': obj.details.lastName,
        'submitted[donor_information][mail]': obj.details.email,
        'submitted[billing_information][address]': obj.details.billingAddress.line1,
        'submitted[billing_information][address_line_2]': obj.details.billingAddress.line2,
        'submitted[billing_information][city]': obj.details.billingAddress.city,
        'submitted[billing_information][country]': obj.details.billingAddress.countryCode,
        'submitted[billing_information][state]': obj.details.billingAddress.state,
        'submitted[billing_information][zip]': obj.details.billingAddress.postalCode,
      };

      var fieldsAreEmpty = (function(field_mapping) {
        return $(field_mapping).filter(function(index, value) {
          return index != 'submitted[billing_information][country]' && $('[name="' + index + '"]').val() != '';
        }).get().length > 0;
      })(field_mapping);

      $.each(field_mapping, function(key, value) {
        var $field = $('[name="' + key + '"]');
        if (autofill == 'always' || (autofill == 'if_blank' && fieldsAreEmpty)) {
          $field.val(value);
          fieldsHaveBeenAutoFilled = true;
        }
      });

      return fieldsHaveBeenAutoFilled;
    }

    return this.bootstrap();
  };

  Drupal.behaviors.braintree = {
    attach: function(context, settings) {
      settings = settings.braintree;

      var $paymentMethod = $('input[name="submitted[payment_information][payment_method]"]');
      if ($paymentMethod.length) {
        settings.currentPaymentMethod = $paymentMethod.filter(':checked').val();
        // Get the available payment methods.
        settings.availableMethods = $paymentMethod.map(function() {
          return this.value;
        }).get();

        // Filter out the unavailable payment methods.
        settings.enabledMethods = $(settings.enabledMethods).filter(function(index, value) {
          return $.inArray(value, settings.availableMethods) >= 0;
        }).get();

        Drupal.braintreeInstance = new BraintreePayment(settings);
        $paymentMethod.on('change', function() {
          Drupal.braintreeInstance.setCurrentPaymentMethod($(this).val());
        });

        // If Apple Pay is enabled, check if the browser supports it.
        if ($.inArray('applepay', settings.enabledMethods)) {
          var supports_applepay = true;
          if (!window.ApplePaySession) {
            error('This device does not support Apple Pay.');
            supports_applepay = false;
          }
          if (supports_applepay && !ApplePaySession.canMakePayments()) {
            error('This device is not capable of making Apple Pay payments.');
            supports_applepay = false;
          }
          if (!supports_applepay) {
            // Remove the Apple Pay option.
            settings.availableMethods.splice($.inArray('applepay', settings.availableMethods), 1);
            var $applepay_input = $('input[name="submitted[payment_information][payment_method]"][value="applepay"]');
            if ($applepay_input.is(':checked')) {
              // If this method is currently checked, check the first payment
              // method instead.
              var $first_payment_method = $applepay_input.parent('.form-item').siblings('.form-item').eq(0).find('input[name="submitted[payment_information][payment_method]"]');
              $first_payment_method.attr('checked', 'checked');
              Drupal.braintreeInstance.setCurrentPaymentMethod($first_payment_method.val());
            }
            $applepay_input.parent('.form-item').remove();
          }
        }
      }
    }
  };
})(jQuery);
