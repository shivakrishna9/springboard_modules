<?php
/**
 * @file
 */
?>
<div id="orders">
 <?php print $orders; ?>
</div>
<?php if (!$cancelled) :?>

<div id="payment-info">
  <h2>Recurring Payment Info</h2>
  <dl>
    <dt>Recurring Donation Amount</dt>
    <dd>$<?php print money_format('%i', $payment_info['donation_amount']); ?></dd>
    <dt>Recurring Charges Processed</dt>
    <dd><?php print $payment_info['orders_processed']; ?></dd>
    <dt>Recurring Charges Remaining</dt>
    <dd><?php print $payment_info['orders_remaining']; ?></dd>
  </dl>
</div>
<div id="billing-info">
  <h2>Billing Address</h2>
  <address>
  <?php print $billing_info['billing_street']; ?><br />
  <?php print $billing_info['billing_city']; ?>, <?php print $billing_info['billing_state']; ?> <?php print $billing_info['billing_postal_code']; ?>
  </address>
</div>
<div id="donation-amount-form">
  <h2>Donation Amount</h2>
  <?php print $amount_form; ?>
</div>
<div id="donation-billing-form">
  <h2>Update Your Credit Card Information</h2>
  <?php print $billing_form; ?>
</div>
<div id="payment-schedule">
  <h2>Payment Schedule</h2>
  <?php print_r($payment_schedule); ?>
</div>
<?php if (!empty($cancel_form)): ?>
<div id="donation-cancel-form">
  <h2>Cancel Future Donations</h2>
  <?php print $cancel_form; ?>
</div>
<?php endif; ?>

<?php else: ?>

<div>The selected has been cancelled.</div>

<?php endif; ?>
 
