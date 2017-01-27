(function($, Drupal)
{
  /**
  * Drupal behavior.
  */
  Drupal.behaviors.secureAutofillAskAmounts = {
    attach:function()
    {
      // Check for js cookie first, if it's in place then use it rather than GS.
      // Cookie is based on nid.
      var nid = getCurrentNodeId();
      if (nid) {
        var gsCookie = $.cookie("gs-" + nid);
      }
      if (!gsCookie) {
        // Get GS from URL if it wasn't in our cookie.
        var gs = getParameterByName('gs');
      }
      else {
        // TODO show welcome message with link to 'not me'
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
                // Set amount label.
                // TODO use appropriate currency.
                $(lastAmount).children('label').text('$' + amounts[i]);
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
                // Set amount label.
                // TODO use appropriate currency.
                $(lastAmount).children('label').text('$' + recurring_amounts[i]);
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