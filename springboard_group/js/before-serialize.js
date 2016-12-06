/**
 * @file
 *
 * Remove the token selector table IDs from ajax requests!
 *
 * @see wysiwyg.js for similar example.
 */
(function($) {

  var origBeforeSerialize = (Drupal.ajax ? Drupal.ajax.prototype.beforeSerialize : false);
  if (origBeforeSerialize) {
    Drupal.ajax.prototype.beforeSerialize = function (element, options) {
      var ret = origBeforeSerialize.call(this, element, options);
      var excludeSelectors = [];
      $.each(['[id^="token-"]'], function () {
        excludeSelectors = excludeSelectors.concat(this);
      });
      options.data['ajax_html_ids[]'] = [];
      $('[id]:not(' + excludeSelectors.join(',') + ')').each(function () {
        options.data['ajax_html_ids[]'].push(this.id);
      });
      return ret;
    };
  }
})(jQuery);
