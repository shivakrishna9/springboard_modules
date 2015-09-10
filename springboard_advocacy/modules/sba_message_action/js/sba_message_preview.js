(function ($) {
    $(document).ready(function () {
        $('textarea').each(function (e) {
            $(this).height(this.scrollHeight)
        });
    });
})(jQuery);