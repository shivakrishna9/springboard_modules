(function($) {
  Drupal.behaviors.springboard_quick_donate = {
    attach: function(context, settings) {
      // Return early if no payment processors are available.
      if (undefined == Drupal.settings.springboard_quick_donate) {
        return;
      }

      var $gateways = {'credit': $('#edit-gateways-credit-id'), bank: $('#edit-gateways-bank-account-id')};
      var $gateways_enabled;
      var $quickdonate = $('.form-item-quickdonate');
      var gateway_available = {'credit': true, 'bank': true};

      var gateways_enabled = function() {
        $gateways_enabled = {'credit': $('#edit-gateways-credit-status').is(':checked'), 'bank': $('#edit-gateways-bank-account-status').is(':checked')};
      };
      gateways_enabled();

      var checkGateway = function($this, type) {
        $quickdonate.next('.note').remove();

        gateways_enabled();

        var $option_name = $gateways[type].find('option[value="' + $this.value + '"]').text();
        var $option_value = this.value;
        if (undefined == $option_value) {
          $option_name = $gateways[type].find('option:selected').text();
          $option_value = $gateways[type].find('option:selected').val();
        }
        if (!$gateways_enabled.credit && !$gateways_enabled.bank) {
          $quickdonate.after('<div class="note"><strong>Note:</strong> Quick donation functionality is only available when using one of the supported payment processors above.<br/></div>');
          $quickdonate.find('input[type=checkbox]').attr('disabled', 'disabled');
          $quickdonate.nextAll('.form-item, .form-wrapper').hide();
          $('#edit-quickdonate-message-container').hide();
        }
        else if ($gateways_enabled[type] && Drupal.settings.springboard_quick_donate.usable_paypment_processors[type].indexOf($option_value) < 0) {
          // Not a usable credit payment processor, display a note.
          $quickdonate.find('input[type=checkbox]').attr('disabled', 'disabled');
          $quickdonate.nextAll('.form-item, .form-wrapper').hide();
          if ($quickdonate.next('.note').length == 0) {
            $quickdonate.after('<div class="note"><strong>Note:</strong> Quick donations are not available for one or more of the payment processors you have selected: <ul><li>' + $option_name + '</li></ul>You\'ll need to configure the <a href="/admin/commerce/config/payment-methods">different payment methods</a> to utilize the quick donation functionality.<br/><br/></div>');
          }
          else {
            $quickdonate.next('.note').find('ul').append('<li>$option_name</li>');
          }
          gateway_available[type] = false;
        }
        else {
          $quickdonate.find('input[type=checkbox]').removeAttr('disabled');
          $quickdonate.nextAll('.form-item, .form-wrapper').show();
          $quickdonate.next('.note').remove();
          gateway_available[type] = true;
        }

        if (!$('#edit-quickdonate').is(':checked')) {
          $quickdonate.nextAll('.form-item, .form-wrapper').hide();
        }

        // Only hide the quick donation message settings if both processing
        // types are unavailable.
        if (!$gateways_enabled.credit && !$gateways_enabled.bank
          || $gateways_enabled.credit && !gateway_available.credit && !$gateways_enabled.bank
          || $gateways_enabled.bank && !gateway_available.bank && !$gateways_enabled.credit) {
          $('#edit-quickdonate-message-container').hide();
        }
        else {
          $('#edit-quickdonate-message-container').show();
        }
      };

      $gateways['credit'].add('#edit-gateways-credit-status').on('change', function() {
        checkGateway(this, 'credit');
      });
      if ($gateways_enabled.credit) {
        checkGateway(this, 'credit');
      }

      $gateways['bank'].add('#edit-gateways-bank-account-status').on('change', function() {
        checkGateway(this, 'bank');
      });
      if ($gateways_enabled.bank) {
        checkGateway(this, 'bank');
      }
    }
  };
})(jQuery);
