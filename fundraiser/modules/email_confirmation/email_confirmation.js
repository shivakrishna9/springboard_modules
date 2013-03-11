Drupal.behaviors.emailBehavior = function(){
  // We have to set the action before the preview button is clicked for ctools modal compatibilty
  $('#fundraiser-confirmation-settings-form').get(0).setAttribute('action', Drupal.settings.basePath + 'email_confirmation/preview_modal');
  // When the standard submit is clicked set it to the current location
  $('#fundraiser-confirmation-settings-form #edit-submit').click(function(){
    $('#fundraiser-confirmation-settings-form').get(0).setAttribute('action', window.location.href);
  });
};
