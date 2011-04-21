$(document).ready(function() {

  $("#edit-title").blur(function() {
    $title = $(this).attr('value');
    $("#edit-internal-name").val($title);
  });

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
});
