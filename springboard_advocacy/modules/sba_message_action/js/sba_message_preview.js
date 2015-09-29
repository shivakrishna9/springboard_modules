(function ($) {
    Drupal.behaviors.multstepForm = {
        attach: function (context, settings) {
            $('#sba-message-action-preview-form').find('#edit-submit').once(function() {
                $('#sba-message-action-preview-form').find('#edit-submit').click(function() {
                    var  button = $(this);
                    // Disable the button. This prevents double/triple mouse clicking.
                    if($('#sba-message-action-preview-form')[0].checkValidity) {
                        if($('#sba-message-action-preview-form')[0].checkValidity()) {
                            $(this).hide();
                            $(this).after('<div class="webform-user-wrapper">' +
                                '<p class="webform-user-thank-you">Thank you, your submission is processing.</p>' +
                                '<div class="webform-user-spinner"></div>' +
                                '</div>');
                        }
                    }
                    else {
                        button.hide();
                        button.after('<div class="webform-user-wrapper">' +
                            '<p class="webform-user-thank-you">Thank you, your submission is processing.</p>' +
                            '<div class="webform-user-spinner"></div>' +
                            '</div>');
                    }
                });
            });
        } // attach.function
    } // drupal.behaviors

    $(document).ready(function () {
        $('textarea').each(function (e) {
            $(this).height(this.scrollHeight)
        });
    });
})(jQuery);