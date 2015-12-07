(function ($) {
  Drupal.behaviors.SpringboardTokenSet = {
    attach: function (context, settings) {
      // Add token click handler to form elements with token set IDs.
      $(".form-wrapper").each(function() {
        var token_set_id = $(this).data("token-set-id");

        if (typeof token_set_id !== typeof undefined) {
          $(this).click(function() {
            showTokens($(this));
          });
        }
      });

      var showTokens = function (element) {
        // Remove existing tokens div.
        $("#token-set-tokens").remove();

        var token_set_id = element.data("token-set-id");
        var tokens = Drupal.settings.token_sets[token_set_id];

        // Generate tokens div.
        var html = '<ul id="token-set-tokens">';
        for (var i = 0; i < tokens.length; i++) {
          html += renderToken(tokens[i].token_type, tokens[i].token_name);
        }
        html += '</ul>';

        element.append(html);
      };

      var renderToken = function (token_type, token_name) {
        var html = '<li>[' + token_type + ":" + token_name + "]</li>";

        return html;
      };
    }
  };
})(jQuery);
