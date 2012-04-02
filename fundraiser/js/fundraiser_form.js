Drupal.behaviors.fundraiserBehavior = {
  attach: function(context) { (function($) {

  $fundraiser = '';
  // Auto-populate the internal name field
  $("#edit-title").blur(function() {
    $title = $(this).attr('value');
    $("#edit-internal-name").val($title);
  });

  $("#ask-amounts .amount_field").focus(function() {   
    $amount = $(this).attr('value');
    $id = $(this).attr('id');
    window.fundraiser_amount = {
      id : $id,
      amount : $amount,
    }
  });
  // Auto-populate label fields when amount is entered
  $("#ask-amounts .amount_field").blur(function() {
    $id = $(this).attr('id');
    $prev_amount = $fundraiser[$id];
    $id = $id.split('-');
    $delta = $id[2];
    $amount = $(this).attr('value');
    $label = $("#edit-label-" + $delta).attr('value');
    if ($label == '' || $label == "$" + window.fundraiser_amount.amount) {
      $("#edit-label-" + $delta).val("$" + $amount);
    }
  });
};

  })(jQuery); }
} 

