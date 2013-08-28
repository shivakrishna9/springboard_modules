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
        var sendData = $(this).serialize();
        path = $(this).attr('action');
        // Update the block message 
        $('#message-modal').hide().html('<h1>Processing your sustainer gift... </h1>').fadeIn(750);
        // Post the form
        req = $.ajax({
          type: 'post',
          url: path,
          cache: false,
          data: sendData,
          dataType: 'json',
        });
        // Display the thank you messagfe if successful
        req.done(function( data, textStatus, jqXHR ) {
          $('#message-modal').hide().html(data).fadeIn(750);
        });
        // Log the error if there is a problem
        req.fail(function( jqXHR, textStatus, errorThrown ) {
          console.log(errorThrown);
        });
        // Unblock the UI no matter what happens
        req.always(function( jqXHR, textStatus ) {
          setTimeout(function() {
            $.unblockUI();
          }, 3000);
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