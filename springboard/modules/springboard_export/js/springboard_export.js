(function ($) {
  Drupal.behaviors.alterSBAActionsView = {
    attach: function (context, settings) {
      // Remove collabsible fieldset from actions results tab view:
      $('#views-exposed-form-sba-action-submissions-page-1').insertBefore('.view-id-sba_action_submissions fieldset#webform-ui-exposed-search');
      $('.view-id-sba_action_submissions fieldset#webform-ui-exposed-search').remove();

      // Attach the download link and remove the default views data export link:
      $('.view-sba-action-submissions .feed-icon a').each(function () {
        if ($(this).attr('href').indexOf('export') >= 0) {
          $(this).parent().hide();
        }
      });
      $('.view-sba-action-submissions .views-table').before(
        '<a href="#" id="sb-queue-csv-download" class="views-data-export"><span class="img-caption">Download as .CSV</span></a>'
      );
      $('.view-sba-action-submissions .views-table caption').insertBefore('#sb-queue-csv-download');

    // Add AJAX CSV Download queue behavior:
      $(document).ready(function() {
        
        var downloadButton = $('.view-sbv-donations #sb-queue-csv-download');
        downloadButton.before('<span id="sbv-export-download-msg"></span>');
        downloadButton.click(function (e) {
          $('#sbv-export-download-msg').text('Working...');
          // Gather data:
          var viewParams = {};
          // viewParams['status'] = $('#views-exposed-form-sbv-donations-page #edit-status').val();
          downloadButton.hide();
          e.preventDefault();
          $.ajax({
            type: 'POST',
            url: '/springboard-export-queue-ajax',
            dataType: 'json',
            data: {
              'export_params' : Drupal.settings.sbExposedFilters,
              'recipient' : Drupal.settings.sbRecipient
            },
            context: document.body,
            success: function(data) {
              if (data.status == 'success') {
                $('#sbv-export-download-msg').text('Your export has been queued; you will be emailed a download link when it is ready.');
              }
              else {
                $('#sbv-export-download-msg').text('Export failed.');
                alert('There was an error; the export file was not generated.');
              }
            }
          });
        });
        if ($('.view-empty').length) {
          downloadButton.hide();
        }
      });
      // Add inline filter values:
      $('#views-exposed-form-sba-action-submissions-page .views-exposed-widget').each(function () {
        var textFilter = $(this).find('.form-type-textfield input');
        $(this).find('label').hide();
        var textFilterBlurb = $(this).find('label').text().trim();
        if (textFilter.val() == '' && textFilterBlurb != '') {
          textFilter.val(textFilterBlurb);
        }
        textFilter.focus(function () {
          if ($(this).val() == textFilterBlurb) {
            $(this).val('');
          }
        });
        textFilter.blur(function () {
          if ($(this).val() == '') {
            $(this).val(textFilterBlurb);
          }
        });
        $('#views-exposed-form-sba-action-submissions-page').submit(function () {
          if (textFilter.val() == textFilterBlurb) {
            textFilter.val('');
          }
        });
      });
    }
  };
})(jQuery);
