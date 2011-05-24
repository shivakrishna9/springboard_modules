$(document).ready(function(){
   $old_action = $('#node-form').attr('action');

  // Add onmouseover and onmouseout functions to swap form action
  $('#edit-template-wrapper-preview').mouseover(function(){
    $('#node-form').get(0).setAttribute('action', '/jr/email_confirmation/preview_modal');
  });

  $('#edit-template-wrapper-preview').mouseout(function(){
    $('#node-form').get(0).setAttribute('action', $old_action);
  });


});
