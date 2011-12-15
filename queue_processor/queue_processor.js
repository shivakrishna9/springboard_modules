$(document).ready(function() {

  $('div.batch-item .batch-item-fields').hide();  
  $('.batch-item a.object-details').click(function(e) {
  	e.preventDefault();
    $(this).parent().find('.batch-item-fields').slideToggle('fast');
  });

  // drives select all checkbox on permanent failures report
  $('#edit-select-all').click(function() {
    status =  $(this).attr('checked');
    $(':checkbox').attr('checked', status);
  });
});
