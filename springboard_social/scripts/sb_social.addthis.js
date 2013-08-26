var addthis_config = {
  pubid: ''
};

var addthis_share = {

};


(function ($) {
  $(document).ready(function() {
  Drupal.behaviors.sb_social = {
    attach: function (context, settings) {
      // AddThis requires some global configuration objects be populated
      // set up the AddThis account ID from admin settings
      window.addthis_config = {
        pubid: Drupal.settings.sb_social.pubid
      };
      // event listener logs share event
      if (typeof window.addthis !== "undefined") {
        window.addthis.addEventListener('addthis.menu.share', shareResponse);
      }
    }
  };
  });
})(jQuery);

function shareResponse(data) {
  $url = '/sb_social/share_event/' + data.data.service + '/' + Drupal.settings.sb_social.sid;

  jQuery.ajax({
       url: $url,
       type: "GET",
       success:function(response){
         console.log(response);
       }
  });
}
