Drupal.behaviors.fundraiserBehavior = {
  attach: function(context) { (function($) {

  var fundraiser = '';
  // Auto-populate the internal name field
  $("#edit-title").blur(function() {
    var title = $(this).attr('value');
    $("#edit-internal-name").val(title);
  });

  // Auto focus on the amounts.
  $("#ask-amounts .amount_field").focus(function() {   
    var amount = $(this).attr('value');
    var id = $(this).attr('id');
    window.fundraiser_amount = {
      id : id,
      amount : amount,
    }
  });

  // Auto-populate label fields when amount is entered
  $("#ask-amounts .amount_field").blur(function() {
    var id = $(this).attr('id');
    var prev_amount = fundraiser[id];
    var id = id.split('-');
    var delta = id[2];
    var amount = $(this).attr('value');
    var label = $("#edit-label-" + $delta).attr('value');
    if (label == '' || label == "$" + window.fundraiser_amount.amount) {
      $("#edit-label-" + delta).val("$" + amount);
    }
  });

  })(jQuery); }
}
