$(document).ready(function() {
  $('div.batch-item .batch-item-fields').hide();  
  $('.batch-item a').click(function(e) {
  	e.preventDefault();
    $(this).next('.batch-item-fields').slideToggle('fast');
  });
});