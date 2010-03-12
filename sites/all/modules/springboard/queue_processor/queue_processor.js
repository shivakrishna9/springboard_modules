$(document).ready(function() {
  $('div.batch-item .batch-item-fields').hide();  
  $('.batch-item a').click(function() {
    $(this).next('.batch-item-fields').slideToggle('fast');
  });
});