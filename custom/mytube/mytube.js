(function ($) {

Drupal.behaviors.mytube = {
  attach: function (context, settings) {
    $('.mytubetrigger').once('mytubetrigger').click(function(){
      $(this).hide();
      $(this).after(unescape($('.mytubeembedcode', this).html()));
      Drupal.attachBehaviors(this);

      // If API usage is enabled, instantiate the API.
      if (Drupal.settings.mytube.enable_js_api === 1) {
        Drupal.behaviors.mytube.InitiateYouTubeAPI();
      }

    });
  }
};

/**
 * If API usage is enabled, initalize the player once the API is ready.
 */
Drupal.behaviors.mytube.InitiateYouTubeAPI = function(context) {
  if (typeof this.initialized === 'undefined') {
    this.initialized = false;
  }
  if (!this.initialized) {
    // Load the iFrame Player API code asynchronously.
    var tag = document.createElement('script');
    tag.src = "//www.youtube.com/iframe_api";
    var firstScriptTag = document.getElementsByTagName('script')[0];
    firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);
  }
};

})(jQuery);
