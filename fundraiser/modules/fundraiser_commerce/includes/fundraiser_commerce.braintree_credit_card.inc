<?php

/**
 * @file
 * Credit-card helper functions for fundraiser commerce braintree.
 */

/**
 * Returns a set of credit card form elements
 *
 * @return Array
 *    Form elements ready to be included on donation form
 */
function _fundraiser_commerce_braintree_credit_card_form() {

  global $user;

  $form['#attached']['library'][] = array('braintree_payment', 'braintree.js');

  // Initialize Braintree and create a token.
  braintree_initialize();

  // Move expiration date values up in the array. ONLY relevant for the default credit card fields.
  $form['card'] = array(
    '#markup' => '<label for="card-number">Card Number</label>
      <div id="card-number"></div>

      <label for="cvv">CVV</label>
      <div id="cvv"></div>

      <label for="expiration-date">Expiration Date</label>
      <div id="expiration-date"></div>'
  );
  $form['js'] =  array(
    '#markup' => '<script src="https://js.braintreegateway.com/js/braintree-2.26.0.min.js"></script>
    <script>
      braintree.setup("'.Braintree_ClientToken::generate().'", "custom", {
        id: "webform-client-form-2",
        hostedFields: {
          number: {
            selector: "#card-number"
          },
          cvv: {
            selector: "#cvv"
          },
          expirationDate: {
            selector: "#expiration-date"
          }
        }
      });
    </script>'
  );

  return $form;
}
