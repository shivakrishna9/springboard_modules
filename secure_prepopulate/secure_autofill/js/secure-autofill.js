(function($, Drupal)
{
  /**
  * Drupal behavior.
  */
  Drupal.behaviors.secureAutofill = {
    attach:function()
    {
      // Check for js cookie first, if it's in place then use it rather than GS.
      // Cookie is based on nid.
      var nid = getCurrentNodeId();
      var showWelcome = false;
      if (nid) {
        var gsCookie = $.cookie("gs-" + nid);
      }
      if (!gsCookie) {
        // Get GS from URL if it wasn't in our cookie.
        var gs = getParameterByName('gs');
      }
      else {
        // Flag to show welcome message with link to 'not me'.
        showWelcome = true;
        var gs = gsCookie;
      }

      // Decrypt whichever value we chose to use above.
      if (typeof(gs) !== 'undefined') {
        // Post GS to 'get_amounts' callback, assemble request params and respond.
        $.post( "/js/secure_autofill/get_amounts", {
          js_callback: "get_amounts",
          js_module: "secure_autofill",
          gs: gs
        }).done(function(data) {
          // If response code is successful, take action.
          if (data.response.code == 200) {
            // Parse amounts and defaults into their appropriate fields.
            if (typeof(data.content.amounts) !== 'undefined') {
              // Find and replace amount.
              // First, hide all regular options from the form.
              $('#edit-submitted-donation-amount .form-item').hide();
              // If the last amount's value is 'other', show it.
              if ($('#edit-submitted-donation-amount .form-item').last().children('input').val() == 'other') {
                $('#edit-submitted-donation-amount .form-item').last().show();
              }

              // Next, iterate and process amounts.
              var amounts = data.content.amounts.split("|");
              for (var i = 0, len = amounts.length; i < len; i++) {
                // Take the 'other' form item, alter its label and value, and insert before the 'other' form item.
                var lastAmount = $('#edit-submitted-donation-amount .form-item').last().clone(true);
                // Set amount value.
                $(lastAmount).children('input').val(amounts[i]);
                // Set amount label, use appropriate currency.
                if (Drupal.settings.fundraiser.currency.symbol) {
                  var symbol = Drupal.settings.fundraiser.currency.symbol;
                }
                else {
                  var symbol = "$";
                }
                $(lastAmount).children('label').text(symbol + amounts[i]);
                // Set element ID.
                lastAmount.attr('id', "edit-submitted-donation-amount-" + i);
                // Insert before last amount.
                $('#edit-submitted-donation-amount .form-item').last().before(lastAmount);
                // Show the new amount.
                $(lastAmount).show();
              }
            }

            // Set 'default' amount.
            if (typeof(data.content.default) !== 'undefined') {
              // Get the default amount as returned and attempt to set it.
              var amounts = $('#edit-submitted-donation-amount .form-item input:visible');
              // Iterate through amounts, set value if possible.
              for (var i = 0, len = amounts.length; i < len; i++) {
                if (amounts[i].value == data.content.default) {
                  // Uncheck all other values first.
                  $('#edit-submitted-donation-amount .form-item input').attr('checked', false);
                  // Now set ours.
                  $(amounts[i]).attr('checked', 'checked');
                  // No need to continue.
                  break;
                }
              }
            }

            // Set recurring ask amounts.
            if (typeof(data.content.recurring_amounts) !== 'undefined') {
              // Find and replace amount.
              // First, hide all regular options from the form.
              $('#edit-submitted-donation-recurring-amount .form-item').hide();
              // If the last amount's value is 'other', show it.
              if ($('#edit-submitted-donation-recurring-amount .form-item').last().children('input').val() == 'other') {
                $('#edit-submitted-donation-recurring-amount .form-item').last().show();
              }
              // Next, iterate and process recurring_amounts.
              var recurring_amounts = data.content.recurring_amounts.split("|");
              for (var i = 0, len = recurring_amounts.length; i < len; i++) {
                // Take the 'other' form item, alter its label and value, and insert before the 'other' form item.
                var lastAmount = $('#edit-submitted-donation-recurring-amount .form-item').last().clone(true);
                // Set amount value.
                $(lastAmount).children('input').val(recurring_amounts[i]);
                // Set amount label, use appropriate currency.
                if (Drupal.settings.fundraiser.currency.symbol) {
                  var symbol = Drupal.settings.fundraiser.currency.symbol;
                }
                else {
                  var symbol = "$";
                }
                $(lastAmount).children('label').text(symbol + recurring_amounts[i]);
                // Set element ID.
                lastAmount.attr('id', "edit-submitted-donation-recurring-amount-" + i);
                // Insert before last amount.
                $('#edit-submitted-donation-recurring-amount .form-item').last().before(lastAmount);
                // Show the new amount.
                $(lastAmount).show();
              }
            }

            // Set recurring default.
            if (typeof(data.content.recurring_default) !== 'undefined') {
              // Get the recurring_default amount as returned and attempt to set it.
              var amounts = $('#edit-submitted-donation-recurring-amount .form-item input');
              // Iterate through amounts, set value if possible.
              for (var i = 0, len = amounts.length; i < len; i++) {
                if (amounts[i].value == data.content.recurring_default) {
                  // Uncheck all other values first.
                  $('#edit-submitted-donation-recurring-amount .form-item input').attr('checked', false);
                  // Now set ours.
                  $(amounts[i]).attr('checked', 'checked');
                  // No need to continue.
                  break;
                }
              }
            }

            if (nid && !gsCookie) {
              // Set GS cookie if it wasn't before.
              // This cookie will expire at end of session.
              $.cookie("gs-" + nid, gs, { path: '/' });
            }

          // End data check.
          }
        // End response handler.
        });
      }

      // Handle secure prepopulate querystring.
      // TODO develop this functionality.
      // Check for js cookie first, if it's in place then use it rather than af.
      // Cookie is based on nid.
      if (nid) {
        var afCookie = $.cookie("af-" + nid);
      }
      if (!afCookie) {
        // Get af from URL if it wasn't in our cookie.
        var af = getParameterByName('af');
      }
      else {
        // Show welcome message with link to 'not me'
        showWelcome = true;
        var af = afCookie;
      }

      // Decrypt whichever value we chose to use above.
      if (typeof(af) !== 'undefined') {
        // Post af to 'get_amounts' callback, assemble request params and respond.
        $.post( "/js/secure_autofill/get_values", {
          js_callback: "get_values",
          js_module: "secure_autofill",
          af: af
        }).done(function(data) {
          // If response code is successful, take action.
          if (data.response.code == 200) {
            // Iterate through returned object and populate appropriate fields.
            for (var key in data.content) {
              if (data.content.hasOwnProperty(key)) {
                // Email is keyed as "email", but appears on donation forms as "mail".
                if (key === "email") {
                  $("input[name*='mail']").val(data.content[key]);
                }
                else {
                  // Populate input fields matching key with value returned from callback.
                  $("input[name*='" + key + "']").val(data.content[key]);
                  // Populate selects.
                  $("select[name*='" + key + "']").each(function(){
                    $('option[value="' + data.content[key] + '"]', this).prop('selected', true);
                  });
                  // TODO handle select fields, radios, and checkboxes.
                }
              }
            }

            if (nid && !afCookie) {
              // Set af cookie if it wasn't before.
              // This cookie will expire at end of session.
              $.cookie("af-" + nid, af, { path: '/' });
            }

          // End data check.
          }
        // End response handler.
        });
      }


      // Helper function to get url params.
      function getParameterByName(name, url) {
        if (!url) {
          url = window.location.href;
        }
        name = name.replace(/[\[\]]/g, "\\$&");
        var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
            results = regex.exec(url);
        if (!results) return null;
        if (!results[2]) return '';
        return decodeURIComponent(results[2].replace(/\+/g, " "));
      }

      // Helper function to get node id.
      function getCurrentNodeId() {
        var $body = $('body.page-node');
        if ( ! $body.length )
          return false;
        var bodyClasses = $body.attr('class').split(/\s+/);
        for ( i in bodyClasses ) {
          var c = bodyClasses[i];
          if ( c.length > 10 && c.substring(0, 10) === "page-node-" )
            return parseInt(c.substring(10), 10);
        }
        return false;
      }
    }
  };
}(jQuery, Drupal));