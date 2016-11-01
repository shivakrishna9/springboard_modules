(function($) {
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
    this.$amount = this.$form.find('input[name="submitted[donation][amount]"]');
    this.amount = 0;
    this.deviceDataInstance = null;
    this.$deviceDataInput = $('<input name="device_data" type="hidden"/>');

    var parent = this;

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

        if (parent.creditEnabled()) {
          parent.createHostedFields(clientInstance);
        }
        if (parent.paypalEnabled()) {
          parent.createPaypalFields(clientInstance);
        }

        parent.resetDeviceData();
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
      if (parent.settings.currentPaymentMethod == paymentMethod) {
        return;
      }

      parent.settings.currentPaymentMethod = paymentMethod;
      if (paymentMethod == 'credit') {
        parent.disablePaypalFieldsSubmit().resetHostedFieldsSubmit();
      }
      else {
        parent.disableHostedFieldsSubmit().resetPaypalFieldsSubmit();
      }

      parent.resetDeviceData();
      return this;
    };

    /**
     * Helper function to determine if credit payment method is enabled.
     */
    this.creditEnabled = function() {
      return this.settings.enabledMethods.indexOf('credit') >= 0;
    };

    this.setClientInstance = function(clientInstance) {
      this.clientInstance = clientInstance;
      return this;
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

    /**
     * Creates hosted fields.
     * @param clientInstance
     *   The client instance.
     */
    this.createHostedFields = function(clientInstance) {
      this.setClientInstance(clientInstance);
      braintree.hostedFields.create({
        client: this.clientInstance,
        styles: this.settings.fieldsStyles,
        fields: this.settings.hostedFields
      }, function(error_message, hostedFieldsInstance) {
        if (error_message) {
          error(error_message);
          return;
        }
        parent.hostedFieldsInstance = hostedFieldsInstance;
        parent.resetHostedFieldsSubmit();
        parent.enableHostedFieldsEvents();
      });

      return this;
    };

    /**
     * Tears down hosted fields.
     * @param event
     *   The event parameter from the listener that called this function.
     */
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

    /**
     * Helper function to determine if bank payment method is enabled.
     */
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
      this.setClientInstance(clientInstance);
      braintree.paypal.create({
        client: this.clientInstance
      }, function (error_message, paypalInstance) {
        if (error_message) {
          error(error_message);
          return;
        }

        parent.setPaypalInstance(paypalInstance);
        parent.resetPaypalFieldsSubmit();
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
      }
    }
  };
})(jQuery);
