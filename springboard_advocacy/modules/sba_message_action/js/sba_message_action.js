/**
 * @file
 * message textarea sizing.
 */
(function ($) {

    $(document).ready(function () {
        $('#your-information-wrapper').append($('.form-layouts.one-column #edit-actions'));

        setColumnHeight();
        var sizer;
        $(window).resize(function() {
            clearTimeout(sizer);
            sizer = setTimeout(function() {
                setColumnHeight();
            }, 200);
        })
    });

    function setColumnHeight(){
        var formHeight = $('#user-information-wrapper').height();
        var textColumn = $('textarea[id*="edit-sba-messages-message"]').parents('.message-column');
        var formColumn = $('#user-information-wrapper').parents('.message-column');
        if (textColumn.attr('id') != formColumn.attr('id')) {
            var fHeight = formColumn.height();
            var tHeight = textColumn.height();
            if (formColumn.css('float') == 'none') {
                $('textarea[id*="edit-sba-messages-message"]').css('min-height', '230px');
                return false;
            }
            if (formHeight > tHeight) {
                var areaHeight = $('textarea[id*="edit-sba-messages-message"]').height();
                var plus = fHeight - tHeight;
                var newHeight = plus + areaHeight;
                $('textarea[id*="edit-sba-messages-message"]').css('min-height', newHeight);
            }
        }
    }
})(jQuery);