(function ($) {
  Drupal.behaviors.SpringboardTokenSet = {
    attach: function (context, settings) {
      // Add token click handler to form elements with token set IDs.
      $(".form-wrapper").each(function() {
        var token_set_id = $(this).data("token-set-id");

        if (typeof token_set_id !== typeof undefined) {
          $(this).addClass("has-token-data");

          $(this).find("input").click(function() {
            Drupal.settings.token_set_last_selected_field = this;
            showTokens($(this));
          });
          $(this).find("textarea").click(function() {
            Drupal.settings.token_set_last_selected_field = this;
            showTokens($(this));
          });
        }
      });

      var showTokens = function (element) {
        // Remove existing tokens div.
        $("#token-set-tokens").remove();

        var token_set_id = element.closest(".has-token-data").data("token-set-id");
        var tokens = Drupal.settings.token_sets[token_set_id];

        // Generate tokens markup.
        var last_token_type = '';
        var html = '<fieldset class="form-wrapper">';
        html += '<legend><span class="fieldset-legend">Tokens</span></legend>'
        html += '<div class="fieldset-wrapper token-set-wrapper">'
        html += '<table id="token-set-tokens">';
        for (var i = 0; i < tokens.length; i++) {
          if (tokens[i].token_type !== last_token_type) {
            html += '<tr><th colspan="3">' + tokens[i].token_type + '</th></tr>';
            last_token_type = tokens[i].token_type;
          }

          html += renderToken(tokens[i]);
        }
        html += '</div>';
        html += '</table>';
        html += '</fieldset>';

        element.parent().append(html);

        $(".token-set-token-link").each(function() {
          $(this).click(function(event) {
            event.preventDefault();

            var token = $(this).data("token");

            insertToken(Drupal.settings.token_set_last_selected_field, token);
          });
        });
      };

      var renderToken = function (token) {
        var html = '<tr>';
        html += '<td>' + token.token_type + '</td>';
        html += '<td><a class="token-set-token-link" data-token="' + token.token + '" href="#">' + token.token + '</a></td>';
        html += '<td>' + token.token_description + '</td>';
        html += '</tr>';
        return html;
      };

      var insertToken = function (element, token) {
        // IE support.
        if (document.selection) {
          element.focus();
          sel = document.selection.createRange();
          sel.text = token;
        }
        // Mozilla support.
        else if (element.selectionStart || (element.selectionStart === '0')) {
          element.value = element.value.substring(0, element.selectionStart)
            + token
            + element.value.substring(element.selectionEnd, element.value.length);
        } else {
          element.value += token;
        }
      };
    }
  };
})(jQuery);
