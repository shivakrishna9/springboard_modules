(function ($) {
    /**
     * Performs a cached and delayed search.
     */
    Drupal.ACDB.prototype.search = function (searchString) {
        var db = this;
        this.searchString = searchString;

        if(db.uri.indexOf('message-action') != -1) {
            //chamberArr = [];
            //var checkBoxes = $('#edit-search-committee-chamber-wrapper input[type="checkbox"]');
            //checkBoxes.each(function () {
            //    if ($(this).prop('checked')) {
            //        chamberArr.push(this.value)
            //    }
            //});
            //
            //var values = chamberArr.join('-');

            chamber = $('#edit-search-committee-chamber-wrapper select').val();

            searchString = searchString + "/" + $("#edit-search-state").val()  + "/" + chamber
            this.searchString = searchString;
        }

        // See if this string needs to be searched for anyway.
        searchString = searchString.replace(/^\s+|\s+$/, '');
        if (searchString.length <= 0 ||
            searchString.charAt(searchString.length - 1) == ',') {
            return;
        }

        // See if this key has been searched for before.
        if (this.cache[searchString]) {
            return this.owner.found(this.cache[searchString]);
        }
        // Initiate delayed search.
        if (this.timer) {
            clearTimeout(this.timer);
        }
        this.timer = setTimeout(function () {
            db.owner.setStatus('begin');

            // Ajax GET request for autocompletion. We use Drupal.encodePath instead of
            // encodeURIComponent to allow autocomplete search terms to contain slashes.
            $.ajax({
                type: 'GET',
                url: db.uri + '/' + Drupal.encodePath(searchString),
                dataType: 'json',
                success: function (matches) {
                    if (typeof matches.status == 'undefined' || matches.status != 0) {
                        db.cache[searchString] = matches;
                        // Verify if these are still the matches the user wants to see.

                        if (db.searchString == searchString) {

                            db.owner.found(matches);
                        }
                        db.owner.setStatus('found');
                    }
                },
                error: function (xmlhttp) {
                    alert(Drupal.ajaxError(xmlhttp, db.uri));
                }
            });
        }, this.delay);
    };


})(jQuery);