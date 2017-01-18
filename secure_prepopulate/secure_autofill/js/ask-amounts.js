

(function($, Drupal)
{
  /**
  * Drupal behavior.
  */
  Drupal.behaviors.secureAutofillAskAmounts = {
    attach:function()
    {
      // Any code here will be run on page load:
      // get GS
      var gs = getParameterByName('gs');
      if (typeof(gs) !== 'undefined') {
        // Post GS to 'get_amounts' callback, assemble request params and respond.
        $.post( "/js/secure_autofill/get_amounts", {
          js_callback: "get_amounts",
          js_module: "secure_autofill",
          gs: gs
        }).done(function(data) {
          // If response code is successful, take action.
          if (data.response.code == 200) {
            // TODO remove
            console.log(data.content);
            // Parse amounts and defaults into their appropriate fields.
            // format like:
            // amounts: "11|22|33"
            // default: "11"
            // recurring_amounts: "17|19|59"
            // recurring_default: "17"
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
                console.log(lastAmount);
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
                console.log(lastAmount);
                $('#edit-submitted-donation-recurring-amount .form-item').last().before(lastAmount);
                // Show the new amount.
                $(lastAmount).show();
              }
            }
            // Set recurring default.
            // TODO this needs to work on change to recurring.
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
          }
        });
      }
      // TODO add submit handler to rewrite selected amounts to 'other' field.


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
    }
  };
}(jQuery, Drupal));