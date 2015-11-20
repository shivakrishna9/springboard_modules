(function ($) {
    Drupal.behaviors.multstepForm = {
        attach: function (context, settings) {
            var editSubmit = $('#sba-social-action-preview-form').find('#edit-submit');
            var send = $('#multiflow-send-now');

            editSubmit.once(function() {
                editSubmit.click(function() {
                    submitOnce(editSubmit, send, this);
                });
            });

            send.once(function() {
                send.click(function() {
                    submitOnce(editSubmit, send, this);
                    $('#sba-social-action-preview-form').submit();
                    return false;
                });
            });

            $(document).ready(function() {

                $text = $('div[id*="edit-messages"]').find('textarea');
                var navKeys = [33,34,35,36,37,38,39,40];
                $text.each(function(){
                    $(this).once(function() {

                        var handleCount = $(this).closest('.control-group').siblings('.editable-message-preview').find('.preview-target-text').text().length + 1;

                        $('#' + this.id).simplyCountable({
                            counter:            $(this).siblings('.description').find('.counter'),
                            countType:          'characters',
                            maxCount:           140 - handleCount,
                            strictMax:          true,
                            countDirection:     'down',
                            safeClass:          'safe',
                            overClass:          'over',
                            thousandSeparator:  ',',
                            onOverCount:        function(count, countable, counter){},
                            onSafeCount:        function(count, countable, counter){
                            },
                            onMaxCount:         function(count, countable, counter){}
                        });

                        var screenName = $(this).closest('.control-group').siblings('.editable-message-preview').find('.preview-target-text').text();

                        var newText;
                        var defaultText = screenName + ' ' +  $(this).val();
                        $(this).closest('.control-group').siblings('.editable-message-preview').text(screenName + ' ' +  $(this).val());

                        $(this).on('keyup blur paste', function(e) {
                            switch(e.type) {
                                case 'keyup':
                                    if ($.inArray(e.which, navKeys) < 0) {
                                        replacer(screenName, $(this));
                                    }
                                    break;
                                case 'paste':
                                    setTimeout(replacer(screenName, $(this)), (e.type === 'paste' ? 5 : 0));
                                    break;
                                default:
                                    replacer(screenName, $(this));
                                    break;
                            }
                        });
                    })
                })
            });

        } // attach.function
    } // drupal.behaviors


    var replacer = function(screenName, element) {
        newText = screenName + ' ' + element.val().replace(/\r?\n/g, '<br>');
        element.closest('.control-group').siblings('.editable-message-preview').html(newText);
    };

    $(document).ready(function () {

        $('textarea').each(function (e) {
            $(this).height(this.scrollHeight)
        });
        $('label').has('.form-required').addClass('required');

        $('.twitter-sign').click(function(){
            $.oauthpopup({
                path: '/sba/twitter/login',
                callback: function(oauthWindow){
                    if(typeof oauthWindow != 'undefined') {
                        $('#edit-ajaxify').trigger('mousedown');
                    }
                }
            });
            return false;
        });
    });

    function submitOnce(editSubmit, send, clicked) {

        if($('#sba-social-action-preview-form')[0].checkValidity) {
            if($('#sba-social-action-preview-form')[0].checkValidity()) {
                editSubmit.hide();
                var link = send.replaceWith('<span id="multiflow-send-now-processed">send now</span>');
                if (clicked.id == 'multiflow-send-now') {
                    rotate();
                }
                editSubmit.after('<div class="webform-user-wrapper">' +
                  '<p class="webform-user-thank-you">Thank you, your submission is processing.</p>' +
                  '<div class="webform-user-spinner"></div>' +
                  '</div>');
            }
        }
        else {
            editSubmit.hide();
            var link = send.replaceWith('<span id="multiflow-send-now-processed">send now</span>');
            if (clicked.id == 'multiflow-send-now') {
                rotate();
            }
            editSubmit.after('<div class="webform-user-wrapper">' +
              '<p class="webform-user-thank-you">Thank you, your submission is processing.</p>' +
              '<div class="webform-user-spinner"></div>' +
              '</div>');
        }
    }
    function rotate(num) {
        num = typeof num !== 'undefined' ? num : 10;
        $('#multiflow-send-now-wrapper').css({ 'transform': 'rotate(' + num + 'deg)'});
        var spinner = setTimeout(function() { rotate(num+10); }, 10);
        if (num >= 360) {
            clearTimeout(spinner);
        }
    }

})(jQuery);