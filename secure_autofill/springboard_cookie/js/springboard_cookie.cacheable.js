(function($) {
  Drupal.behaviors.springboard_cookie = {
    attach: function(context, settings) {
      // Is the Springboard cookie set on this client?
      var cookie = $.cookie(Drupal.settings.springboard_cookie.name);
      if (!cookie) {
        // No cookie, so make sure cookies are enabled.
        $.cookie('cookies_enabled', '1', { path: '/' });
        if ($.cookie('cookies_enabled') == '1') {
          // Call out to the server for a cookie.
          $.ajax({
            url: '/springboard_cookie/js/new',
            success: function(data, status) {
              // Save the cookie to this client.
              $.cookie(
                Drupal.settings.springboard_cookie.name,
                data.cookie,
                { expires: parseInt(data.expires), path: data.path }
              );
            }
          });
        }
      }
    }
  };
})(jQuery);