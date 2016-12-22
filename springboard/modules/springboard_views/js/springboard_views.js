(function ($) {
  Drupal.behaviors.alterSBVDonationsView = {
    attach: function (context, settings) {
      var donationsReportTable = $('.view-sbv-donations table.views-table');
      // Remove collabsible fieldset from donation results tab view:
      $('#views-exposed-form-sbv-donations-page-2').insertBefore('.view-sbv-donations fieldset#webform-ui-exposed-search');
      $('.view-sbv-donations fieldset#webform-ui-exposed-search').remove();

      // Attach help popup to the right of the submit button:
      var submitButton = $('#views-exposed-form-sbv-donations-page .views-submit-button input');
      submitButton.after('<span id="sb-db-donations-report-help"> <i class="fa fa-question-circle-o fa-lg"></i> </span>');
      $('#sb-db-donations-report-help').click(function (e) {
        e.preventDefault();
        if ($('#sb-db-donations-report-help-popup').length) {
          $('#sb-db-donations-report-help-popup').fadeOut('slow', function () {
            $('#sb-db-donations-report-help-popup').remove();
          });
        }
        else {
          $(this).after('<div id="sb-db-donations-report-help-popup" style="display:none;">' +
            'Use this form to view all donation data in the Drupal database.  ' +
            'Click on the columns to sort results or apply filters to the search and hit Go.  ' +
            'Clicking the download button queues export of a CSV file created based on your current seach filters;  ' +
            'you will receive an email alert when your custom report export is available for download.</div>');
          $('#sb-db-donations-report-help-popup').fadeIn('slow');
          $('#sb-db-donations-report-help-popup').click(function () {
            $('#sb-db-donations-report-help-popup').fadeOut('slow', function () {
              $('#sb-db-donations-report-help-popup').remove();
            });
          });
        }
      });

      // Alter date range filter markup:
      $('#views-exposed-form-sbv-donations-page #edit-date-filter-min input, ' +
        '#views-exposed-form-sbv-donations-page-2 #edit-date-filter-min input').after('<i class="fa fa-calendar db-calendar-icon"></i> ' +
        '<span id="db-date-range-separator">and:</span>');
      $('#views-exposed-form-sbv-donations-page #edit-date-filter-max input, ' +
        '#views-exposed-form-sbv-donations-page-2 #edit-date-filter-max input' ).after('<i class="fa fa-calendar db-calendar-icon"></i>');
   
     $('#views-exposed-form-sbv-donations-page #edit-next-charge-min input, ' +
        '#views-exposed-form-sbv-donations-page-2 #edit-next-charge-min input').after('<i class="fa fa-calendar db-calendar-icon"></i> ' +
        '<span id="db-date-range-separator">and:</span>');
      $('#views-exposed-form-sbv-donations-page #edit-next-charge-max input, ' +
        '#views-exposed-form-sbv-donations-page-2 #edit-next-chage-max input' ).after('<i class="fa fa-calendar db-calendar-icon"></i>');
      $('#views-exposed-form-sbv-donations-page i.db-calendar-icon, ' +
        '#views-exposed-form-sbv-donations-page-2 i.db-calendar-icon').click(function () {
        if ($('#ui-datepicker-div').length && !$('#ui-datepicker-div').is(':hidden')) {
          $(this).parent().children('input').blur();
        }
        else {
          $(this).parent().children('input').focus();
          $(this).parent().children('input').click();
        }
      });

      // Rework presentation of the items per page element:
      var itemsPerPage = $('#views-exposed-form-sbv-donations-page #edit-items-per-page, #views-exposed-form-sbv-donations-page-2 #edit-items-per-page');
      itemsPerPage.parent().parent().hide();
      donationsReportTable.before('<div id="sb-db-items-per-page">Showing </div>');
      var optionCount = 0;
      var totalOptions = itemsPerPage.children().length;
      itemsPerPage.children().each(function () {
        var linkElementID = 'db-page-val-' + $(this).val();
        var linkElementValue = $(this).val();
        var pageValue = $(this).val();
        if ($(this).attr('selected') != 'selected') {
          pageLink = '<a href="#" id="' + linkElementID + '">' + linkElementValue + '</a>';
        }
        else {
          pageLink = linkElementValue;
        }
        $('#sb-db-items-per-page').append(pageLink + ' ');
        if ($(this).attr('selected') != 'selected') {
          $('#' + linkElementID).click(function (e) {
            e.preventDefault();
            itemsPerPage.val(linkElementValue);
            $('#views-exposed-form-sbv-donations-page').submit();
          });
        }
        if (optionCount < totalOptions - 1) {
          $('#sb-db-items-per-page').append('| ');
        }
        optionCount++;
      });
      $('#sb-db-items-per-page').append('per page');

      // Alter order status default option label:
      $('#views-exposed-form-sbv-donations-page #edit-status option[value="All"]').text('Order Status');

      // Alter sustainers donation ID (Recurs) filter:
      $('#views-exposed-form-sbv-donations-page #edit-recurs-op option').each(function() {
        if ($(this).text() == 'Regular expression') {
          $(this).text('Any');
          $(this).parent().prepend(($(this)));
        }
        else if ($(this).text() == 'Is empty (NULL)') {
          $(this).text('No');
        }
        else if ($(this).text() == 'Is not empty (NOT NULL)') {
          $(this).text('Yes');
          $(this).parent().append(($(this)));
        }
        else {
          $(this).remove();
        }
      });

      // Display search info as inline field value; hide if the field is used or the form is submitted:
      var searchFilter = $('#views-exposed-form-sbv-donations-page .form-item-combine input');
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
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (searchFilter.val() == searchFilterBlurb) {
          searchFilter.val('');
        }
      });

      // Display internal name info as inline field value; hide if the field is used or the form is submitted:
      var internalFilter = $('#views-exposed-form-sbv-donations-page .form-item-field-fundraiser-internal-name-value input');
      var internalFilterBlurb = 'Internal Name Form';
      if (internalFilter.val() == '') {
        internalFilter.val(internalFilterBlurb);
      } 

      internalFilter.focus(function () {
        if ($(this).val() == internalFilterBlurb) {
          $(this).val('');
        }
      });
      internalFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(internalFilterBlurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (internalFilter.val() == internalFilterBlurb) {
          internalFilter.val('');
        }
      });

      // Display order_id label as field value; hide on change or submit:
      var orderIDFilter = $('#views-exposed-form-sbv-donations-page-2 input#edit-order-id');
      var orderIDBlurb = 'Order ID';
      if (orderIDFilter.val() == '') {
        orderIDFilter.val(orderIDBlurb);
      }
      orderIDFilter.focus(function () {
        if ($(this).val() == orderIDBlurb) {
          $(this).val('');
        }
      });
      orderIDFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(orderIDBlurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page-2').submit(function () {
        if (orderIDFilter.val() == orderIDBlurb) {
          orderIDFilter.val('');
        }
      });

      // Display sid label as field value; hide on change or submit:
      var submIDFilter = $('#views-exposed-form-sbv-donations-page-2 input#edit-sid');
      var submIDBlurb = 'Submission ID';
      if (submIDFilter.val() == '') {
        submIDFilter.val(submIDBlurb);
      }
      submIDFilter.focus(function () {
        if ($(this).val() == submIDBlurb) {
          $(this).val('');
        }
      });
      submIDFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(submIDBlurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page-2').submit(function () {
        if (submIDFilter.val() == submIDBlurb) {
          submIDFilter.val('');
        }
      });

      // User ID inline label behavior:
      var subID2Filter = $('#views-exposed-form-sbv-donations-page-2 input#edit-uid-raw');
      var subID2Blurb = 'User ID';
      if (subID2Filter.val() == '') {
        subID2Filter.val(subID2Blurb);
      } 
      subID2Filter.focus(function () {
        if ($(this).val() == subID2Blurb) {
          $(this).val('');
        }
      });
      subID2Filter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(subID2Blurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page-2').submit(function () {
        if (subID2Filter.val() == subID2Blurb) {
          subID2Filter.val('');
        }
      });

      // Email inline label behavior:
      var subID3Filter = $('#views-exposed-form-sbv-donations-page-2 input#edit-mail');
      var subID3Blurb = 'Email Address';
      if (subID3Filter.val() == '') {
        subID3Filter.val(subID3Blurb);
      }
      subID3Filter.focus(function () {
        if ($(this).val() == subID3Blurb) {
          $(this).val('');
        }
      });
      subID3Filter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(subID3Blurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page-2').submit(function () {
        if (subID3Filter.val() == subID3Blurb) {
          subID3Filter.val('');
        }
      });

      // Date picker inline labels:
      var subID4Filter = $('#views-exposed-form-sbv-donations-page-2 input#edit-date-filter-min-datepicker-popup-0');
      var subID4Blurb = 'mm/dd/yy';
      if (subID4Filter.val() == '') {
        subID4Filter.val(subID4Blurb);
      }
      subID4Filter.focus(function () {
        if ($(this).val() == subID4Blurb) {
          $(this).val('');
        }
      });
      subID4Filter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(subID4Blurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page-2').submit(function () {
        if (subID4Filter.val() == subID4Blurb) {
          subID4Filter.val('');
        }
      });
      var subID5Filter = $('#views-exposed-form-sbv-donations-page-2 input#edit-date-filter-max-datepicker-popup-0');
      var subID5Blurb = 'mm/dd/yy';
      if (subID5Filter.val() == '') {
        subID5Filter.val(subID5Blurb);
      } 
      subID5Filter.focus(function () { 
        if ($(this).val() == subID5Blurb) {
          $(this).val('');
        }
      });
      subID5Filter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(subID5Blurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page-2').submit(function () {
        if (subID5Filter.val() == subID5Blurb) {
          subID5Filter.val('');
        }
      });

      var subIDZFilter = $('#views-exposed-form-sbv-donations-page input#edit-date-filter-min-datepicker-popup-0');
      var subIDZBlurb = 'mm/dd/yy';
      if (subIDZFilter.val() == '') {
        subIDZFilter.val(subIDZBlurb);
      }
      subIDZFilter.focus(function () {
        if ($(this).val() == subIDZBlurb) {
          $(this).val('');
        }
      });
      subIDZFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(subIDZBlurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (subIDZFilter.val() == subIDZBlurb) {
          subIDZFilter.val('');
        }
      });
      var subIDXFilter = $('#views-exposed-form-sbv-donations-page input#edit-date-filter-max-datepicker-popup-0');
      var subIDXBlurb = 'mm/dd/yy';
      if (subIDXFilter.val() == '') {
        subIDXFilter.val(subIDXBlurb);
      }
      subIDXFilter.focus(function () {
        if ($(this).val() == subIDXBlurb) {
          $(this).val('');
        }
      });
      subIDXFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(subIDXBlurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (subIDXFilter.val() == subID5Blurb) {
          subIDXFilter.val('');
        }
      });

      // Next charge date filter
      var nextChMinFilter = $('#views-exposed-form-sbv-donations-page input#edit-next-charge-min-datepicker-popup-0');
      var nextChMinFilterBlurb = 'mm/dd/yy';
      if (nextChMinFilter.val() == '') {
        nextChMinFilter.val(nextChMinFilterBlurb);
      }
      nextChMinFilter.focus(function () {
        if ($(this).val() == nextChMinFilterBlurb) {
          $(this).val('');
        }
      });
      nextChMinFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(nextChMinFilterBlurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (nextChMinFilter.val() == nextChMinFilterBlurb) {
          nextChMinFilter.val('');
        }
      });
      var nextChMaxFilter = $('#views-exposed-form-sbv-donations-page input#edit-next-charge-max-datepicker-popup-0');
      var nextChMaxFilterBlurb = 'mm/dd/yy';
      if (nextChMaxFilter.val() == '') {
        nextChMaxFilter.val(nextChMaxFilterBlurb);
      }
      nextChMaxFilter.focus(function () {
        if ($(this).val() == nextChMaxFilterBlurb) {
          $(this).val(''); 
        }
      });
      nextChMaxFilter.blur(function () {
        if ($(this).val() == '') {
          $(this).val(nextChMaxFilterBlurb);
        } 
      }); 
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (nextChMaxFilter.val() == nextChMaxFilterBlurb) {
          nextChMaxFilter.val('');
        }   
      });

      var nextChMinFilter2 = $('#views-exposed-form-sbv-donations-page input#edit-next-charge-min-datepicker-popup-0');
      var nextChMinFilter2Blurb = 'mm/dd/yy';
      if (nextChMinFilter2.val() == '') {
        nextChMinFilter2.val(nextChMinFilter2Blurb);
      }
      nextChMinFilter2.focus(function () {
        if ($(this).val() == nextChMinFilter2Blurb) {
          $(this).val('');
        }
      });
      nextChMinFilter2.blur(function () {
        if ($(this).val() == '') {
          $(this).val(nextChMinFilter2Blurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (nextChMinFilter2.val() == nextChMinFilter2Blurb) {
          nextChMinFilter2.val('');
        }
      });
      var nextChMaxFilter2 = $('#views-exposed-form-sbv-donations-page input#edit-next-charge-max-datepicker-popup-0');
      var nextChMaxFilter2Blurb = 'mm/dd/yy';
      if (nextChMaxFilter2.val() == '') {
        nextChMaxFilter2.val(nextChMaxFilter2Blurb);
      }
      nextChMaxFilter2.focus(function () {
        if ($(this).val() == nextChMaxFilter2Blurb) {
          $(this).val('');
        }
      });
      nextChMaxFilter2.blur(function () {
        if ($(this).val() == '') {
          $(this).val(nextChMaxFilter2Blurb);
        }
      });
      $('#views-exposed-form-sbv-donations-page').submit(function () {
        if (nextChMaxFilter2.val() == nextChMaxFilter2Blurb) {
          nextChMaxFilter2.val('');
        }
      });
      // End next charge date filter



      // Re-position the download link and add ajax callback when it is clicked; hide on no results:
      $(document).ready(function() {
        var downloadButton = $('.view-sbv-donations a.views-data-export');
        $('#sb-db-items-per-page').before('<span id="sbv-export-download-msg"></span>');
        downloadButton.click(function (e) {
          $('#sbv-export-download-msg').text('Working...');
          // Gather data:
          var viewParams = {};
          viewParams['status'] = $('#views-exposed-form-sbv-donations-page #edit-status').val();
          downloadButton.hide();
          e.preventDefault();
          var isRecurringValue = $('.form-item-recurs-op #edit-recurs-op').val();
          if (isRecurringValue == 'not empty') {
            isRecurringValue = 'yes'; 
          } else if (isRecurringValue == 'empty') {
            isRecurringValue = 'no'; 
          } else {
            isRecurringValue = ''; 
          }

          $.ajax({
            type: 'POST',
            url: '/springboard-export-queue-ajax',
            dataType: 'json',
            data: {
              'export_params' : Drupal.settings.sbExposedFilters,
              'date_range_min' : $('.form-item-date-filter-min-date input').val(),
              'date_range_max' : $('.form-item-date-filter-max-date input').val(),
              'next_charge_min' : $('.form-item-next-charge-min-date input').val(),
              'next_charge_max' : $('.form-item-next-charge-max-date input').val(),
              'is_recurring' : isRecurringValue,
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
        donationsReportTable.before(downloadButton);
        if ($('.view-empty').length) {
          downloadButton.hide();
        }
      });

      // Re-position the reset button:
      var submitButton = $('#views-exposed-form-sbv-donations-page .views-submit-button');
      var resetButton = $('#views-exposed-form-sbv-donations-page .views-reset-button');
      submitButton.before(resetButton);

      // Alter the results table action links:
      $('.view-sbv-donations table.views-table tbody tr').each(function () {
        var row = $(this);
        // Update actions:
        var status = row.children('.views-field-status').text();
        status = $.trim(status);
        if (status != 'Payment Received') {
          row.find('.table-actions .action-refund-link').hide();
          row.find('.table-actions .bar').hide();
        }

        // Link Yes/No recur value:
        order_id = row.find('.views-field-order-id-1 a').text();
        order_id = $.trim(order_id);
        recursField = row.children('.views-field-sustainer-key')
        recurs = $.trim(recursField.text());
        if (recurs == 'Yes') {
          recursField.html('<a href="/springboard/donations/' + order_id + '/recurring">Yes</a>');
        }
        else {
          recursField.html('<a href="/springboard/donations/' + order_id + '/payment">No</a>');
        }
      });
    }
  };
})(jQuery);

