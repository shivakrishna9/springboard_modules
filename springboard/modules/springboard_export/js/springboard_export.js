(function ($) {
  Drupal.behaviors.alterSBAActionsView = {
    attach: function (context, settings) {
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
        var downloadButton = $('#sb-queue-csv-download');
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
              'export_params' : Drupal.settings.sbExportActionsData,
              'date_range_min' : $('.form-item-date-filter-min-date input').val(),
              'date_range_max' : $('.form-item-date-filter-max-date input').val(),
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
    }
  };
})(jQuery);
