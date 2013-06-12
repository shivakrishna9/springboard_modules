(function ($) {
Drupal.behaviors.fundraiserEasyEditForms = {
  attach: function (context, settings) {
    $(window).ready(function(){
      
      if (!$('.fundraiser-donation-form')[0] && $('.admin-menu')[0]) {
        return;
      }
      
      $('.node .content').prepend('<div class="ui-additions hero-unit"><div class="btn-group"><div class="input-prepend"><span class="add-on">Label</span><input class="span2" id="labelInput" type="text" placeholder="Username"></div></div><div class="btn-group"><button class="new-ele btn" data-type="text" data-size="input-large">Text Large</button><button class="new-ele btn" data-type="text" data-size="input-medium">Text Medium</button><button class="new-ele btn" data-type="text" data-size="input-small">Text Small</button></div></div>');
      
      $('.node .content').prepend('<div id="admin-bar"><div class="navbar"><div class="navbar-inner closed"><ul class="nav"><li class="edit"><a class="edit-form" href="#"><i class="icon-edit"></i></a></li><li class="divider-vertical"></li><li><a class="reorder-fieldsets" href="#"><i class="icon-move"></i> Re-order Fieldsets</a></li><li class="divider-vertical"></li><li><a class="reorder-items" href="#"><i class="icon-move"></i> Re-order Items</a></li><li class="divider-vertical"></li><li><a class="add-elements" href="#"><i class="icon-plus"></i> Add Form Elements</a></li></ul></div></div></div>');
         
/* 
 * Button Actions 
 */
      $('#admin-bar .navbar-inner .edit-form').click(function(e) {
        e.preventDefault();
        if ($(this).closest('#admin-bar').find('.navbar-inner').hasClass('closed')) {
          $(this).closest('#admin-bar').find('.navbar-inner').switchClass('closed', 'open', 400, 'easeInOutQuad');
        } else if ($(this).closest('#admin-bar').find('.navbar-inner').hasClass('open')) {
          $(this).closest('#admin-bar').find('.navbar-inner').switchClass('open', 'closed', 400, 'easeInOutQuad');
        }    
      });
      // Set some classes
      $('.webform-component-fieldset > div.control-group').addClass('sortable');
      $('.webform-component-fieldset > div.control-group').parent().addClass('connectedSortable');
      $('#left, #right').addClass('col-connect');      
      // Re-order Fieldsets
      function reOrderFieldsets() {
        $('.webform-component-fieldset').sortable({
          connectWith: '.connectedSortable',
          cancel: '.webform-component-fieldset > div.div-title',
          placeholder: 'ui-state-highlight',
          distance: 5
        });
        $('.webform-component-fieldset > div.control-group').disableSelection();
        /*$('#left .webform-component-fieldset, #right .webform-component-fieldset').toggleClass(function() {
          if ($(this).parent().is('.sortable')) {
            return 'disabled';
          } else {
            return 'sortable';
          }
        });*/
      }
      // Re-Order items
      function reOrderItems() {
        $('#left, #right').sortable({
          cancel:'.disabled',
          connectWith: '.col-connect',
          placeholder: 'ui-state-highlight'
        });
      
        $('#left .webform-component-fieldset, #right .webform-component-fieldset').disableSelection();
        /*$('.webform-component-fieldset > div.control-group').toggleClass(function() {
          if ($(this).parent().is('.sortable')) {
            return 'disabled';
          } else {
            return 'sortable';
          }
        });*/
      }
      // Admin menu click functions
      $('.add-elements').click(function(e) {
        e.preventDefault();
        $('.ui-additions').toggle();
        $(this).parent().toggleClass('active');
      });

      $('.reorder-items').click(function(e) {
        e.preventDefault();
        reOrderFieldsets();
        $(this).parent().toggleClass('active');
      });

      $('.reorder-fieldsets').click(function(e) {
        e.preventDefault();
        reOrderItems();
        $(this).parent().toggleClass('active');
      });
      // New element create functions
      $('.new-ele').click(function(){
        var type = $(this).attr('data-type');
        var size = $(this).attr('data-size');
        var label = $('#labelInput').val();
        switch (type) {
          case 'text':
          var item = '<div class="control-group"><div class="btn-group">'+ ((typeof label != undefined) ? '<label>'+label+'</label>' : '') +'<input class="'+size+'" type="text" placeholder="'+label+'"></div></div>';
          
          $('.webform-client-form').prepend(item);
          break;
        }
      });
    
    });
  }
}
})(jQuery);
