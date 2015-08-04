Drupal.behaviors.commercePayWithMyBank = {
  attach: function(context) { (function($) {
    $(document).ready(function() {
      $("#edit-continue").click(function(event){
        event.preventDefault();
        PayWithMyBank.establish(Drupal.settings.paywithmybank_establish_data);
      });
    });
  })(jQuery);}
};
