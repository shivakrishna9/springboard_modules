(function ($) {

  Drupal.behaviors.emailWrappersModule = {
    attach: function (context, settings) {
      $('select[name=email_wrappers_email_template]', context).mouseover(function () {
        $val = $(this).val();
        $error = $('#template-overwrite-warning').val();
        if ($val != 0 && typeof $error == 'undefined') {
          $(this).after('<div id="template-overwrite-warning" class="messages error">' +
        'Warning: changing the selected template will overwrite any changes you have made. ' +
        '</div>');
        }
      });
    }
  };

})(jQuery);
