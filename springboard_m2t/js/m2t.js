(function ($) {
  Drupal.behaviors.m2t = {
    attach: function (context, settings) {

    $("form.geocode-form").submit(function() {
      var address = '';
      var geocoder = new google.maps.Geocoder();
      geocoder.geocode({'address': address}, function (results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        var latitude = results[0].geometry.location.lat();
        var longitude = results[0].geometry.location.lng();
      } else {
        //something
      }
    });
    return false;
  });
  }
};
})(jQuery);