(function ($) {
  Drupal.behaviors.SpringboardTokenSet = {
    attach: function (context, settings) {
      $(".form-wrapper").each(function() {
        var token_set_id = $(this).data("token-set-id");

        if (typeof token_set_id !== typeof undefined) {
          $(this).click(function() {
            ShowTokens($(this));
          });
        }
      });

      function ShowTokens(element) {
        var token_set_id = element.data("token-set-id");

        console.log("ShowTokens: " + token_set_id);

      }
    }
  };
})(jQuery);
