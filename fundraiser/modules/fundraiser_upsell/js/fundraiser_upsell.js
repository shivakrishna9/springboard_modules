(function ($) {
  Drupal.behaviors.fundraiser_upsell = {
    attach: function(context, settings) {
      // Get the modal window size
      modalWidth = $('#message-modal').width();
      modalHeight = $('#message-modal').height();
      // Pop up the modal on the page load
      $.blockUI({ 
        message: $('#message-modal'),
        centerY : 0,
        css: {
          width: modalWidth,
          height: modalHeight,
          textAlign: 'left',
          padding: '1em',
          top: '10em'
        }
      });
      // Submit live form
      $('.live #fundraiser-upsell-donation-form').submit(function(e) {
        // Stop the default form submit
        e.preventDefault();
        // Grab the values
        // @TODO: change this to POST data to tokenized string for XSS security
        var sendData = $(this).serialize();
        amount = $('#fundraiser-upsell-donation-form #edit-amount').val();
        orderid = $('#fundraiser-upsell-donation-form #order-id').val();
        path = '/upsell/' + orderid + '/' + amount;
        // update the block message 
        $.blockUI({ 
          message: "<h1>Processing your sustainer gift... </h1>",
          centerY : 0,
          css: {
            width: modalWidth,
            height: modalHeight,
            textAlign: 'left',
            padding: '1em',
            top: '10em'
          }
        });
        $.ajax({ 
          url: path,
          cache: false,
          complete: function() {
            // @TODO: Have this replace the modal with the thank you message
            // unblock when remote call returns
            $.unblockUI();
          }
        });
      });
      // Preview submit function
      $('.preview #edit-submit').click(function() {
        // update the block message 
        $.blockUI({ 
          message: $('#message-return'),
          centerY : 0,
          timeout: 3000,
          css: {
            width: modalWidth,
            height: modalHeight,
            textAlign: 'left',
            padding: '1em',
            top: '10em'
          }
        });
        return false;
      });
      // Close button
      $('#edit-close').click(function() {
        $.unblockUI();
        return false;
      });
    }
  };
})(jQuery);