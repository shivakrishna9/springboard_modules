/**
 * @file
 * JS for Springbaord Data Warehouse views.
 */

Drupal.behaviors.springboardDataWarehouseViews = {
  attach: function(context) { (function($) {
    // Attach help popup to the right of the submit button:
    var submitButton = $('#views-exposed-form-springboard-dw-donations-report-page .views-submit-button input');
    var donationsReportTable = $('.view-springboard-dw-donations-report table.views-table');
    submitButton.after('<span id="sb-dw-donations-report-help"> ? </span>');
    $('#sb-dw-donations-report-help').click(function (e) {
      e.preventDefault();
      donationsReportTable.before('<div id="sb-dw-donations-report-help-text">' +
        'Use this form to search archived Donation data.  Please note that it take up to 16 minutes for website donation data to appear in this archive.</div>');
      $(this).hide();
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
  })(jQuery); }
}
