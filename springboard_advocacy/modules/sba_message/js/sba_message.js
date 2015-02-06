/**
 * @file
 */
(function ($) {
    Drupal.behaviors.AdvocacyMessageRecipients = {
        attach: function(context, settings) {
            links = $('a.advocacy-add-target, a#advo-add-all');
            links.each(function(){
                $(this, context).once('advocacy-add-target', function() {
                    $(this).click(function (e){
                        e.preventDefault();
                        count = $('target-recipient').length;
                        var query = $(this).attr('href').replace('add-all?','').replace('add-target?','').split('&');
                        var readable = buildReadableQuery(query);
                        $('#springboard-advocacy-message-recipients-container')
                            .append('<div id = "target-' + count + '" class = "target-recipient" style="height: 40px">' + readable + '</div>');
                        $(query).each(function(index, value) {
                            value = value.split('=');
                            $('#target-' + count).attr('data-' + value[0], value[1]);

                        });
                        buildFormValue();
                    });
                });
            });
        }
    };

    function buildReadableQuery(query) {
        var queryObj = {};
        $(query).each(function(index, value) {
            var segments = value.split('=');
            segments[0] = segments[0].ucfirst();
            queryObj[segments[0]] = segments[1].split('%7C');
            $(queryObj[segments[0]]).each(function(i, v){
                switch(v) {
                    case 'FR':
                        queryObj[segments[0]][i] = 'Federal Representatives';
                        break;
                    case 'SR':
                        queryObj[segments[0]][i] = 'State Representatives';
                        break;
                    case 'FS':
                        queryObj[segments[0]][i] = 'Federal Senators';
                        break;
                    case 'SS':
                        queryObj[segments[0]][i] = 'State Senators';
                        break;
                    case 'SX':
                        queryObj[segments[0]][i] = 'State Executives';
                        break;
                    case 'FX':
                        queryObj[segments[0]][i] = 'Federal Executive';
                        break;
                    case 'R':
                        queryObj[segments[0]][i] = 'Republicans';
                        break;
                    case 'D':
                        queryObj[segments[0]][i] = 'Democrats';
                        break;
                    case '0':
                        queryObj[segments[0]][i] = 'Other';
                        break;

                    case 'M':
                        queryObj[segments[0]][i] = 'Male';
                        break;
                    case 'F':
                        queryObj[segments[0]][i] = 'Female';
                        break;
                }
            });
        });

        if (typeof(queryObj.id) !== 'undefined') {
            return "Individual: " + queryObj.sal + " " +  queryObj.first + " " + queryObj.last;
        }

        asString = JSON.stringify(queryObj);
        cleanUp = asString
            .replace(/{/g, '')
            .replace(/}/g, '')
            .replace(/"/g, '')
            .replace(/,/g, ', ')
            .replace(/:/g, ': ')
            .replace(/\[/g, '')
            .replace(/\]/g, '. ')
            .replace(/. ,/g, '. ');

        if (typeof(queryObj.gender) !== 'undefined' || typeof(queryObj.social) !== 'undefined' ||  typeof(queryObj.district) !== 'undefined') {
            return "Multilple Individuals: " +  cleanUp;
        }
        return "Group: " +  cleanUp;
    }

    function buildFormValue() {
        recipients =  $('.target-recipient');
    }
    String.prototype.ucfirst = function()
    {
        return this.charAt(0).toUpperCase() + this.substr(1);
    }

})(jQuery);