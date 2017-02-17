(function($) {
  Drupal.behaviors.zipLookup = {
    attach: function(context, settings) {
      $('form.webform-client-form').each(function(i, v) {
        configUpdateCityAndStateByZip("#webform-component-billing-information--zip", "#webform-component-billing-information--city", "#webform-component-billing-information--state", "#webform-component-billing-information--country", v);
      });

      function configUpdateCityAndStateByZip(zipId, cityId, stateId, countryId, v) {
        var zipObj = $(zipId + " input", v);
        var cityObj = $(cityId + " input", v);
        var stateObj = $(stateId + " select", v);
        var countryObj = $(countryId + " select", v);
        // Make sure only 1 form field matches each of the ID parameters supplied.
        if (zipObj.length == 1 && cityObj.length == 1 && stateObj.length == 1 && countryObj.length == 1) {
          zipObj.blur(function() {
            // Only make the ajax request if we've got a valid 5-digit zip code, US is the selected country, and either the city or state field are empty.
            if ((/^\d{5}$/.test($(this).val())) && countryObj.val() == 'US' && (cityObj.val().trim() === "" || stateObj.val().trim() === "")) {
              $.ajax({
                url: "/zip_lookup/autopop-city-state-by-zip",
                dataType: "json",
                type: "POST",
                data: {
                  zip: $(this).val()
                }
              }).done(function(data) {
                if (cityObj.val().trim() == "") {
                  cityObj.val(data.city);
                }
                if (stateObj.val().trim() == "") {
                  stateObj.val(data.state_id);

                  // Overwrite the HTML of the span used to display the select box value on responsive forms
                  var targetOption = stateObj.find("option[value=" + data.state_id + "]");
                  stateObj.next().filter(".custom-select").find("span > span").html(targetOption.html());
                }
              });
            }
          });
        }
      }
    },

  };

})(jQuery);