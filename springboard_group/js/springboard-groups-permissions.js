(function ($) {
  Drupal.behaviors.SpringboardGroupPermissionsForm = {
    attach: function (context, settings) {
      if ($.cookie('Drupal.groupPermissions.showHidden') == 0 || $.cookie('Drupal.groupPermissions.showHidden') === null) {
        $('table#permissions').before('<a href="#" class="show-content-type show-hidden-fields-off">Show Content type permissions</a>');
      }
      else {
        $('table#permissions').before('<a href="#" class="show-content-type show-hidden-fields-on">Hide Content type permissions</a>');
      }
      $('.show-content-type').css('float','right');
      $('.show-content-type').click(function() {
        if ($(this).hasClass('show-hidden-fields-on')) {
          $(this).removeClass('show-hidden-fields-on').addClass('show-hidden-fields-off').text('Show content type permssions');
          $.cookie('Drupal.groupPermissions.showHidden', 0, {
            path: Drupal.settings.basePath,
            // The cookie expires in one year.
            expires: 365
          });
        }
        else {
          $(this).removeClass('show-hidden-fields-off').addClass('show-hidden-fields-on').text('Hide Content type permissions');
          $.cookie('Drupal.groupPermissions.showHidden', 1, {
            path: Drupal.settings.basePath,
            // The cookie expires in one year.
            expires: 365
          });
          console.log($.cookie('Drupal.groupPermissions.showHidden'));

        }

        $('table#permissions tr').each(function(){
          if ($(this).hasClass('hidden-component-off')) {
            $(this).removeClass('hidden-component-off').addClass('hidden-component-on');
            $(this).children('td:first:contains("new content")').parents('tr').show();
            $(this).children('td:first:contains("own content")').parents('tr').show();
            $(this).children('td:first:contains("any content")').parents('tr').show();
          }
          else {
            $(this).removeClass('hidden-component-on').addClass('hidden-component-off');
            $(this).children('td:first:contains("new content")').parents('tr').hide();
            $(this).children('td:first:contains("own content")').parents('tr').hide();
            $(this).children('td:first:contains("any content")').parents('tr').hide();
          }
        });
        return false;
      });
      var rows = $('table#permissions tr');
      var hidem = false;
      // Hide everything but the rows necessary to administer organization issues.
      rows.each(function () {
        var id = $(this).children('td:first').attr('id');
        var allow = [
          'springboard-group',
          'module-springboard_group',
          'module-og',
          'module-node',
          'module-publishcontent',
          'module-rules',
          'module-commerce_payment',
          'module-springboard_target',
          'module-contextual',
          'module-filter',
        ];
        if ($(this).children('td:first').hasClass('module') && $.inArray(id, allow) === -1) {
          hidem = true;
        }
        else if ($(this).children('td:first').hasClass('module')) {
          hidem = false;
        }
        if (hidem === true) {
          $(this).hide();
        }
        else {
          console.log($.cookie('Drupal.groupPermissions.showHidden'));
          if ($.cookie('Drupal.groupPermissions.showHidden') == 0 || $.cookie('Drupal.groupPermissions.showHidden') === null) {
            $(this).children('td:first:contains("new content")').parents('tr').hide().addClass('hidden-component-off');
            $(this).children('td:first:contains("own content")').parents('tr').hide().addClass('hidden-component-off');
            $(this).children('td:first:contains("any content")').parents('tr').hide().addClass('hidden-component-off');
          }
          $(this).children('td:first:contains("Administer content")').parents('tr').hide();
          $(this).children('td:first:contains("revisions")').parents('tr').hide();
          $(this).children('td:first:contains("content overview page")').parents('tr').hide();
          $(this).children('td:first:contains("View published content")').parents('tr').hide();
          $(this).children('td:first:contains("Access the Rules debug log")').parents('tr').hide();
          $(this).children('td:first:contains("Administer text format")').parents('tr').hide();
          $(this).children('td:first:contains("payments")').parents('tr').hide();
          $(this).children('td:first:contains("Use Rules component add_og_membership")').parents('tr').hide();
          $(this).children('td:first:contains("View own unpublished")').parents('tr').hide();
          $(this).children('td:first:contains("Administer content types")').parents('tr').hide();
          $(this).children('td:first:contains("Springboard Group")').parents('tr').show();
        }
      });
    }
  };
})(jQuery);