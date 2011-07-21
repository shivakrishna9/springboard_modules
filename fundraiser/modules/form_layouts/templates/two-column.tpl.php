<?php
/**
 * @file
 * This renders a donation form in 2 columns.
 *
 * The entire form is available in the variable $form. This can be broken down and rendered as individual fields if needed.
 * print drupal_render($form['submitted']); - prints the entire form as-is.
 *
 * Variables
 * -----------------------------------------------
 * $form['submitted'] - The entire raw form. Break out individual fields as needed.
 * $donation_fieldset - The rendered donation fields.
 * $billing_address_fieldset - The rendered billing address fields.
 * $donor_information_fieldset - The rendered donor information fields.
 * $credit_card_fielset - The rendered credit card fields.
 *
 * Always print out the entire $form. This renders the remaining pieces of the
 * form that haven't yet been rendered above.
 * print drupal_render($form);
 *
 */
?>

<div id="donation-form-column-wrapper">
  <div id="left">
    <? print $donation_fieldset; ?>
    <? print $donor_information_fieldset; ?>
    <? print $billing_address_fieldset; ?>
  </div>
  <div id="right">
    <? print $credit_card_fieldset; ?>
  </div>
  <div id="donation-form-footer">
    <? print drupal_render($form); ?>
  </div>
</div>