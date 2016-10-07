(function ($) {
  Drupal.behaviors.SpringboardTokenSetFieldsAdmin = {
    attach: function (context, settings) {
      var submitButton = $('#springboard-token-set-fields-admin-form #edit-token-add');
      var formSelect = $('#springboard-token-set-fields-admin-form #edit-target-form-id');
      var fieldSelect = $('#springboard-token-set-fields-admin-form #edit-field-key');
      var tokenSetSelect = $('#springboard-token-set-fields-admin-form #edit-token-set');
      fieldSelect.attr('disabled', 'disabled');
      fieldSelect.parent().addClass('form-disabled');
      tokenSetSelect.attr('disabled', 'disabled');
      tokenSetSelect.parent().addClass('form-disabled');
      submitButton.hide();
      formSelect.keyup(function () {
        if ($(this).val() == '') {
          fieldSelect.attr('disabled', 'disabled');
          fieldSelect.parent().addClass('form-disabled');
          fieldSelect.val('');
          tokenSetSelect.attr('disabled', 'disabled');
          tokenSetSelect.parent().addClass('form-disabled');
          tokenSetSelect.val('');
          submitButton.hide();
        }
        else {
          fieldSelect.removeAttr('disabled');
          fieldSelect.parent().removeClass('form-disabled');
          if (fieldSelect.val() != '' && tokenSetSelect.val() > 0) {
            submitButton.show();
          } 
        }
      });
      fieldSelect.keyup(function () {
        if ($(this).val() == '') {
          tokenSetSelect.attr('disabled', 'disabled');
          tokenSetSelect.parent().addClass('form-disabled');
          tokenSetSelect.val('');
          submitButton.hide();
        }
        else {
          tokenSetSelect.removeAttr('disabled');
          tokenSetSelect.parent().removeClass('form-disabled');
          if (fieldSelect.val() != '' && tokenSetSelect.val() > 0) {
            submitButton.show();
          }          
        }
      });
      tokenSetSelect.change(function () {
        if (tokenSetSelect.val() == 0 || !tokenSetSelect.val()) {
          submitButton.hide();
        }
        else {
          submitButton.show();
        }
      });
      $('#springboard-token-set-fields-admin-form').submit(function (e) {
        if (formSelect.val() == '' || fieldSelect.val() == '' || tokenSetSelect.val() == '0' || !tokenSetSelect.val()) {
          e.preventDefault();
        }
      });
      $('#sb-token-fields-list .sb-tsid-remove').click(function (e) {
        var parentRow = $(this).parent().parent();
        e.preventDefault();
        $.post(
          '/sb-admin-token-fields-remove-ajax', {
            'tsid' : $(this).attr('tsid'),
            'field_name' : $(this).attr('field_name'),
            'form_id' : $(this).attr('form_id')
          },
          function(data) {
            if (data != 'success') {
              alert(data);
            }
            else {
              parentRow.remove(); 
            }
          }
        );
      });
    }
  }; // End SpringboardTokenSetFieldsAdmin

  Drupal.behaviors.SpringboardTokenSetAdmin = {
    attach: function (context, settings) {
      function _sbTokenSetManagerRemoveToken(delLink) {
        $.post(
          '/sb-admin-token-delete-ajax', {
            'tsid' : delLink.attr('tsid'),
            'tid' : delLink.attr('tid')
          },
          function(data) {
            if (data != 'success') {
              alert(data);
            }
            else {
              delLink.parent().remove();
            }
          }
        );
      }
      $('#springboard-token-set-sets-admin-form .form-type-textfield').keydown(function(e) {
        if(e.keyCode == 13) {
          e.preventDefault();
          return false;
        }
      });
      $('#springboard-token-set-sets-admin-form .remove-token-link').click(function (e) {
        e.preventDefault();
        _sbTokenSetManagerRemoveToken($(this));
      });
      $('#springboard-token-set-sets-admin-form .add-token-button').click(function (e) {
        var submitButton = $(this);
        e.preventDefault();
        $.post(
          '/sb-admin-token-add-ajax', {
            'tsid' : $(this).attr('tsid'),
            'token_machine' : $(this).parent().find('.token-machine').val(),
            'token_descr' : $(this).parent().find('.token-description').val()
          },
          function(data) {
            if (!$.isNumeric(data)) {
              alert(data);
            }
            else {
              var tsid = submitButton.attr('tsid');
              var tid = data;
              var tokenMachine = submitButton.parent().find('.token-machine').val();
              submitButton.parent().before(
                '<div class="form-wrapper">' +
                  '<div class="form-item form-type-textfield form-disabled">' +
                    '<input type="text" ' +
                      'disabled="disabled" name="token-sets[token-set-' + tsid + '][' + tokenMachine + '][token]" ' +
                      'value="' + tokenMachine + '" size="60" maxlength="128" class="form-text">' +
                  '</div>' + "\n" +
                  '<div class="form-item form-type-textfield form-disabled">' +
                    '<input type="text" ' +
                      'disabled="disabled" name="token-sets[token-set-' + tsid + '][' + tokenMachine + '][token_description]" ' +
                      'value="' + submitButton.parent().find('.token-description').val() + '" size="60" maxlength="128" class="form-text">' +
                  '</div>' + "\n" +
                  '<a href="#" class="remove-token-link" id="remove-token-' + tsid + '-' + tid + '" tsid="' + tsid + '" tid="' + tid + '">Remove token</a>' +
                '</div>'
              );
              $('#remove-token-' + tsid + '-' + tid).click(function (e) {
                e.preventDefault();
                _sbTokenSetManagerRemoveToken($(this));
              });
              submitButton.parent().find('.token-machine').val('');
              submitButton.parent().find('.token-description').val('');
            }
          }
        );
      });
    }
  }; // End SpringboardTokenSetAdmin
})(jQuery);
