/**
 * @file
 * Handles Views' exposed form AJAX data submission.
 * see https://www.drupal.org/node/1183418
 */
(function ($) {

    $(document).ready(function () {
        var formHeight = $('#user-information-wrapper').height();
        var textColumn = $('textarea[id*="edit-sba-messages-message"]').parents('.message-column');
        var formColumn = $('#user-information-wrapper').parents('.message-column');
        if (textColumn.attr('id') != formColumn.attr('id')) {
            var fHeight = formColumn.height();
            var tHeight = textColumn.height();
            if (formHeight > tHeight) {
                var areaHeight = $('textarea[id*="edit-sba-messages-message"]').height();
                var plus = fHeight - tHeight;
                var newHeight = plus + areaHeight;
                $('textarea[id*="edit-sba-messages-message"]').css('min-height', newHeight);
            }
        }

  });

})(jQuery);