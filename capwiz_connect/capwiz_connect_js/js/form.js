(function ($) {
  // jquery ext. for grabbing query string vars
  $.extend({
    getUrlVars: function(){
      var vars = [], hash;
      var hashes = window.location.href.slice(window.location.href.indexOf('?') + 1).split('&');
      for(var i = 0; i < hashes.length; i++)
      {
        hash = hashes[i].split('=');
        vars.push(hash[0]);
        vars[hash[0]] = hash[1];
      }
      return vars;
    },
    getUrlVar: function(name){
      return $.getUrlVars()[name];
    }
  });

  $(document).ready(function() {
    // Run the form script if we are on the alert page
    if (window.location.pathname.indexOf("alert") > -1) {
      var action = $('form[name=mailapp]').attr('action'),
        qstrings = $.getUrlVars(),
        af = $.getUrlVar('af');
        additional = '?';

      // Add the querystring vars, except for our encrypted values to the action
      $.each(qstrings, function(key, val) {
        if (val && val != 'af') {
          additional = additional  + val + '=' + $.getUrlVar(val) + '&';
        }
      });

      //console.log(additional);
      // Set the new the action string to the forms action attribute
      if (additional != '?') {
        $('form[name=mailapp]').attr('action', action + additional);
      }

      $.getJSON(capwizConnectJsDomain + "/capwiz_connect_js/decrypt.json?af=" + af + '&callback=?', function(data) {
        $.each(data.fieldValues, function(name, value) {
          if (value != '') {
            $(":input[name='" + name + "']").val(value);
          }
        });
      });
    }
  });
})(jQuery);