(function ($) {

    $(document).ready(function () {

        if ($.isFunction($.fn.uniform)) {
            $('select').each(function () {
                $.uniform.restore(this);
            });
        }
    });

    })(jQuery);