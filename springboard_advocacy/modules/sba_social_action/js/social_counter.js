(function ($) {
  $(document).ready(function(){
    $text = $('#edit-messages').find('textarea');
    var direction = 'down';
    if ($text.length == 0) {
      var direction = 'up';
      $text = $('#edit-field-sba-twitter-message-und-0-value');
    }

    $text.each(function(){
      var handleCount = $(this).closest('.message-preview-message-fieldset').prev('.message-preview-header').find('.twitter-handle').text().length + 1;
      var target = $(this).siblings('.description').find('.counter');

      $('#' + this.id).simplyCountable({
        counter:            target,
        countType:          'characters',
        maxCount:           140 - handleCount,
        strictMax:          true,
        countDirection:     direction,
        safeClass:          'safe',
        overClass:          'over',
        thousandSeparator:  ',',
        onOverCount:        function(count, countable, counter){},
        onSafeCount:        function(count, countable, counter){            console.log(this)
        },
        onMaxCount:         function(count, countable, counter){}
      });
    })
  });


})(jQuery);