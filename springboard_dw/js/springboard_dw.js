/**
 * @file
 * JS for Springbaord Data Warehouse views.
 */

Drupal.behaviors.springboardDataWarehouseViews = {
  attach: function(context) { (function($) {
    // Attach help popup to the right of the submit button:
    var submitButton = $('#views-exposed-form-springboard-dw-donations-report-page .views-submit-button input');
    submitButton.after('<span id="sb-dw-donations-report-help"> ? </span>');
    $('#sb-dw-donations-report-help').click(function (e) {
      e.preventDefault();
      $('.view-springboard-dw-donations-report table.views-table').before('<div id="sb-dw-donations-report-help-text">' +
        'Use this form to search archived Donation data.  Please note that it take up to 16 minutes for website donation data to appear in this archive.</div>');
      $(this).hide();
    });

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
