Drupal.behaviors.fundraiserBehavior = function() {
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

  // Auto-populate label fields when amount is entered
  $("#ask-amounts .amount_field").blur(function() {
    $id = $(this).attr('id');
    $id = $id.split('-');
    $delta = $id[2];
    $("#edit-label-" + $delta).val("$" + $(this).attr('value'));
  });
};

