Drupal.behaviors.fundraiserBehavior = {
  attach: function(context, settings) { (function($) {

  // Auto-populate the internal name field
  $("#edit-title").blur(function() {
    var title = $(this).attr('value');
    var currently = $("#edit-internal-name").attr('value');
    if (currently.length == 0) {
      $("#edit-internal-name").val(title);
    }
  });

  // Auto-populate label fields when amount is entered.
  $('#ask-amounts input[id*="amount"]').blur(function() {
    var amount = $(this).val();
    var label = $(this).parents('tr').find('input[id*="label"]').val();
    if (label == '') {
      $(this).parents('tr').find('input[id*="label"]').val("$" + amount);
    }
  });

  // Automatically change submit button text when payment gateway selected.
  var submitText = function() {
      var gateway = $(this).val();
      var text = Drupal.settings.fundraiser[gateway];
      $("#edit-submit").val(text);
  }
  $('input[class*="fundraiser-payment-methods"]').change(submitText);
  // Set button text when page first loaded
  $('input[class*="fundraiser-payment-methods"]').trigger('change');

  })(jQuery); }
}
