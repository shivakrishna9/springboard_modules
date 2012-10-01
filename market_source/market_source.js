/**
 * Store and parse market source data in cookies.
 */
Drupal.behaviors.marketSourceInit = {
  attach: function(context) { (function($) {

  $('body:not(.marketsource-processed)', context).each(function(i) {
    if (typeof Drupal.settings.market_source === 'undefined' ||
      typeof Drupal.settings.market_source.qs_keys === 'undefined') {
        return;
    }

    /**
     * Parse the raw querystring into an object.
     */
    var urlParams = {};
    (function () {
      var parsed,
          regex = /([^&=]+)=?([^&]*)/g,
          decode = function (value) {
            // Regex for replacing addition symbol with a space
            return decodeURIComponent(value.replace(/\+/g, " "));
          },
          querystring = window.location.search.substring(1);
      while (parsed = regex.exec(querystring)) {
        var keyname = new String(decode(parsed[1]));
        urlParams[keyname.toLowerCase()] = decode(parsed[2]);
      }
    })();

    /**
     * Sets a cookie.
     */
    var setCookie = function (name, value) {
      if (typeof $.cookie !== 'undefined') {
        name = 'market_source__' + name;
        if (typeof Drupal.settings.market_source.cookie_domain !== 'undefined') {
          $.cookie(name, value, { path: '/', domain: Drupal.settings.market_source.cookie_domain });
        }
        else {
          $.cookie(name, value, { path: '/'});
        }
      }
    }

    /**
     * Reads data from a cookie.
     */
    var getCookie = function (name) {
      var retval = false;
      if (typeof $.cookie !== 'undefined') {
        name = 'market_source__' + name;
        retval = $.cookie(name);
      }
      return retval;
    }

    /**
     * Check the querystring keys for existing cookie values.
     */
    var qs_keys = Drupal.settings.market_source.qs_keys;
    for (var key in qs_keys) {
      // Check the querystring for this key.
      if (typeof urlParams[key] !== 'undefined') {
        qs_keys[key] = urlParams[key];
        // Save this value as a cookie.
        setCookie(key, urlParams[key]);
      }
      if (qs_keys[key] == false || qs_keys[key] == null || qs_keys[key]  == '') {
        // Is there already a cookie set?
        var cookie = getCookie(key);
        if (cookie != false && cookie != null && cookie != '') {
          // Store the cookie's value in our qs_keys.
          qs_keys[key] = cookie;
        }
      }
    }

    /**
     * Handle referrers as a special case.
     */
    var referrer = document.referrer;
    if (referrer == '') {
      referrer = '(none)';
    }
    if (typeof qs_keys['initial_referrer'] !== 'undefined') {
      var cookie = getCookie('initial_referrer');
      if (cookie == false || cookie == null || cookie == '') {
        // Store the referrer value in our qs_keys.
        qs_keys['initial_referrer'] = referrer;
        // Set initial_referrer cookie.
        setCookie('initial_referrer', qs_keys['initial_referrer']);
      }
    }
    if (typeof qs_keys['referrer'] !== 'undefined') {
      // Store the referrer value in our qs_keys.
      qs_keys['referrer'] = referrer;
      // Set the referrer cookie for backwards compat.
      setCookie('referrer', qs_keys['referrer']);
    }

  }).addClass('marketsource-processed');

  })(jQuery); }
}

/**
 * Populate the market source fields in this page's webforms.
 */
Drupal.behaviors.marketSourceFormPopulate = {
  attach: function(context) { (function($) {

  if (typeof Drupal.settings.market_source === 'undefined' ||
    typeof Drupal.settings.market_source.form_keys === 'undefined' ||
    typeof Drupal.settings.market_source.qs_keys === 'undefined') {
      return;
  }

  // Set a maximum length for each key's value.
  var maxlength = 255;
  if (typeof Drupal.settings.market_source.maxlength !== 'undefined') {
    maxlength = Drupal.settings.market_source.maxlength;
  }

  var qs_keys = Drupal.settings.market_source.qs_keys;
  // Iterate across all webforms on the page.
  for (var form_id in Drupal.settings.market_source.form_keys) {
    // Iterate across all form keys in this webform.
    var form_keys = Drupal.settings.market_source.form_keys[form_id];
    for (var key in form_keys) {
      // Is this key set in qs_keys object?
      if (typeof qs_keys[key] !== 'undefined') {
        var value = qs_keys[key];
        if (typeof value === 'string') {
          value = value.substring(0, maxlength);
        }
        // Handle referrers as a special case.
        if (key == 'initial_referrer' || key == 'referrer') {
          if (value == '(none)') {
            value = null;
          }
        }
        // If the value is not null, set the value.
        if (value != null) {
          var selector = 'form#' + form_id + ' #' + form_keys[key] + ':not(.marketsource-processed)';
          // Set the value.
          $(selector, context)
            .val(qs_keys[key])
            .addClass('marketsource-processed');
        }
      }
    }
  }

  })(jQuery); }
}
