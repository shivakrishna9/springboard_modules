/**
 * @file
 * JS for Springbaord Data Warehouse views.
 */

Drupal.behaviors.springboardDataWarehouseViews = {
  attach: function(context) { (function($) {
    // Place filters within the advanced or basic filter fieldsets:
    var filterGroups = $('#edit-filter-groups');
    $('#views-exposed-form-springboard-dw-donations-report-page').prepend(filterGroups);
    $('.views-exposed-widget').each(function () {
      if ($(this).hasClass('views-widget-filter-donation_id') || $(this).hasClass('views-widget-filter-email')
        || $(this).hasClass('views-widget-filter-transaction_date') || $(this).hasClass('views-submit-button') 
        || $(this).hasClass('views-reset-button')) {
        filterGroups.find('#edit-basic-filters .fieldset-wrapper').append($(this));
      }
      else {
        filterGroups.find('#edit-advanced-filters .fieldset-wrapper').append($(this));
      }
    });

    // Expand the advanced filters field set if any of its values are populated:
    $('#edit-advanced-filters .views-exposed-widget input.form-text').each(function () {
      if ($(this).val() != '') {
        $('#edit-advanced-filters').removeClass('collapsed');
        $('#edit-advanced-filters .fieldset-wrapper').show();
        
        return false;
      }
    });

    // Attach help popup to the right of the submit button:
    var submitButton = $('#views-exposed-form-springboard-dw-donations-report-page .views-submit-button input');
    var donationsReportTable = $('.view-springboard-dw-donations-report table.views-table');
    submitButton.after('<span id="sb-dw-donations-report-help"> <i class="fa fa-question-circle-o fa-lg"></i> </span>');
    $('#sb-dw-donations-report-help').click(function (e) {
      e.preventDefault();
      if ($('#sb-dw-donations-report-help-popup').length) {
        $('#sb-dw-donations-report-help-popup').fadeOut('slow', function () {
          $('#sb-dw-donations-report-help-popup').remove();
        });
      }
      else {
        $(this).after('<div id="sb-dw-donations-report-help-popup" style="display:none;">' +
          'Use this form to view all donation data in the warehouse.  ' +
          'Click on the columns to sort results or apply filters to the search and hit Go.  ' +
          'Please note that it take up to 15 minutes for website donation data to appear in the data warehouse.</div>');
        $('#sb-dw-donations-report-help-popup').fadeIn('slow');
        $('#sb-dw-donations-report-help-popup').click(function () {
          $('#sb-dw-donations-report-help-popup').fadeOut('slow', function () {
            $('#sb-dw-donations-report-help-popup').remove();
          });
        });
      }
    });

    // Alter date range filter markup:
    $('#views-exposed-form-springboard-dw-donations-report-page #edit-transaction-date-min input').after('<i class="fa fa-calendar dw-calendar-icon"></i> ' +
      '<span id="dw-date-range-separator">and:</span>');
    $('#views-exposed-form-springboard-dw-donations-report-page #edit-transaction-date-max input').after('<i class="fa fa-calendar dw-calendar-icon"></i>');
    $('#views-exposed-form-springboard-dw-donations-report-page i.dw-calendar-icon').click(function () {
      if ($('#ui-datepicker-div').length && !$('#ui-datepicker-div').is(':hidden')) {
        $(this).parent().children('input').blur();
      }
      else {
        $(this).parent().children('input').focus();
        $(this).parent().children('input').click();
      }
    });

    // Rework presentation of the items per page element:
    var itemsPerPage = $('#views-exposed-form-springboard-dw-donations-report-page #edit-items-per-page');
    itemsPerPage.parent().parent().hide();
    donationsReportTable.before('<div id="sb-dw-items-per-page">Showing </div>');
    var optionCount = 0;    
    var totalOptions = itemsPerPage.children().length;
    itemsPerPage.children().each(function () {
      var linkElementID = 'dw-page-val-' + $(this).val();
      var linkElementValue = $(this).val();
      var pageValue = $(this).val();
      if ($(this).attr('selected') != 'selected') {
        pageLink = '<a href="#" id="' + linkElementID + '">' + linkElementValue + '</a>';
      }
      else {
        pageLink = linkElementValue;
      }
      $('#sb-dw-items-per-page').append(pageLink + ' ');
      if ($(this).attr('selected') != 'selected') {
        $('#' + linkElementID).click(function (e) {
          e.preventDefault();
          itemsPerPage.val(linkElementValue);
          $('#views-exposed-form-springboard-dw-donations-report-page').submit();
        });
      }
      if (optionCount < totalOptions - 1) {
        $('#sb-dw-items-per-page').append('| ');
      }
      optionCount++;
    });
    $('#sb-dw-items-per-page').append('per page');

    // Add inline labels; this displays exposed filter titles as filter content until user starts typing:
    $('#views-exposed-form-springboard-dw-donations-report-page .views-exposed-widget').each(function () {
      if ($(this).hasClass('views-widget-filter-transaction_date')) {
        return;
      }
      $(this).children('label').hide();
      var widgetLabel = $(this).children('label').text();
      widgetLabel = $.trim(widgetLabel);
      var textField = $(this).find('input.form-text');
      textField.attr('label_value', widgetLabel);
      if (textField.val() == '') {
        textField.val(widgetLabel);
      }
      textField.focus(function () {
        if ($(this).val() == $(this).attr('label_value')) {
          $(this).val('');
        }
      });
      textField.blur(function () {
        if ($(this).val() == '') {
          $(this).val($(this).attr('label_value'));
        }
      });
    });
    $('#views-exposed-form-springboard-dw-donations-report-page').submit(function () {
      $('#views-exposed-form-springboard-dw-donations-report-page .views-exposed-widget').each(function () {
        var textField = $(this).find('input.form-text');
        if (textField.val() == textField.attr('label_value')) {
          textField.val('');
        }
      });
    });

    // Inline date picker label behavior:
    var dateLabel = 'mm/dd/yy';
    $('#views-exposed-form-springboard-dw-donations-report-page input#edit-transaction-date-min-datepicker-popup-0,' +
      '#views-exposed-form-springboard-dw-donations-report-page input#edit-transaction-date-max-datepicker-popup-0').each(function () {
      if ($(this).val() == '') {
        $(this).val(dateLabel);
      }
      $(this).focus(function () {
        if ($(this).val() == dateLabel) {
          $(this).val('');
        }
      });
      $(this).blur(function () {
        if ($(this).val() == '') {
          $(this).val(dateLabel);
        }
      });
    });
    $('#views-exposed-form-springboard-dw-donations-report-page').submit(function () {
      $('#views-exposed-form-springboard-dw-donations-report-page input#edit-transaction-date-min-datepicker-popup-0,' +
        '#views-exposed-form-springboard-dw-donations-report-page input#edit-transaction-date-max-datepicker-popup-0').each(function () {
        if ($(this).val() == dateLabel) {
          $(this).val('');
        }
      });
    });

    // Add CSV download button
    $('.view-springboard-dw-donations-report .views-table').before(
      '<a href="#" id="sb-queue-csv-download" class="views-data-export"><span class="img-caption">Download as .CSV</span></a>'
    );
    $('.view-springboard-dw-donations-report .views-table caption').insertBefore('#sb-queue-csv-download');

    // Add AJAX CSV Download queue behavior:
    $(document).ready(function() {
      var downloadButton = $('#sb-queue-csv-download');
      downloadButton.after('<span id="sbv-export-download-msg"></span>');
      downloadButton.click(function (e) {
        $('#sbv-export-download-msg').text('Working...');
        // Gather data:
        var viewParams = {};
        // viewParams['status'] = $('#views-exposed-form-sbv-donations-page #edit-status').val();
        downloadButton.hide();
        e.preventDefault();
        $.ajax({
          type: 'POST',
          url: '/sb-dw-export-queue-ajax',
          dataType: 'json',
          data: {
            'export_params' : Drupal.settings.sbDWExportDonationsData,
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
  })(jQuery); }
}
