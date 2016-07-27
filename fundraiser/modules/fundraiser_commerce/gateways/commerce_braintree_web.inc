<?php

/**
 * @file
 * Commerce based hook for commerce_authnet.
 */

/**
 * Implements hook_fundraiser_commerce_fundraiser_gateway_info().
 */
function commerce_braintree_web_fundraiser_commerce_fundraiser_gateway_info() {
  return array(
    'payment_method' => array('credit', 'paypal'),
    'allow_recurring' => array('credit', 'paypal'),
    'allow_refund' => array('credit', 'paypal'),
    'form callback' => 'commerce_braintree_web_fundraiser_commerce_form',
    // 'cleanup callback' => 'commerce_authnet_fundraiser_commerce_cleanup',
    // 'scrub callback' => 'commerce_authnet_fundraiser_commerce_scrub',
    // 'expire callback' => 'commerce_authnet_fundraiser_commerce_expire',
    // 'validate callback' => 'commerce_authnet_fundraiser_commerce_validate',
    'charge callback' => 'commerce_braintree_web_fundraiser_commerce_charge',
    // 'update callback' => 'commerce_authnet_fundraiser_commerce_update',
    'refund callback' => 'commerce_braintree_web_fundraiser_commerce_refund',
    // 'cardonfile callback' => 'commerce_authnet_fundraiser_commerce_cardonfile',
    // 'response fields callback' => 'commerce_authnet_fundraiser_commerce_response_fields',
    // 'response save callback' => 'commerce_authnet_fundraiser_commerce_response_save',
  );
}

/**
 * Refund callback for authnet, wraps the admin form used for refund with our own system.
 */
function commerce_braintree_web_fundraiser_commerce_refund($payment_method, $refund) {
  // Retrieve data.
  $refunding_order = commerce_order_load($refund->did);
  $transactions = commerce_payment_transaction_load_multiple(array(), $conditions = array('order_id' => $refunding_order->order_id));
  $transaction = array_shift($transactions); // The first one is the original donation.

  $form = array();
  $form_state = array();
  // Set up the values needed for the transaction to go through.
  $form_state['transaction'] = $transaction;
  $form_state['values']['amount'] = $refund->amount;
  $form_state['order'] = $refunding_order;

  $form_state['payment_method'] = commerce_payment_method_instance_load($transaction->instance_id);
  // Submit the donation form.
  // module_load_include('inc', 'commerce_braintree_web', '/includes/commerce_authnet.admin');
  $result = commerce_braintree_web_refund_form_submit($form, $form_state);
  // At this point the card has been saved on the user profile.
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  return $success;
}

/**
 * Callback function, use this form for the given gateway.
 */
function commerce_braintree_web_fundraiser_commerce_form($payment_method) {
  if ($payment_method == 'credit') {
     $form = fundraiser_commerce_braintree_credit_card_form();
  }
  if ($payment_method == 'paypal') {
     $form = fundraiser_commerce_braintree_paypal_form();
  }
  return $form;
}

/**
 *
 */
function fundraiser_commerce_braintree_paypal_form() {
  global $user;

  // $form['#attached']['library'][] = array('braintree_payment', 'braintree.js');

  // Initialize Braintree and create a token.
  braintree_initialize();

  $form['card'] = array(
    '#markup' => '<div id="paypal-container"></div>'
  );
  $form['js'] =  array(
    '#markup' => '<script src="https://js.braintreegateway.com/js/braintree-2.26.0.min.js"></script>
    <script>
      braintree.setup("'.Braintree_ClientToken::generate().'", "paypal", {
        id: "webform-client-form-2",
        paypal: {
          container: "paypal-container",
        }
      });
    </script>'
  );
  return $form;
}

/**
 *
 */
function fundraiser_commerce_braintree_credit_card_form() {

  global $user;

  $form['#attached']['library'][] = array('braintree_payment', 'braintree.js');

  // Initialize Braintree and create a token.
  braintree_initialize();

  // Move expiration date values up in the array. ONLY relevant for the default credit card fields.
  $form['card'] = array(

    '#markup' => '
      <div class="control-group">
        <label for="card-number">Card Number</label>
        <div id="card-number" style="height: 20px" class="braintree-hosted-field control-group required"></div>
      </div>
      <div class="control-group">
        <label for="expiration-date">Expiration Date (MM/YY)</label>
        <div id="expiration-date" style="height: 20px" class="braintree-hosted-field control-group required"></div>
      <div class="control-group">
        <label for="cvv">CVV</label>
        <div id="cvv" style="height: 20px" class="braintree-hosted-field control-group required"></div>
      </div>'
  );
  $form['js'] =  array(
    '#markup' => '<script src="https://js.braintreegateway.com/js/braintree-2.26.0.min.js"></script>
    <script>
      braintree["errorStatus"] = false;

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
          },
          onFieldEvent: function (event) {
            if (event.type === "blur") {
              // console.log(event.isValid);
              if (event.isValid) {
                jQuery(event.target.container).removeClass("error").addClass("success");
              }
              else {
                jQuery(event.target.container).removeClass("success").addClass("error");
                jQuery(braintree).triggerHandler("baz");
              }
            } else if (event.type === "fieldStateChange") {
              if (event.card) {
                console.log(event.card.type);
              }
            }

          }
        },
        onError: function (event) {
          console.log("setting error status");
          braintree["errorStatus"] = true;
          console.log(braintree["errorStatus"]);
        }
      });
    </script>'
  );
  return $form;
}

/**
 * Callback function, charge gateway.
 */
function commerce_braintree_web_fundraiser_commerce_charge($method_instance, $donation) {
  // Translate from donation settings to the correct values for the plugin.
  // module_load_include('inc', 'fundraiser_commerce', 'includes/fundraiser_commerce.credit_card');
  $order = commerce_order_load($donation->did);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $charge = $wrapper->commerce_order_total->value();

  $pane_form = array(); // Not actually used anywhere in this system, so ignore it.

  // @TODO this function won't work b/c it is looking at CC field values that
  // do not exist with hosted fields.
  // $pane_values = _fundraiser_commerce_credit_card_pane_values($donation);
    $pane_values = array();

  // This only does stuff when commerce_cardonfile is enabled.
  // _fundraiser_commerce_submit_handler_pane_values($pane_values, $donation);

  $result = commerce_braintree_web_submit_form_submit($method_instance, $pane_form, $pane_values, $order, $charge);
  $success = FALSE;
  if ($result !== FALSE) {
    $success = TRUE;
  }
  // Perform post processing functions.
  _fundraiser_commerce_charge_submit_form_process($success, $method_instance, $pane_values, $donation);
  return $success;

}
