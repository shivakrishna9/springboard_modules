/* javascript functions for sustainer management page */
Drupal.behaviors.fundraiserSustainer = {
  attach: function(context) { (function($) {

  $(document).ready(function() {
    // clear out the cancellation reason when it receives the focus for the first time
    $('#edit-reason').one('focus', function() {
      $(this).val('');
    });

    /* Disable submit buttons on the 3 main forms when submitting */
    $('#fundraiser-donation-amount-form').submit(function() {
      $('input[type=submit]', this).attr('disabled', 'disabled');
    });
    
    $('#fundraiser-billing-update-form').submit(function() {
      $('input[type=submit]', this).attr('disabled', 'disabled');
    });
    
    $('#fundraiser-cancel-form').submit(function() {
      $('input[type=submit]', this).attr('disabled', 'disabled');
    }); 
  });

  })(jQuery); }
} 
