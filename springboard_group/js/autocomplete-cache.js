(function($) {
  // Remove caching on the entity reference autocomplete selector.
  Drupal.ACDB.prototype.orig = Drupal.ACDB.prototype.search;
  Drupal.ACDB.prototype.search =
    function (searchString) {
      var id = this.owner.input.id;
      if ($(id.indexOf('some_id_to_reference')) !== -1) {
        this.cache = {};
      }
      this.orig(searchString);
    };
})(jQuery);