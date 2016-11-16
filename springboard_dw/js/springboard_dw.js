/**
 * @file
 * JS for Springbaord Data Warehouse views.
 */

Drupal.behaviors.springboardDataWarehouseViews = {
  attach: function(context) { (function($) {
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

    // Add a description to the keyword search exposed filter's value until somebody types or the form is submitted:
    var searchFilter = $('#views-exposed-form-springboard-dw-donations-report-page .form-item-dw-global-search input');
    var searchFilterBlurb = 'Search for report values here';
    if (searchFilter.val() == '') {
      searchFilter.val(searchFilterBlurb);
    }
    searchFilter.focus(function () {
      if ($(this).val() == searchFilterBlurb) {
        $(this).val('');
      }
    });
    searchFilter.blur(function () {
      if ($(this).val() == '') {
        $(this).val(searchFilterBlurb);
      }
    });
    $('#views-exposed-form-springboard-dw-donations-report-page').submit(function () {
      if (searchFilter.val() == searchFilterBlurb) {
        searchFilter.val('');
      }
    });

    // Inline date picker label behavior:
    var subIDAFilter = $('#views-exposed-form-springboard-dw-donations-report-page input#edit-transaction-date-min-datepicker-popup-0');
    var subIDABlurb = 'mm/dd/yy';
    if (subIDAFilter.val() == '') {
      subIDAFilter.val(subIDABlurb);
    } 
    subIDAFilter.focus(function () { 
      if ($(this).val() == subIDABlurb) {
        $(this).val('');
      }
    });
    subIDAFilter.blur(function () {
      if ($(this).val() == '') {
        $(this).val(subIDABlurb);
      }
    });
    $('#views-exposed-form-springboard-dw-donations-report-page').submit(function () {
      if (subIDAFilter.val() == subIDABlurb) {
        subIDAFilter.val('');
      }
    });
    var subIDBFilter = $('#views-exposed-form-springboard-dw-donations-report-page input#edit-transaction-date-max-datepicker-popup-0');
    var subIDBBlurb = 'mm/dd/yy';
    if (subIDBFilter.val() == '') {
      subIDBFilter.val(subIDBBlurb);
    }
    subIDBFilter.focus(function () {
      if ($(this).val() == subIDBBlurb) {
        $(this).val('');
      }
    });
    subIDBFilter.blur(function () {
      if ($(this).val() == '') {
        $(this).val(subIDBBlurb);
      }
    });
    $('#views-exposed-form-springboard-dw-donations-report-page').submit(function () {
      if (subIDBFilter.val() == subIDBBlurb) {
        subIDBFilter.val('');
      }
    });


  })(jQuery); }
}
