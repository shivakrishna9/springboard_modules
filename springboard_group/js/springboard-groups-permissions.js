
(function ($) {
  Drupal.behaviors.SpringboardGroupPermissionsForm = {
    attach: function(context, settings) {
      var rows = $('table#permissions tr');
      var hidem = false;
      // Hide everything but the rows necessary to administer organization issues.
      rows.each(function(){
        var id = $(this).children('td:first').attr('id');
        if ($(this).children('td:first').hasClass('module') && (id !== 'springboard-group' && id !== 'module-springboard_group' && id !== 'module-og' && id !== 'module-node')) {
          hidem = true;
        }
        else if ($(this).children('td:first').hasClass('module')) {
          hidem = false;
        }
        if(hidem === true) {
          $(this).hide();
        }
        else {
            $(this).children('td:first:contains("new content")').parents('tr').hide();
            $(this).children('td:first:contains("own content")').parents('tr').hide();
            $(this).children('td:first:contains("any content")').parents('tr').hide();
            $(this).children('td:first:contains("Administer content")').parents('tr').hide();
            $(this).children('td:first:contains("revisions")').parents('tr').hide();
            $(this).children('td:first:contains("content overview page")').parents('tr').hide();
            $(this).children('td:first:contains("View published content")').parents('tr').hide();
            $(this).children('td:first:contains("View own unpublished")').parents('tr').hide();
            $(this).children('td:first:contains("Administer content types")').parents('tr').hide();
            $(this).children('td:first:contains("Springboard Group")').parents('tr').show();
        }
      });
    }
  };
})(jQuery);