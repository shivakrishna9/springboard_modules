(function ($) {

    Drupal.theme.prototype.fundraiserUpsellModal = function () {
        var html = ''
        html += '  <div id="ctools-modal">';
        html += '    <div class="ctools-modal-content ' + Drupal.settings['fundraiser-upsell'].modalClass + '">'; // panels-modal-content
        html += '      <div class="modal-header">';
        html += '        <span id="modal-title" class="modal-title">&nbsp;</span>';
        html += '      </div>';
        html += '      <div id="modal-content" class="modal-content">';
        html += '      </div>';
        html += '    </div>';
        html += '  </div>';

        return html;
    };

    $('document').ready(function() {
       $('.ctools-modal-fundraiser-upsell').click();
    });

})(jQuery);


(function ($) {
    Drupal.behaviors.fundraiser_upsell = {
        attach: function (context, settings) {
            $('.ctools-close-modal.rejection', context).click(function() {
                // Get the days for the rejection cookie
                var rejectionDays = Drupal.settings['fundraiser-upsell'].rejectionDays;
                var exdate=new Date();
                exdate.setDate(exdate.getDate() + rejectionDays);
                var c_value = "1;path=/" + ((rejectionDays==null) ? "" : ";expires="+exdate.toUTCString());
                document.cookie='fundraiser_upsell_rejection' + "=" + c_value;
            });

            $('.ctools-modal-content form', context).on('submit', function() {
               $('.ctools-close-modal', context).hide();
            });
        }
    }

})(jQuery);
