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
 * $form['submitted']['donation'] - The rendered donation fields.
 * $form['submitted']['billing_information'] - The rendered billing address fields.
 * $form['submitted']['donor_information'] - The rendered donor information fields.
 * $form['submitted']['credit_card_information'] - The rendered credit card fields.
 *
 * Always print out the entire $form. This renders the remaining pieces of the
 * form that haven't yet been rendered above.
 * print drupal_render($form);
 *
 */
?>

<div id="donation-form-column-wrapper">
  <div id="left">
    <?php print drupal_render($form['submitted']['donation']); ?>
    <?php print drupal_render($form['submitted']['donor_information']); ?>
    <?php print drupal_render($form['submitted']['billing_information']); ?>
  </div>
  <div id="right">
    <?php print drupal_render($form['submitted']['credit_card_information']); ?>
  </div>
  <div id="donation-form-footer">
    <?php print drupal_render($form); ?>
  </div>
</div>
