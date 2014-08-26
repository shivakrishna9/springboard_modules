/**
 * @file
 * JS for the P2P campaign node add/edit form.
 */
(function ($) {
    // Add drupal 7 code.
    Drupal.behaviors.p2pCampaign = {
        attach: function (context, settings) {
            $('.springboard_p2p_campaign_goal_table_fundraiser input[type=checkbox]', context).change(function(e) {
                var thisCheck = $(this);
                if (thisCheck.is(':checked')) {
                    $('#node_p2p_campaign_form_group_p2p_donation_amount', context).show();
                }
            });

            $('.springboard_p2p_campaign_goal_table_webform_user input[type=checkbox]', context).change(function(e) {
                var thisCheck = $(this);
                if (thisCheck.is(':checked')) {
                    $('#node_p2p_campaign_form_group_p2p_donation_amount', context).hide();
                }
            });
        }
    };
})
(jQuery);
