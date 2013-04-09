(function ($) {
  Drupal.behaviors.capwiz_connect_dedupe = {
    attach: function (context, settings) {
      $('.dedupe-link', context).click(function(e) {
  	    e.preventDefault();
  	    var url = $(this).attr('href');
  	    var action = $(this).text();	
  	    // ajax to set or remove dedupe field
  	    $.get(url);

        // check the action
  	    if (action == 'Remove dedupe field') {
  		    new_url = url.replace('remove', 'set');
  		    text = 'Set as dedupe field';
  	    }
  	    else {
          new_url = url.replace('set', 'remove');
  		  text = 'Remove dedupe field';
  	      // set all others back to original state
          $('.webform-dedupe').not(this).each(function(){
  	        $(this).text('Set as dedupe field');
            $(this).attr('href', $(this).attr('href').replace('remove', 'set'));
          });
  	    }

        // set the new properties of the clicked link
        $(this).attr('href', new_url);
        $(this).text(text);
      });

      // Confirm intention when the user clicks on the Remove Map button
      $('input#edit-remove', context).click(function(e) {
        e.preventDefault();
        if (confirm("Are you sure you want to remove the field mapping? This cannot be undone; you will need to re-build this mapping after removing it.")) {
          this.form.submit();
        }
      });
    }
  }
})(jQuery);
