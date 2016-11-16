;(function($) {
  $(document).on('BraintreePaymentInstance', function(e, BraintreePaymentInstance, error) {
    BraintreePaymentInstance.addPaymentMethod('applepay', (function() {
      var BIAP = {
        applePayInstance: null
      };
      var BI = BraintreePaymentInstance;

      BIAP.preboot = function() {
        // If Apple Pay is enabled, check if the browser supports it.
        if ($.inArray('applepay', BI.settings.enabledMethods) !== false) {
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
            BI.removePaymentMethod('applepay');
          }
        }
      };

      BIAP.isEnabled = function() {
        return BI.settings.enabledMethods.indexOf('applepay') >= 0;
      };

      BIAP.setInstance = function(applePayInstance) {
        BIAP.applePayInstance = applePayInstance;
        return BIAP;
      };

      BIAP.enableFieldsSubmit = function() {
        BI.$form.on('submit.braintree_applepay', BIAP.submitFields);
        return BIAP;
      };

      BIAP.disableFieldsSubmit = function() {
        BI.$form.off('submit.braintree_applepay');
        return BIAP;
      };

      BIAP.resetFieldsSubmit = function() {
        BIAP.disableFieldsSubmit().enableFieldsSubmit();
        return BIAP;
      };

      BIAP.submitFields = function(event) {
        if (BI.settings.currentPaymentMethod != 'applepay') {
          return;
        }

        if (!BI.$nonce.val().length) {
          event.preventDefault();
          var paymentRequest = BIAP.applePayInstance.createPaymentRequest({
            total: {
              label: 'Jackson River',
              amount: BI.amount
            }
          });

          var session = new ApplePaySession(1, paymentRequest);

          session.onvalidatemerchant = function(event) {
            console.log(event);
            BIAP.applePayInstance.performValidation({
              validationURL: event.validationURL,
              displayName: 'Jackson River'
            }, function (validationErr, merchantSession) {
              if (validationErr) {
                // You should show an error to the user, e.g. 'Apple Pay failed to
                // load.'
                error('Error validating merchant:', validationErr);
                session.abort();
                return;
              }
              session.completeMerchantValidation(merchantSession);
            });
          };

          session.onpaymentauthorized = function (event) {
            BIAP.applePayInstance.tokenize({
              token: event.payment.token
            }, function(tokenizeErr, payload) {
              if (tokenizeErr) {
                error('Error tokenizing Apple Pay:', tokenizeErr);
                session.completePayment(ApplePaySession.STATUS_FAILURE);
                return;
              }
              session.completePayment(ApplePaySession.STATUS_SUCCESS);

              BI.$nonce.val(payload.nonce);
            });
          }
        };

        session.begin();
      };

      BIAP.createFields = function(clientInstance) {
        BI.setClientInstance(clientInstance);
        if (BIAP.applepayFieldsCreated) {
          return BIAP;
        }

        braintree.applePay.create({
          client: BI.clientInstance
        }, function (error_message, applePayInstance) {
          if (error_message) {
            error(error_message);
            return;
          }

          BIAP.setInstance(applePayInstance);

          var promise = ApplePaySession.canMakePaymentsWithActiveCard(applePayInstance.merchantIdentifier);
          promise.then(function(canMakePaymentsWithActiveCard) {
            if (canMakePaymentsWithActiveCard) {
              BIAP.resetFieldsSubmit();
              BIAP.applepayFieldsCreated = true;
            }
          });
        });
        return BIAP;
      };

      return BIAP;
    })());
  })
})(jQuery);
