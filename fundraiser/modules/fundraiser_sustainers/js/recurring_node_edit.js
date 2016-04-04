(function($) {
    Drupal.behaviors.recurringNodeEditForm = {
        attach: function (context, settings) {
            // "Are you sure?" dialog pops up when legacy form is set to be upgraded.
            $('input[name*="recurring_preserve_donation_type_checkbox"]').click(function(e) {
                if (!this.checked) {
                    return confirm("Are you sure?\n\nUpgrading this form will remove the \"Recurring Payment\" checkbox and replace it with tabs or radio buttons above the amount. This change cannot be reversed.");
                }
            });
        }
    }
})(jQuery);
