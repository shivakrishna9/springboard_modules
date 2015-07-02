/**
 * @file
 * message textarea equal-heights sizing on user-facing action webforms
 */
(function ($) {

    $(document).ready(function () {
        setColumnHeight();
        var sizer;
        $(window).resize(function(){
            clearTimeout(sizer);
            sizer = setTimeout(function() {
                setColumnHeight();
            }, 200);
        });

        //message confiramtions javascript.
        $('.sba-message-show-message').on('click', function() {
            var message = $(this).siblings('.sba-message-body');
            message.toggle('slow', function() {
                var link = $(this).siblings('.sba-message-show-message');
                if (message.is(":visible")) {
                    link.text('Hide message')
                }
                if (!message.is(":visible")) {
                    link.text('Show message')
                }
            });
        })
    });

    function setColumnHeight(){
        var userWrapper = $('#user-information-wrapper');
        var formHeight = userWrapper.height();
        var message = $('textarea[id*="edit-sba-messages-message"]');
        var textColumn = message.parents('.message-column');
        var formColumn = userWrapper.parents('.message-column');
        if (textColumn.attr('id') != formColumn.attr('id')) {
            var fHeight = formColumn.height();
            var tHeight = textColumn.height();
            if (formColumn.css('float') == 'none') {
                message.css('min-height', '230px');
                return false;
            }
            if (formHeight > tHeight) {
                var areaHeight = message.height();
                var plus = fHeight - tHeight;
                var newHeight = plus + areaHeight;
                message.css('min-height', newHeight);
            }
        }
    }
})(jQuery);