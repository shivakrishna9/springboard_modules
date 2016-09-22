(function ($) {
  Drupal.behaviors.SpringboardTokenSet = {
    attach: function (context, settings) {
      // Add token click handler to form elements with token set IDs:
      $(".form-wrapper, #webform-component-edit-form #edit-value, #webform-component-edit-form #edit-extra-description").each(function() {
        if ($(this).hasClass('token-ui-field')) {
          $(this).addClass("has-token-data");
          var targetElement = $(this).find("textarea");
          if (targetElement.length == 0) {
            targetElement = $(this);
          }
          targetElement.filter('textarea').each(function () {
            $(this).after('<div class="sb-tokens-expander"><a href="#">+ View tokens</a></div>');
            var targetTextarea = $(this);
            targetTextarea.parent().children('.sb-tokens-expander').children('a').click(function (e) {
              e.preventDefault();
              if (typeof targetTextarea.attr('sb-selection-start-pos') != 'undefined') {
                targetTextarea.attr('sb-token-initial-cursor-pos', targetTextarea.attr('sb-selection-start-pos'));
              }
              else {
                var endPosition = targetTextarea.val().length;
                targetTextarea.attr('sb-token-initial-cursor-pos', endPosition);
              }
              targetTextarea.click();
            });
          });

          targetElement.filter("textarea").keypress(function () {
            if (typeof $(this)[0].selectionStart != 'undefined') {
              $(this).attr('sb-selection-start-pos', $(this)[0].selectionStart + 1);
              $(this).attr('sb-selection-end-pos', $(this)[0].selectionStart + 1);
            }
          });

          targetElement.filter("textarea").select(function() {
            if (typeof $(this)[0].selectionStart != 'undefined') {
              $(this).attr('sb-selection-start-pos', $(this)[0].selectionStart);
            }
            if (typeof $(this)[0].selectionEnd != 'undefined') {
              $(this).attr('sb-selection-end-pos', $(this)[0].selectionEnd);
            }
            else if (typeof $(this).attr('sb-selection-start-pos') != 'undefined') {
              $(this).attr('sb-selection-end-pos', $(this).attr('sb-selection-start-pos'));
            }
          });       
 
          targetElement.filter("textarea").click(function() {
            $('.sb-tokens-expander, .sb-tokens-expander a').each(function () {
              $(this).show(); 
            });
            $(this).parent().children('.sb-tokens-expander').hide();
            if (typeof $(this).attr('sb-token-initial-cursor-pos') != 'undefined') {
              $(this).attr('sb-selection-start-pos', $(this).attr('sb-token-initial-cursor-pos'));
              $(this).attr('sb-selection-end-pos', $(this).attr('sb-token-initial-cursor-pos'));
              $(this).removeAttr('sb-token-initial-cursor-pos');
            }
            else if (typeof $(this)[0].selectionStart != 'undefined') {
              $(this).attr('sb-selection-start-pos', $(this)[0].selectionStart);
              $(this).attr('sb-selection-end-pos', $(this)[0].selectionEnd);
            }
            $(this).addClass('sb-token-textarea');

            Drupal.settings.token_set_last_selected_field = this;
            showTokens($(this));
            $('.first-token-set').click();
            $('.first-token-set').removeClass('first-token-set');
            var tokensContainer;
            $(this).siblings('#token-set-tokens').each(function () {
              tokensContainer = $(this);
            });
            $(this).parent().children(".description").each(function () {
              $(this).insertAfter(tokensContainer);
            });
            if (!$('.sb-tokens-contractor').length) {
              $(this).after('<div class="sb-tokens-contractor"><a href="#">- Hide tokens</a></div>');
              $(this).parent().children('.sb-tokens-contractor').children('a').click(function (e) {
                e.preventDefault();
                $(this).parent().parent().children('.sb-tokens-expander').show();
                $('#token-set-tokens').remove();
                $('.sb-token-textarea').each(function () {
                  $(this).parent().removeAttr('sb-selection-start-pos');
                  $(this).parent().removeAttr('sb-selection-end-pos');
                });
                $(this).parent().remove();
              });
            }
          });
        }
      });

      var showTokens = function (element) {
        // Remove the existing token widget if a new widget has been clicked; otherwise, do nothing:
        if ($('#token-set-tokens').attr('target-element-id') == $(element).attr('id')) {
          return;
        }
        $('#token-set-tokens').remove();
        $('.sb-tokens-contractor').remove();
        $('.sb-token-textarea').each(function () {
          if (!$(this).is(element)) {
            $(this).removeAttr('sb-selection-start-pos');
            $(this).removeAttr('sb-selection-end-pos');
          }
        });
        var targetElementID = $(element).attr('id');
        var tokenFieldKey = element.closest('.has-token-data').attr("token_key");
        if (Drupal.settings['field_token_sets__' + tokenFieldKey] != undefined) {
          var tokens = Drupal.settings["field_token_sets__" + tokenFieldKey];
        }
        else {
          return;
        }
        // Generate tokens markup.
        var last_token_type = '';
        var headerHTML = '';
        /*headerHTML += '<div class="token-ui-selector"><select>' +
          '<option value="">- Browse Token Sets -</option><option value="all">- View All Tokens -</option>';
        for (var tsid in Drupal.settings.token_sets_list) {
          headerHTML += '<option value="' + tsid + '">' + Drupal.settings.token_sets_list[tsid] + '</option>';
        }
        headerHTML += '</select></div>'; */
        headerHTML += '<div class="token-ui-header">'; 
        var html = '<fieldset id="token-set-tokens" class="form-wrapper">';
        html += '<div class="token-set-tokens-spike">&nbsp;</div>';
        html += '<div class="token-set-tokens-inner">';
        html += '<legend><span class="fieldset-legend">Tokens</span></legend>';
        html += '<div class="fieldset-wrapper token-set-wrapper">';
        html += '<div class="token-list">';
        var firstTokenSet = ' first-token-set token-set-expanded';
        var tokenTabsAdded = [];
        var tokensAdded = [];
        var tokenSetNames = Drupal.settings['token_set_names'];
        for (var i = 0; i < tokens.length; i++) {
          var tsid = tokens[i];
          var tokenSetName = tokenSetNames[tsid];
          tokenTabsAdded.push(tokenSetName);
          headerHTML += '<a class="token-set-expand' + firstTokenSet + '" tsid="' + tsid + '" ' +
          'data-expanded="0" href="#">' + tokenSetName + '</a>';

          firstTokenSet = '';
        }
        headerHTML += '</div>';
        

        html += '</div></div></div>';
        html += '</fieldset>';
        element.parent().append(html);
        $('#token-set-tokens').attr('target-element-id', targetElementID);
        $('#token-set-tokens legend').after(headerHTML);


        var fetchTokenSet = function (tsid) {
          $.ajax({
            url: '/sb-token-set-ajax/' + tsid,
            dataType: 'json',
            context: document.body,
            success: function(data) {
              var token = '';
              var rowHTML = '';
              var tokenExists;
              $('.token-list .token-set-token-row').hide();
              for (var i = 0; i < data.length; i++) {
                tokenExists = false;
                token = {
                  token: data[i]['token'],
                  token_type: data[i]['token_type'],
                  tsid: tsid,
                  token_description: data[i]['token_description']
                };
                $('.token-list .token-set-token-row').each(function() {
                  if ($(this).attr('data-token') == data[i]['token']) {
                    tokenExists = true;
                    $(this).show();
                  }
                });
                if (tokenExists == false) {
                  rowHTML = renderToken(token);
                  $('.token-list').prepend(rowHTML);
                  $('.token-list .token-set-token-row').each(function() {
                    if ($(this).attr('data-token') == data[i]['token']) {
                      $(this).show();
                      $(this).click(function(event) {
                        event.preventDefault();
                        var token = $(this).data("token");
                        insertToken(Drupal.settings.token_set_last_selected_field, token);
                      });
                    }
                  });
                }
              }
            }
          });
        };

        $(".token-set-expand").each(function() {
          $(this).click(function(event) {
            event.preventDefault();
            var targetTsid = $(this).attr('tsid');
            $(".token-set-expand").removeClass("token-set-expanded");
            $(this).addClass('token-set-expanded');
            if ($(this).hasClass('ajax-fetch-complete')) {
              $('.token-set-token-row').each(function() {
                if ($(this).attr('tsid') == targetTsid) {
                  $(this).show();
                } else {
                  $(this).hide();
                }
              });  
            } else {
              fetchTokenSet(targetTsid);
              $(this).addClass('ajax-fetch-complete');
            }
          });
        });

        $('.token-set-token-row').each(function() {
          $(this).click(function(event) {
            event.preventDefault();
            var token = $(this).data("token");
            insertToken(Drupal.settings.token_set_last_selected_field, token);
          });
        });
      };

      $('.token-set-token-row').click(function () {
        event.preventDefault();
        $(this).children('a.token-set-token-link').click();
      });


      var renderToken = function (token) {
        var html = '<div class="token-set-token-row token-set-token-type-' + token.token_type.replace(' ', '-') + '" ' +
          'tsid="' + token.tsid + '" data-token="' + token.token + '">';
        html += '<div class="token-col token-machine">' + token.token + '</div>';
        html += '<div class="token-col token-descr">' + token.token_description + '</div>';
        html += '<div class="token-col token-use">use token</div>';
        html += '</div>';
        return html;
      };

      var insertToken = function (element, token) {
        var cursorPos;
        var cursorEndPos;
        if (typeof $(element).attr('sb-selection-start-pos') != 'undefined') {
          cursorPos = $(element).attr('sb-selection-start-pos');
        }
        else if (typeof $(element)[0].selectionStart != 'undefined') {
          cursorPos = $(element)[0].selectionStart;
        }
        else {
          cursorPos = $(element).val().length;
        }

        if (typeof $(element).attr('sb-selection-end-pos') != 'undefined') {
          cursorEndPos = $(element).attr('sb-selection-end-pos');
        }
        else if (typeof $(element)[0].selectionEnd != 'undefined') {
          cursorEndPos = $(element)[0].selectionEnd;
        }
        else {
          cursorEndPos = cursorPos; 
        }
        
        // Reset the textarea's content with the selection replaced with this token:
        element.value = element.value.substring(0, cursorPos)
          + token
          + element.value.substring(cursorEndPos, element.value.length);

        // If a selection was replaced, the selection's end position is no longer valid:
        $(element).attr('sb-selection-start-pos', cursorPos);
        $(element).attr('sb-selection-end-pos', cursorPos);
      };
    }
  };
})(jQuery);
