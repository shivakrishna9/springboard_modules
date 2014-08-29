(function ($) {
  Drupal.behaviors.m2t = {
    attach: function (context, settings) {

      var addressFields = 'input[name$="address]"], input[name*="city"], select[name*="state"], input[name*="zip"]';

      $('form.geocode-form').on('blur', addressFields, function(){
        parseAddressFields();
      });

      $('form.geocode-form').on('change', 'select[name*="state"]', function(){
        parseAddressFields();
      });

      function parseAddressFields() {
        var address = '';
        $(addressFields,'form.geocode-form').each(function(){
           if(this.value === '') {
            return false;
           }
           else {
            address += this.value + ', ';
           }
           if(this.name == 'submitted[sbp_zip]' && this.value !== '')  {
             lookupGeo(address);
           }
        });
      }

      function lookupGeo(address) {
        activeGeocoder = Drupal.settings.springboard_m2t.geocoder;
        if (activeGeocoder == 'googlej') {
          var geocoder = new google.maps.Geocoder();
          geocoder.geocode({'address': address}, function (results, status) {
            if (status == google.maps.GeocoderStatus.OK) {
              var latitude = results[0].geometry.location.lat();
              var longitude = results[0].geometry.location.lng();
              var url =  '/sunlightlookup';
              data = {coords: [latitude, longitude]};
            }
          });
        }
        else {
          data = {onelineaddress: address};
          if (activeGeocoder == 'census') {
            url = '/censuslookup';
          }
          else if (activeGeocoder == 'geocodio') {
            url = '/geocodiolookup';
            console.log(address);
          }
        }
        $.ajax({
          type: "POST",
          url: url,
          data: data,
          dataType: 'json',
          success: function(data) {
            console.log(data);
          },

          error: function(xhr, textStatus, error){
            console.log(error);
          }
        });
      }
    }
  };
})(jQuery);