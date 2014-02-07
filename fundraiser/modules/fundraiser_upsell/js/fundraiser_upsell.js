(function ($) {

Drupal.behaviors.fundraiser_upsell = {
    attach: function(context, settings) {
        // Get the modal window size
        var modal = $('#message-modal');
        modalWidth = modal.css('width');
        modalHeight = modal.css('height');
        // Get the days for the rejection cookie
        rejectionDays = Drupal.settings.fundraiser_upsell.rejectionDays;
        // Pop up the modal on the page load
        $.blockUI({
            message: modal,
            centerY : 0,
            css: {
                width: modalWidth,
                height: modalHeight,
                textAlign: 'left',
                padding: '1em',
                top: '10em'
            }
        });
        // Assign an ID to the blockUI message
        $('.blockUI.blockMsg').attr('id', 'upsell-modal');

        // Submit live form
        $('.live-modal #fundraiser-upsell-donation-form').submit(function(e) {
            // Stop the default form submit
            e.preventDefault();
            // Grab the values
            var sendData = $(this).serialize();
            path = $(this).attr('action');
            // Update the block message
            var modalMessage = Drupal.t('Processing your sustainer gift...');
            $('#message-modal').hide().html('<div class="message-body"><h1>' + modalMessage + '</h1></div>').fadeIn(750);
            var closeText = Drupal.t('Close');
            // Post the form and process
            $.ajax({
                type: 'post',
                url: path,
                cache: false,
                data: sendData,
                dataType: 'json',
                // Display the thank you message if successful
                success: function(response) {
                    if (response.showClose) {
                        $('#message-modal').append('<div id="modal-close">' + closeText + '</div>');
                        $('#modal-close').click(function(e) {
                            e.preventDefault();
                            $.unblockUI();
                        });
                    }
                    else {
                        $('#modal-close').remove();
                    }
                    $('.message-body', '#message-modal').hide().html(response.content).fadeIn(750);
                    //$('#message-modal').addClass('thank-you');
                },
                // Log the error if there is a problem
                error: function(msg) {
                    console.log(msg);
                }
            });
        });
        // Live close button
        $('.live-modal #edit-close').click(function() {
            var exdate=new Date();
            exdate.setDate(exdate.getDate() + rejectionDays);
            var c_value = "1;path=/" + ((rejectionDays==null) ? "" : ";expires="+exdate.toUTCString());
            document.cookie='fundraiser_upsell_rejection' + "=" + c_value;
            $.unblockUI();
            return false;
        });

    }
};

})(jQuery);
