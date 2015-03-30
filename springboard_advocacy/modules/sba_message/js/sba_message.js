/**
 * @file
 */

/**
 * Advocacy recipients list  and search page
 */
(function ($) {

    Drupal.behaviors.AdvocacyMessageRecipients = {
        attach: function(context, settings) {

            $('.view-targets').once('advocacy-committee-search', function() {
                var finder = $('#springboard-advocacy-find-targets-container .view-targets');
                finder.prepend('<div class="faux-tab-container"><div class="faux-tab"' +
                '><a href ="#full" class="full-search">Full Search</a></div><' +
                'div class="faux-tab"><a href ="#committee" class="committee-search">Committee Search</a></div>');
            });

            $('.committee-search').click(function () {
                reset('committee');
                $('.radio-widgets, #state-district-wrapper,#edit-combine-wrapper, .search-reset, .views-targets-button-wrapper').hide();
                $('#edit-search-committee-wrapper').show(300);
                $('a.committee-search').closest('.faux-tab').addClass('active');
                $('a.full-search').closest('.faux-tab').removeClass('active');
                window.search_state = 'committee';
                return false;

            });

            $('.full-search').click(function () {
                $('#edit-search-committee-wrapper').hide();
                $('a.committee-search').closest('.faux-tab').removeClass('active');
                $('a.full-search').closest('.faux-tab').addClass('active');
                $('.radio-widgets,#state-district-wrapper,#edit-combine-wrapper, .search-reset, .views-targets-button-wrapper').show(300);
                window.search_state = 'full-search';
                return false;

            });

            if(window.search_state == 'committee') {
                $('.radio-widgets,#state-district-wrapper,#edit-combine-wrapper, .search-reset, .views-targets-button-wrapper').hide();
                $('#edit-search-committee-wrapper').show(300);
            }
            else {
                $('a.committee-search').closest('.faux-tab').removeClass('active');
                $('a.full-search').closest('.faux-tab').addClass('active');
                $('.radio-widgets,#state-district-wrapper,#edit-combine-wrapper, .search-reset').show(300);
                $('#edit-search-committee-wrapper').hide();
            }

            if ($('#edit-combine').text().length == 0) {
                var placeholder = $('#edit-combine-wrapper .description').text().trim();
                $('#edit-combine-wrapper .description').hide();
                $('#edit-combine').attr('placeholder', placeholder);
                $('#edit-combine').focus(function () {
                    $(this).attr('placeholder', '');
                });
            }

            if ($('#edit-search-committee').text().length == 0) {
                var placeholder = $('#edit-search-committee-wrapper .description').text().trim();
                $('#edit-search-committee-wrapper .description').hide();
                $('#edit-search-committee').attr('placeholder', placeholder);
                $('#edit-search-committee').focus(function () {
                    $(this).attr('placeholder', '');
                });
            }
            // Apply click event to the search form add links
            // Allows views search results to be appended to the recipients list
            var links = $('a.advocacy-add-target, a#advo-add-all');
            links.each(function() {
                $(this, context).once('advocacy-add-target', function() {
                    $(this).click(function (e){
                        e.preventDefault();
                        addTarget('search', this);
                    });
                });
            });

            if ($.isFunction($.fn.uniform)) {
                var deUniform
                clearTimeout(deUniform);
                deUniform = setTimeout(function () {
                    $('select').each(function () {
                        $.uniform.restore(this);
                    });
                }, 10);
            }

            //update exposed form element states when a district is selected
            $('select[name="search_district_name"]', context).once('advocacy-district-reloaded', function() {
                setElStates();

                $('#views-exposed-form-targets-block-3 input, #views-exposed-form-targets-block-3 select').on('change', function(){
                    if(this.type != 'button' && this.type !='hidden') {
                        if($('div.view-targets').is(':visible')) {
                            reset();
                        }
                        setElStates();
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
                        if($('div.view-targets').is(':visible')) {
                            reset();
                        }
                        setElStates();
                    }, 400);
                }
            });

            //set the click event for the quick target button
            $('#quick-target', context).once('advocacy-add-quick-target', function() {
                setElStates();//need this?
                reset('all');
                $('input#quick-target').click(function() {
                    var query = getQuickQuery();
                    addTarget('quick', query);
                });
            });
        }
    };

    // selectively disable/enable exposed form elements based on user actions
    function setElStates() {

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
               nm.indexOf('social')  != -1 || nm.indexOf('district') != -1 ||
                nm.indexOf('PRES')  != -1 || nm.indexOf('GOVNR') != -1) {
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
                //console.log(this)

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
                $(this).siblings('label').css({'cursor': 'not-allowed'});
            })
            combine.prop('disabled', true);
            combine.closest('.views-exposed-widget').addClass('disabled');

        }
        if(hasDistrict == false) {
            allBoxes.each(function(){
                if($(this).prop('disabled') == true) {
                    $(this).prop('disabled', false);
                    $(this).closest('.views-exposed-widget').removeClass('disabled');
                    $(this).siblings('label').css({'cursor': 'pointer'});

                }
            });
            combine.prop('disabled', false);
            combine.closest('.views-exposed-widget').removeClass('disabled');
            if(state.val() != "All" && groupable == false && notGroupable == false) {
                district.prop('disabled', false);
                district.removeClass('disabled');

                //jquery uniform
                $('#edit-search-district-name-wrapper div.disabled').removeClass('disabled');
            }
            else {
                district.prop('disabled', true);
                district.addClass('disabled');

                //jquery uniform
                $('#edit-search-district-name-wrapper div.selector').addClass('disabled');
            }
        }

        //update quick target button based on meta-variables
        if(notGroupable == true || hasDistrict == true || (groupable == false && hasState == false)) {

            $('.views-targets-button-wrapper').prop("disabled", true).fadeTo(400, 0).css({'cursor': 'default'}).addClass('cancel-hover');
            $('.views-targets-button-wrapper input').prop("disabled", true).fadeTo(400, 0).css({'cursor': 'default'}).addClass('cancel-hover');

        }

       else if((groupable == true || hasState == true)  && hasDistrict == false && notGroupable == false) {
            $('.views-targets-button-wrapper').prop("disabled", false).fadeTo(200, 1).css({'cursor': 'pointer'}).removeClass('cancel-hover');
            $('.views-targets-button-wrapper input').prop("disabled", false).fadeTo(200, 1).css({'cursor': 'pointer'}).removeClass('cancel-hover');
        }
    }

    // quick target click function, builds query array
    function getQuickQuery() {

        var roles = []
        var parties = []
        var states = []

        $('#views-exposed-form-targets-block-3 input[type="checkbox"], #views-exposed-form-targets-block-3 select').each(function(i){
            if ($(this).prop('checked') || (this.name == 'search_state' && this.value != "All")) {
                var nm = this.name
                var v = this.value
                if (nm.indexOf('role') != -1) {
                    roles.push(v);
                }
                else if (nm.indexOf('party') != -1) {
                    parties.push(v);
                }
                else if (nm.indexOf('state') != -1) {
                    states.push(v);
                }
                else return false;
            }
        });

        roles = roles.toString().replace(/,/g, '|');
        parties = parties.toString().replace(/,/g, '|');
        states = states.toString().replace(/,/g, '|');

        var query = [];
        if(states.length > 0) {
            query.push('state=' + states)
        }
        if(roles.length > 0) {
            query.push('role=' + roles)
        }
        if(parties.length > 0) {
            query.push('party=' + parties)
        }
        return query;
    }

    // add target
    function addTarget(type, query) {
        // create unique ids for target divs
        if (type == 'search') {
            var qArr = $(query).attr('href').replace('add-all?', '').replace('add-target?', '').split('&');
        }
        else {
           qArr = query;
        }
        var readable = buildReadableQuery(qArr);
        var id = calcDivId()
        buildDiv(id, readable, qArr, false);
        setUpdateMessage();
        setCountMessage();
        setFormValue();
    }

    function calcDivId(){
        if ($('.target-recipient').length !== 0) {
            var count = $(".target-recipient").map(function() {
                return $(this).attr('id').replace('target-', '');
            }).get().sort(function(a, b){
                return a-b
            });
            var  id = parseInt(count.pop()) + 1;
        }
        else {
            id = 0;
        }
        return id;
    }

    // validation error message displayed next to save button
    function setError(messages) {
        var err = $('#advo-error-wrapper');
        err.text('').hide().css('margin-bottom', 0);
        err.prepend('<div class = "advo-warning"><strong>Oops!</strong> it looks like you missed the following required fields:</div>');
        $.each(messages, function(i, message) {
            err.append('<div>' + message + ' field is required </div>');
        });
        err.fadeIn(500);//.delay(7000).fadeOut(1000);
    }


    // Rebuild the recipients list on existing messages
    function buildEditPage(recipients) {
       $('#springboard-advocacy-message-recipients-content').text('');
        recipients = recipients.replace(/&quot;/g, '"')
         $.each(JSON.parse(recipients), function(id, obj) {
            var query = '';
            query = JSON.stringify(obj)
                .replace(/"/g, '')
                .replace(/,/g, '&')
                .replace(/:/g, '=')
                .replace('{', '')
                .replace('}', '')
                .split('&');

            var readable = buildReadableQuery(query);
            buildDiv(id, readable, query, 'reload');
       });
        //reverse the display order
        var content = $('#springboard-advocacy-message-recipients-content');
        var contentItems = content.children('.target-recipient');
        content.append(contentItems.get().reverse());
        setCountMessage();
    }

    // Create the recipients list divs, apply data attributes which
    // will be aggregated as a JSON string used by a hidden form field
    // for submission to the API
    function buildDiv(id, readable, query, reload) {
        console.log(query)
        if (reload == false) {
            var isnew = 'new';
        }
        else{
            isnew = 'reloaded';
        }
        $('#springboard-advocacy-message-recipients-content')
            .prepend('<div id = "target-' + id + '" class = "target-recipient ' + isnew +'" style="display: none;">' + readable +
            ' <span><a class ="target-delete remove-target" href="#"></a></span></div>');
        $('#target-' + id).show(200).addClass('trans');
        $('#target-' + id + ' a').click(function(ev){
            ev.preventDefault();
            $(this).closest('.target-recipient').hide(300, function(){
                $(this).remove();
                setUpdateMessage();
                setFormValue();
                setCountMessage();
            });

        })
        $(query).each(function(i, v) {
            v = v.split('=');
            $('#target-' + id).attr('data-' + v[0], v[1].replace(/%7C/g, '|'));
        });
    }

    // attaches JSONified data attributes of the recipients list to a hidden form field
    function setFormValue() {
        var obj = {};
        $('.target-recipient').each(function(i) {
            obj[i] = $(this).data();
        });
        recipients = JSON.stringify(obj).replace(/"/g, '&quot;');
        $('input[name="data[recipients]"]').val(recipients);
    }

    function setUpdateMessage() {
        $('.sba-message-status').text('You have unsaved changes').show('slow');
    }

    function setCountMessage() {
        $('.targeting-count, .remove-all-targets').remove();
        var message = [];
        var groups = parseInt($('.target-recipient .title').length);
        var individs = parseInt($('.target-recipient .individual').length);
        var gCount = groups > 1 ? 'groups' : 'group'
        var iCount = individs > 1 ? 'individuals' : 'individual';
        //'<strong>Targeting: </strong> '
        if(groups > 0) {
            message.push(groups + ' ' + gCount);
        }
        if(individs > 0 ) {
            message.push(individs + ' ' + iCount)
        }

        countMessage =     '<strong>Targeting: </strong> ' + message.join(' & ');
        var counter = '<div class="targeting-count">' + countMessage + '</div><a class="remove-all-targets">Remove All Targets</a></div>';
        if(individs > 0 || groups > 0) {
            $('#springboard-advocacy-message-recipients-content').append(counter);
        }

        $('.remove-all-targets').click(function(){
            $('.target-recipient').remove();
            $('.targeting-count, .remove-all-targets').remove();
            setUpdateMessage();
            setFormValue();
        });
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
                    case 'PRES01':
                        queryObj[segments[0]][i] = 'US President';
                        break;
                    case 'PRES03':
                        queryObj[segments[0]][i] = 'US Vice-President';
                        break;
                    case 'GOVNR':
                        queryObj[segments[0]][i] = 'State Governors';
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
            return  '<div class="individual">' + queryObj.Sal + " " +  queryObj.First + " " + queryObj.Last + '</div>';
        }
        var cleanUp = JSON.stringify(queryObj).jsonToReadable();
        if (typeof(queryObj.Fields) !== 'undefined' || typeof(queryObj.Gender) !== 'undefined'
            || typeof(queryObj.Social) !== 'undefined' ||  typeof(queryObj.District) !== 'undefined') {
            return '<div class="title"><h4>Group Target</h4></div>' +  cleanUp;
        }
        return '<div class="title"><h4>Group Target</h4></div> ' +  cleanUp;
    }

    function scroller() {
        setTimeout(function() {
            var offset = $('#springboard-advocacy-message-recipients').offset();
            var newTop;
            $(window).scroll(function() {
                if(offset.top <= $(window).scrollTop() && $('#springboard-advocacy-message-recipients').css('position') == 'absolute') {
                    //console.log(offset.top);

                    newTop =$(window).scrollTop() - offset.top;
                    //console.log(newTop);
                    $('#springboard-advocacy-message-recipients').css('top', newTop).addClass('recipients-fixed');
                }
                else {
                    $('#springboard-advocacy-message-recipients').css('top', 0).removeClass('recipients-fixed');
                }
            });

        }, 500);
    }

    function reset($type) {
        if ($type == 'all') {
            $('.views-submit-button').append('<a class = "search-reset" href ="#">reset</a>');
            $('.search-reset').click(function () {
                resetFull();
                return false;
            });
        }
        else if ($type == 'committee') {
            resetFull();
        }
        else {
            $('.view-content').fadeOut(333);
            $('.attachment').fadeOut(333);
            $('div.view-targets .item-list').fadeOut(333);
        }
    }

    function resetFull() {
        var allInputs = $('#views-exposed-form-targets-block-3 input[type="checkbox"], #views-exposed-form-targets-block-3 input[type="text"], #views-exposed-form-targets-block-3 select');
        allInputs.each(function () {
            if ($(this).prop('tagName') == "SELECT") {
                $(this).prop('selectedIndex', 0);
                if ($.isFunction($.fn.uniform)) {
                    $.uniform.restore(this);
                }
            }
            if (this.type == 'checkbox') {
                $(this).prop('checked', false);
                $(this).prop('disabled', false);
                $(this).removeClass('disabled');
                $(this).siblings('label').removeClass('disabled').css({'cursor': 'pointer'});
                $(this).closest('.views-exposed-widget').removeClass('disabled');
            }
            if (this.type == 'text') {
                $(this).val('');
            }
        });
        $('.views-targets-button-wrapper').fadeOut(333);
        $('.view-content').fadeOut(333);
        $('.attachment').fadeOut(333);
        $('div.view-targets .item-list').fadeOut(333);
        $('select[name="search_district_name"]').prop('disabled', true).addClass('disabled');
        return false;
    }
    //Uc first helper
    String.prototype.ucfirst = function()
    {
        return this.charAt(0).toUpperCase() + this.substr(1);
    }

    //json notation to readable string helper
    String.prototype.jsonToReadable = function()
    {
      return this.replace(/{"/g, '<ul><li class ="heading">')
            .replace(/]}/g, '</li></ul>')
            .replace(/\["/g, '<li>')
            .replace(/\],"/g, '</li></ul><ul><li class ="heading">')
            .replace(/:/g, '</li>')
            .replace(/","/g, '<li>')
            .replace(/"/g, '')
            .replace(/%20/g, ' ')
    }

    // Main set of functions which need to happen on every page load
    $(document).ready(function () {
        if ($.isFunction($.fn.uniform)) {
            $('select').each(function () {
                $.uniform.restore(this);
            });
        }
        //stop views ajax auto scroll
        $('.pager-item a, #edit-submit-targets').click(function(){
            Drupal.ajax.prototype.commands.viewsScrollTop = null;
        });
        var showEdit = $('input[name*=field_user_editable]');
        var editable = $('#sba_message_sba_message_action_message_form_group_editable, #edit-field-bottom-conclusion')
         if(showEdit.prop('checked')) {
            editable.show();
             scroller();
         }
        showEdit.on('change', function() {
            if(this.checked) {
                editable.show(400, 'linear');
                scroller();
            }
            else {
                 editable.hide(400, 'linear');
                 scroller();
            }
        });

        // submit the form or return error message
        $("#edit-submit, #edit-delete").click(function (e) {
            e.preventDefault();
            var messages = [];
            //@todo add the rest of required fields
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
                setError(messages);
            }
        });

        // rearrange some div orders
        var recipContainer = $('#springboard-advocacy-message-recipients-container');

        var finder = $('#springboard-advocacy-find-targets-container');

        var actions = $('#sba-message-edit-form #edit-actions');
        var err = $('#advo-error-wrapper');
        $('#springboard-advocacy-message-form-container').append(finder);
        finder.append(recipContainer)
        finder.append(actions);
        actions.append(err);
        $('.views-targets-button-wrapper').hide();
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

        scroller();
    });

})(jQuery);