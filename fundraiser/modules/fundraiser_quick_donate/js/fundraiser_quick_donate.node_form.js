(function($) {
  Drupal.behaviors.fundraiser_quick_donate = {
    attach: function(context, settings) {
      // Return early if no payment processors are available.
      if (undefined == Drupal.settings.fundraiser_quick_donate) {
        return;
      }

      var $gateways = {
        'credit': $('#edit-gateways-credit-id'),
        'bank': $('#edit-gateways-bank-account-id')
      };

      var $gateways_enabled;
      var $quickdonate = $('.form-item-quickdonate');
      var gateway_available = {'credit': true, 'bank': true};

      var gateways_enabled = function() {
        $gateways_enabled = {
          'credit': $('#edit-gateways-credit-status').is(':checked'),
          'bank': $('#edit-gateways-bank-account-status').is(':checked')
        };
      };
      gateways_enabled();

      var disabled = false;
      var wasChecked = $quickdonate.find('input[type=checkbox]').is(':checked');
      var checkGateway = function($this, type) {
        gateways_enabled();

        var $selected = $gateways[type].find('option:selected');
        var $option_name = $selected.text();
        var $option_value = $selected.val();
        var $note = $quickdonate.next('.note');
        if (!$note.length) {
          $note = $('<div/>', {
            class: 'note'
          }).html('<strong>Note:</strong> ');
          $quickdonate.after($note);
        }
        /*var othertype = type == 'credit' ? 'bank' : 'credit';*/
        var unusable = false;
        var partial = false;

        var class_name = $option_value.replace('|', '_');
        if ($gateways_enabled[type] && Drupal.settings.fundraiser_quick_donate.usable_paypment_processors[type].indexOf($option_value) < 0) {
          // Remove any previous notices.
          $note.find('li.' + type).remove();

          if (!$note.children('.invalid-processor').length) {
            $note.append('<span class="invalid-processor">One or more of the payment gateways you have selected are not compatible with or configured for Quick Donate. Donors will not be able to opt-in to Quick Donate when using this payment method: <ul></ul></span>');
          }

          if (!$note.find('li.' + class_name).length) {
            $note.find('ul').append('<li class="' + type + ' ' + class_name + '">' + $option_name + '</li>');
          }

          gateway_available[type] = false;
        }
        else if ($gateways_enabled[type] && Drupal.settings.fundraiser_quick_donate.usable_paypment_processors[type].indexOf($option_value) >= 0) {
          gateway_available[type] = true;
          $note.find('li.' + type).remove();
        }
        else if (!$gateways_enabled[type]) {
          $note.find('li.' + class_name).remove();
          gateway_available[type] = false;
        }

        if (!$gateways_enabled.credit/* && !$gateways_enabled.bank*/) {
          unusable = true;
          if (!$note.children('.unusable').length) {
            $note.append('<span class="unusable">Quick donation functionality is only available when using one of the supported payment processors above.<br/></span>');
          }
        }
        else if (($gateways_enabled.credit && !gateway_available.credit/* && !$gateways_enabled.bank)
          || ($gateways_enabled.bank && !gateway_available.bank && !$gateways_enabled.credit*/)) {
          unusable = true;
        }
        else {
          unusable = false;
          $note.children('.unusable').remove();
        }

        if (unusable
          /*|| $gateways_enabled.credit && !gateway_available.credit && !$gateways_enabled.bank
          || $gateways_enabled.bank && !gateway_available.bank && !$gateways_enabled.credit*/
          /*|| ($gateways_enabled.credit && $gateways_enabled.bank && !gateway_available.credit && !gateway_available.bank)*/) {
          // Disable the QD checkbox and hide all succeeding elements.
          $quickdonate.find('input[type=checkbox]').attr('disabled', 'disabled');
          $quickdonate.nextAll('.form-item, .form-wrapper').hide();
          $('#edit-quickdonate-message-container').hide();

          $note.show();
        }
        else if (!unusable
          || ($gateways_enabled[type] && gateway_available[type]
          /*|| $gateways_enabled[othertype] && gateway_available[othertype]*/)) {
          $quickdonate.find('input[type=checkbox]').removeAttr('disabled');
          if (wasChecked) {
            $quickdonate.find('input[type=checkbox]').attr('checked', wasChecked);
          }
          $quickdonate.nextAll('.form-item, .form-wrapper').show();
          $('#edit-quickdonate-message-container').show();

          if ($gateways_enabled.credit && !gateway_available.credit
            /*|| $gateways_enabled.bank && !gateway_available.bank*/) {
            $note.show();
          }
          else {
            $note.hide();
          }
        }

        if (!$('#edit-quickdonate').is(':checked')) {
          $quickdonate.nextAll('.form-item, .form-wrapper').hide();
          $('#edit-quickdonate-message-container').hide();
        }
      };

      $quickdonate.find('input[type=checkbox]').on('change', function() {
        if ($(this).is(':checked')) {
          wasChecked = true;
        }
      });

      $gateways['credit'].add('#edit-gateways-credit-status').add('#edit-gateways-credit-id').on('change', function() {
        // $quickdonate.next('.note').remove();
        // disabled = false;
        checkGateway(this, 'credit');
      });
      if ($gateways_enabled.credit) {
        checkGateway(this, 'credit');
      }

      /*$gateways['bank'].add('#edit-gateways-bank-account-status').on('change', function() {
        // $quickdonate.next('.note').remove();
        // disabled = false;
        checkGateway(this, 'bank');
      });
      if ($gateways_enabled.bank) {
        checkGateway(this, 'bank');
      }*/
    }
  };
})(jQuery);
