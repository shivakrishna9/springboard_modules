Drupal.behaviors.emailBehavior = function(){
   $current_action = $('#fundraiser-confirmation-settings-form').attr('action');
   if ($current_action != Drupal.settings.basePath + 'email_confirmation/preview_modal') {
    $old_action = $('#fundraiser-confirmation-settings-form').attr('action');
   }


  // Add onmouseover and onmouseout functions to swap form action
  $('#edit-template-wrapper-preview').mouseover(function(){
    //alert('base path:' + Drupal.settings.basePath);
    $('#fundraiser-confirmation-settings-form').get(0).setAttribute('action', Drupal.settings.basePath + 'email_confirmation/preview_modal');
  });

  $('#edit-template-wrapper-preview').mouseout(function(){
    $('#node-form').get(0).setAttribute('action', $old_action);
     $current_action = $('#node-form').attr('action');
  });

  $('#edit-submit').mouseover(function() {
    // This shouldn't be necessary.
    $('#fundraiser-confirmation-settings-form').get(0).setAttribute('action', $old_action);

  });
};

