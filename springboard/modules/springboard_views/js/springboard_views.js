(function ($) {
  Drupal.behaviors.alterSBVDonationsView = {
    attach: function (context, settings) {
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

