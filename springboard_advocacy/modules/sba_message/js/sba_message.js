/**
 * @file
 */

/**
 * Advocacy recipients list  and search page
 */
(function ($) {

    Drupal.behaviors.AdvocacyMessageRecipients = {
        attach: function(context, settings) {

            // Apply click event to the search form add links
            // Allows views search results to be appended to the recipients list
            links = $('a.advocacy-add-target, a#advo-add-all');
            links.each(function() {
                $(this, context).once('advocacy-add-target', function() {
                    $(this).click(function (e){
                        e.preventDefault();
                        addTargets('search', this);
                    });
                });
            });

            //update exposed form element states when a district is selected
            $('select[name="search_district_name"]', context).once('advocacy-district-reloaded', function() {
                elementStates(this);
                $('#views-exposed-form-targets-block-3 input, #views-exposed-form-targets-block-3 select').on('change', function(){
                    if(this.type != 'button' && this.type !='hidden') {
                        elementStates(this);
                    }
                });
            });

            //update exposed form element states when the text search field is changed
            var combine, oldVal;
            $('#edit-combine').on('keyup', function() {
                clearTimeout(combine);
                var newVal = $(this).val();
                if (oldVal != newVal) {
                    combine = setTimeout(function() {
                        oldVal = newVal;
                        elementStates(this);
                    }, 400);
                }
            });

            //set the click event for the quick target button
            $('#quick-target', context).once('advocacy-add-quick-target', function() {
                elementStates(this);//need this?
                $('input#quick-target').click(function() {
                    prepareQuickTarget();
                });
            });
        }
    };

    // selectively disable/enable exposed form elements based on user actions
    function elementStates(item) {

        //element state meta-variables
        var notGroupable = false;
        var groupable = false;
        var hasDistrict = false;
        var hasState = false;
        //elements
        var combine = $('input#edit-combine');
        var district = $('select[name="search_district_name"]');
        var state = $('select[name="search_state"]');
        var allBoxes = $('#views-exposed-form-targets-block-3 input[type="checkbox"]');
        var allInputs =$('#views-exposed-form-targets-block-3 input[type="checkbox"], #views-exposed-form-targets-block-3 input[type="text"], #views-exposed-form-targets-block-3 select');

        allInputs.each(function() {
            //update our element state meta-variables
            var nm = this.name;
            if(nm.indexOf('combine') != -1 || nm.indexOf('gender') != -1 ||
               nm.indexOf('social')  != -1 || nm.indexOf('district') != -1) {
               if ($(this).prop('checked') || (nm.indexOf('district') != -1 && district.val() != 'All')) {
                   notGroupable = true;
                   if (nm.indexOf('district') != -1) {
                       hasDistrict = true;
                   }
               }
               if(nm.indexOf('district') != -1 && district.val() == 'All') {
                   hasDistrict = false;
               }

               if( nm.indexOf('combine') != -1) {
                  if ($(this).val().length > 0) {
                      notGroupable = true;
                  }
              }
           } else {
               if ((this.type=='checkbox' && $(this).prop('checked'))) {
                   groupable = true;
               }
               if((nm == 'search_state' && $(this).val() != 'All')) {
                   hasState = true;
               }
           }
        });

        //update form element states based on meta-variables
        if(hasDistrict == true){
            allBoxes.each(function(){
                $(this).prop('disabled', true);
                $(this).closest('.views-exposed-widget').addClass('disabled');
            })
            combine.prop('disabled', true);
            combine.closest('.views-exposed-widget').addClass('disabled');

        }
        if(hasDistrict == false) {
            allBoxes.each(function(){
                if($(this).prop('disabled') == true) {
                    $(this).prop('disabled', false);
                    $(this).closest('.views-exposed-widget').removeClass('disabled');
                }
            });
            combine.prop('disabled', false);
            combine.closest('.views-exposed-widget').removeClass('disabled');
            if(state.val() != "All" && groupable == false && notGroupable == false) {
                district.prop('disabled', false);
                //jquery uniform
                $('#edit-search-district-name-wrapper div.disabled').removeClass('disabled');
            }
            else {
                district.prop('disabled', true);
                //jquery uniform
                $('#edit-search-district-name-wrapper div.selector').addClass('disabled');

            }
        }

        //update quick target button based on meta-variables
        if(notGroupable == true || hasDistrict == true || (groupable == false && hasState == false)) {
            $('input#quick-target').prop("disabled", true).fadeTo(400, 0.6).css({'cursor': 'not-allowed'}).addClass('cancel-hover');
        }

        if((groupable == true || hasState == true)  && hasDistrict == false && notGroupable == false) {
            $('input#quick-target').prop("disabled", false).fadeTo(200, 1).css({'cursor': 'pointer'}).removeClass('cancel-hover');
        }
    }

    // quick target click function, prepares selected items
    function prepareQuickTarget() {
        values = [];
        values["role"] = []
        values["party"] = []
        values["state"] = []

        $('#views-exposed-form-targets-block-3 input[type="checkbox"], #views-exposed-form-targets-block-3 select').each(function(i){
            if ($(this).prop('checked') || (this.name == 'search_state')) {
                var nm = this.name
                var v = this.value
                if (nm.indexOf('role') != -1) {
                    values["role"].push(v);
                }
                if (nm.indexOf('party') != -1) {
                    values["party"].push(v);
                }
                if (nm.indexOf('state') != -1) {
                    if (v != "All")
                    {
                        values["state"].push(v);
                    }
                }

            }
        });

        roles = values["role"].toString().replace(/,/g, '|');
        parties = values["party"].toString().replace(/,/g, '|');
        states = values["state"].toString().replace(/,/g, '|');

        item = [];
        if(roles.length > 0) {
            item.push('roles=' + roles)
        }
        if(states.length > 0) {
            item.push('state=' + states)
        }
        if(parties.length > 0) {
            item.push('party=' + parties)
        }

        addTargets('quick', item);
    }

    // add target
    function addTargets(type, item) {
        // create unique ids for target divs
        if ($('.target-recipient').length !== 0) {
            var count = $(".target-recipient").map(function() {
                    return $(this).attr('id').replace('target-', '');
                }).get().sort(function(a, b){
                    return a-b
                });
            idx = parseInt(count.pop()) + 1;
        }
        else {
            idx = 0;
        }
        if (type == 'search') {
            var query = $(item).attr('href').replace('add-all?', '').replace('add-target?', '').split('&');
        }
        else {
           query = item;
        }
        var readable = buildReadableQuery(query);
        buildDivs(idx, readable, query);
        //window.topper = $('#springboard-advocacy-message-recipients').height();
        buildUpdateMessage();
        buildFormValue();
    }

    // validation error message displayed next to save button
    function buildError(messages) {
        err = $('#advo-error-wrapper').text('').hide().css('margin-bottom', 0);
        err.parent().css({'float': 'left'});
        $.each(messages, function(i, message) {
            err.append('<div>' + message + ' field is required </div>');
        });
        err.fadeIn(500).fadeOut(5000);
    }


    // Rebuild the recipients list on existing messages
    function buildEditPage(recipients) {
       $('#springboard-advocacy-message-recipients-content').text('');
       //var count = 0;
        recipients = recipients.replace(/&quot;/g, '"')
         $.each(JSON.parse(recipients), function(idx, obj) {
            var query = '';
            query = JSON.stringify(obj)
                .replace(/"/g, '')
                .replace(/,/g, '&')
                .replace(/:/g, '=')
                .replace('{', '')
                .replace('}', '')
                .split('&');

            var readable = buildReadableQuery(query);

            buildDivs(idx, readable, query);
            //count++;
       });
        var content = $('#springboard-advocacy-message-recipients-content');
        var contentItems = content.children('.target-recipient');
        content.append(contentItems.get().reverse());
    }

    // Create the recipients list divs, apply data attributes which
    // will be aggregated as a JSON string used by a hidden form field
    // for submission to the API
    function buildDivs(idx, readable, query) {
        $('#springboard-advocacy-message-recipients-content')
            .prepend('<div id = "target-' + idx + '" class = "target-recipient" style="display: none;">' + readable +
            ' <span><a class ="target-delete" href="#">delete</a></span></div>');
        $('#target-' + idx).show(300);
        $('#target-' + idx + ' a').click(function(ev){
            ev.preventDefault();
            $(this).closest('.target-recipient').hide(300, function(){
                $(this).remove();
                buildUpdateMessage();
                buildFormValue();
               // window.topper = $('#springboard-advocacy-message-recipients').height();
            });

        })
        $(query).each(function(index, value) {
            value = value.split('=');
            $('#target-' + idx).attr('data-' + value[0], value[1].replace(/%7C/g, '|'));
        });
    }

    // attaches JSONified data attributes of the recipients list to a hidden form field
    function buildFormValue() {
        var arr = {};
        $('.target-recipient').each(function(i) {
            arr[i] = $(this).data();
        });
        recipients = JSON.stringify(arr).replace(/"/g, '&quot;')//.replace('}}', '}');
        $('input[name="data[recipients]"]').val(recipients);
    }

    function buildUpdateMessage() {
        $('.sba-message-status').text('You have unsaved changes').show('slow');
    }

    // Takes a url query string from the search form "add" links
    // or the quick target parameters
    // and builds readable text for the recipients list.
    function buildReadableQuery(query) {
        var queryObj = {};
        $(query).each(function(index, value) {
            var segments = value.split('=');
            if (segments[0] == 'ids') {
                return false;
            }
            segments[0] = segments[0].ucfirst();
            segments[1] = segments[1].replace(/%7C/g, '|');
            queryObj[segments[0]] = segments[1].split('|');
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
                    case 'O':
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

        if (typeof(queryObj.Id) !== 'undefined') {
            return "Individual: " + queryObj.Sal + " " +  queryObj.First + " " + queryObj.Last;
        }

        cleanUp = JSON.stringify(queryObj).jsonToReadable();
        if (typeof(queryObj.Fields) !== 'undefined' || typeof(queryObj.Gender) !== 'undefined'
            || typeof(queryObj.Social) !== 'undefined' ||  typeof(queryObj.District) !== 'undefined') {
            return "Multiple Individuals: " +  cleanUp;
        }
        return "Group: " +  cleanUp;
    }

    String.prototype.ucfirst = function()
    {
        return this.charAt(0).toUpperCase() + this.substr(1);
    }

    String.prototype.jsonToReadable = function()
    {
      return this.replace(/{"/g, '<ul><li>')
            .replace(/]}/g, '</li></ul>')
            .replace(/\["/g, '')
            .replace(/\],"/g, '</li><li>')
            .replace(/:/g, ': ')
            .replace(/","/g, ', ')
            .replace(/"/g, '')
            .replace(/%20/g, ' ')
    }

    $(document).ready(function () {

        // submit the form or return error message
        $("#edit-submit, #edit-delete").click(function (e) {
            e.preventDefault();
            var messages = [];
            if($('[name*="field_subject_editable"]').length != 0 && !$('[name*="field_subject_editable"]').is(':checked')) {
                messages.push( 'Subject is editable');
            }
            if($('[name*="field_message_editable"]').length != 0 && !$('[name*="field_message_editable"]').is(':checked')) {
                messages.push('Message is editable');
            }
            $('input.required').each(function() {
                if ($(this).val() == '') {
                    messages.push($("label[for='" + this.id + "']").text().replace('*', ''));
                }
            });
            if(messages.length === 0) {
                $("#sba-message-edit-form").submit();
            }
            else {
                buildError(messages);
            }
        });

        // rearrange some divs
        finder = $('.springboard-advocacy-find-targets-container');
        actions = $('#sba-message-edit-form #edit-actions');
        err = $('#advo-error-wrapper');
        err.css({'display': 'inline-block', 'padding-left': '20px'});
        $('#springboard-advocacy-message-form-container').append(finder);
        $(finder).append(actions);
        $(actions).append(err);
        $('input#quick-target').prop("disabled", true).fadeTo(400, 0.6).css({'cursor': 'not-allowe'}).addClass('cancel-hover');

        // Editing a pre-existing message, append the recipients
        // to the recipients div using hidden form value
        var recipients =  $('input[name="data[recipients]"]').val();
        if(recipients.length > 0) {
            $('body').once('edit-page', function() {
                buildEditPage(recipients);
            });
        }
        else {
            $('.sba-message-status').text('No recipients have been selected.').show('slow');
        }

        // scroll the recipients box
        var offset = $('#springboard-advocacy-message-recipients').offset();
        $(window).scroll(function() {
            if(offset.top <= $(window).scrollTop() && $('#springboard-advocacy-message-recipients').css('position') == 'absolute') {
                newTop =$(window).scrollTop() - offset.top;
                $('#springboard-advocacy-message-recipients').css('top', newTop).addClass('recipients-fixed');

            }
            else {
                $('#springboard-advocacy-message-recipients').css('top', 0).removeClass('recipients-fixed');
            }
        });
    });
})(jQuery);