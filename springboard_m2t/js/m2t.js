(function ($) {
  Drupal.behaviors.m2t = {
    attach: function (context, settings) {

      var addressFields = 'input[name$="address]"], input[name*="city"], select[name*="state"], input[name*="zip"]';
      var address = '';

      $('form.geocode-form').on('blur', addressFields, function(){
        parseAddresFields();
      });

      $('form.geocode-form').on('change', 'select[name*="state"]', function(){
        parseAddresFields();
      });

      function parseAddresFields() {
        $(addressFields,'form.geocode-form').each(function(){
           if(this.value === '') {
            return false;
           }
           else {
            address += this.value + ', ';
           }
           if(this.name == 'submitted[sbp_zip]' && this.value !== '') {
             lookupGeo();
           }
        });
      }

      function lookupGeo() {
        var geocoder = new google.maps.Geocoder();
        geocoder.geocode({'address': address}, function (results, status) {
          if (status == google.maps.GeocoderStatus.OK) {
            var latitude = results[0].geometry.location.lat();
            var longitude = results[0].geometry.location.lng();
            console.log(latitude + ' ' + longitude);


            $.ajax({
              type: "POST",
              url: '/sunlightlookup',
              data: {coords: [latitude, longitude]},
              dataType: 'json',
              success: function(data) {
                console.log(data);
              },

              error: function(xhr, textStatus, error){
                console.log(error);
              }
            });




          } else {
            //something
          }
        });
        return false;
      }

    }
  };
})(jQuery);