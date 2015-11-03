(function ($) {
    window.sbaCountable = {};
    Drupal.settings.charCount = {size: 0, count: 0, person: ''};
    sbaCountable.charCount = function () {
      var currentCount = Drupal.settings.charCount.size;
      var message = 'This option has the potential to cause hundreds of targets to be presented to the user. Check back here when updating your target sets to get a count of all possible targets.';
      if (Drupal.settings.charCount.count > 0) {
        message = 'This message currently has ' + Drupal.settings.charCount.count + ' eligible targets which will be displayed to all visitors.';
      }
      $('.undistricted-update').text(message);

      $text = $('#edit-messages').find('textarea');
      var direction = 'down';
      var strict = true;
      if ($text.length == 0) {
        var direction = 'up';
        $text = $('#edit-field-sba-twitter-message-und-0-value');
        strict = false;
      }
      $text.each(function () {
        if (this.id = 'edit-field-sba-twitter-message-und-0-value') {
          var handleCount = Drupal.settings.charCount.size;
          $(this).siblings('.description').find('.counter-max').text(140 - handleCount);
          var currLen = $(this).val().replace(/(\r\n|\n|\r)/gm,"").length;
          if (currLen > 140 - handleCount) {
           $(this).addClass('error');
          }
          else {
            $(this).removeClass('error');
          }
        }
        else {
          handleCount = $(this).closest('.message-preview-message-fieldset').prev('.message-preview-header').find('.twitter-handle').text().length + 1;
        }
        var target = $(this).siblings('.description').find('.counter');

        $('#' + this.id).simplyCountable({
          counter: target,
          countType: 'characters',
          maxCount: 140 - handleCount,
          strictMax: strict,
          countDirection: direction,
          safeClass: 'safe',
          overClass: 'over',
          thousandSeparator: ',',
          onOverCount: function (count, countable, counter) {
            $('#edit-field-sba-twitter-message-und-0-value').addClass('twit-too-long');
          },
          onSafeCount: function (count, countable, counter) {
            $('#edit-field-sba-twitter-message-und-0-value').removeClass('twit-too-long');
          },
          onMaxCount: function (count, countable, counter) {
          }
        });
      })
    };

})(jQuery);