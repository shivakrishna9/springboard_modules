Drupal.behaviors.fundraiserGateway = {
  attach: function(context) { (function($) {
    $(document).ready(function() {
      $("#edit-submit").click(function(){
        return confirm("A misconfiguration can cause transactions to fail. Are you sure you want to proceed?");
      });
    });
  })(jQuery);}
};
