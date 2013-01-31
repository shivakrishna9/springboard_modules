$(document).ready(function() {
  $('.webform-dedupe').click(function(e) {
    e.preventDefault();
    var url = $(this).attr('href');
    var action = $(this).text();
    // ajax to set or remove dedupe field
    $.get(url);

    // check the action
    if (action == 'Remove dedupe field') {
      new_url = url.replace('remove', 'set');
      text = 'Set as dedupe field';
    }
    else {
      new_url = url.replace('set', 'remove');
      text = 'Remove dedupe field';
      // set all others back to original state
      $('.webform-dedupe').not(this).each(function(){
        $(this).text('Set as dedupe field');
        $(this).attr('href', $(this).attr('href').replace('remove', 'set'));
      });
    }
    // set the new properties of the clicked link
    $(this).attr('href', new_url);
    $(this).text(text);
  });
});