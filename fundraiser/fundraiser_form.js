Drupal.behaviors.fundraiserBehavior = function() {
  $fundraiser = '';
  // Auto-populate the internal name field
  $("#edit-title").blur(function() {
    $title = $(this).attr('value');
    $("#edit-internal-name").val($title);
  });

  // Remove form elements when remove button is clicked.
  $("#ask-amounts .form-submit").click(function(e) {
    $(this).hide();
    $name = $(this).attr('id');
    $name = $name.split('-');
    $delta = $name[2];
    $("#edit-amount-" + $delta).remove();
    $("#edit-label-" + $delta).remove();
    $("#edit-label-" + $delta + "-wrapper, #edit-amount-" + $delta + "-wrapper").remove();
    e.preventDefault(); // Thanks for the link Brock!
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
    alert ('prev value:' + window.fundraiser_amount.amount);
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

