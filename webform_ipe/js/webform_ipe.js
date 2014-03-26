(function($) {
  Drupal.behaviors.WebformIPE = {
    attach: function (context, settings) {
      $(window).ready(function() {
       /*
        * Namespace (e.g., "App") the Backbone application
        * and create the backbone objects
        */
        window.App = {
          Models: {},
          Collections: {},
          Views: {},
          Templates: {},
          Handlers: {},
          Mixins: {},
          Vars: {},
          Events: _.extend({}, Backbone.Events) //add the standard backbone event types to window.App  - http://backbonejs.org/#Events
        };

        /*
         *  BUILD THE  BACKBONE MODELS. One model per field type, more or less. We will call each model a "Component."
         */

        /*
         * Component base model uses DeepModel so that we can handle
         * multiple-level objects (webform components are multi-level).
         * Deep model improves support for models with nested attributes.
         * Allows you to get and set nested attributes with path syntax, e.g. user.type.
         * Triggers change events for changes on nested attributes.
         */

        // generic component
        App.Models.Component = Backbone.DeepModel.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              disabled: 0,
              field_prefix: '',
              field_suffix: '',
              attributes: {
                class: '',
                placeholder: '',
              }
            },
            form_key: 'default_key',
            mandatory: 0,
            name: '',
            pid: '',
            type: '',
            weight: '',
            deleted: 0,
          }
        });

        // an alternate template allows us to add field specific classes to edit forms
        // so we can hide some "extra" options on required fundraiser fields
        altFieldTemplate = _.template('\
          <div class="backbone-edit control-group field-<% if(key.indexOf("extra.") != -1) { key = key.substring(6)} %><%= key %>">\
            <label class="control-label" for="<%= editorId %>"><%= title %></label>\
            <div class="controls">\
              <span data-editor></span>\
              <div data-error></div>\
              <div><%= help %></div>\
            </div>\
          </div>\
        ');

        // TextComponent creates textfield component
        App.Models.TextComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              disabled: 0,
              unique: 0,
              private:0,
              field_prefix: '',
              field_suffix: '',
              attributes: {
                class: '',
                placeholder: ''
              }
            },
            form_key: 'default_key',
            mandatory: 0,
            name: 'New Textfield',
            pid: '',
            type: 'textfield',
          },
          // Edit form schema
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            mandatory: { type: 'Checkbox', title:'Required'},
            'extra.description': {type: 'Text', title: 'Description Text'},
            'extra.unique': {template: altFieldTemplate, type: 'Checkbox', title:'Unique', help:'Check that all entered values for this field are unique. The same value is not allowed to be used twice.'},
            'extra.attributes.class': {type: 'Text', title: 'Class'},
            'extra.attributes.placeholder': {type: 'Text', title: 'Placeholder'},
            'extra.title_display': {template: altFieldTemplate, type: 'Select', title: 'Label display', options: [{val:'before',label: 'Above'}, {val: 'none', label: 'None'}], help:'Determines the placement of the component\'s label.'},
            'extra.private': {template: altFieldTemplate, type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},
            'extra.field_prefix': {type: 'Text', title: 'Prefix Text'},
            'extra.field_suffix': {type: 'Text', title: 'Suffix Text'},
          },
        });

        App.Models.MailComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              disabled: 0,
              unique: 0,
              private:0,
              field_prefix: '',
              field_suffix: '',
              attributes: {
                class: '',
                placeholder: ''
              }
            },
            form_key: 'default_key',
            mandatory: '0',
            name: 'New Email Field',
            pid: '',
            type: 'email',
          },
          // Edit form schema
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            mandatory: { type: 'Checkbox', title:'Required'},
            'extra.description': {type: 'Text', title: 'Description Text'},
            'extra.unique': { type: 'Checkbox', title:'Unique', help:'Check that all entered values for this field are unique. The same value is not allowed to be used twice.'},
            'extra.attributes.class': {type: 'Text', title: 'Class'},
            'extra.attributes.placeholder': {type: 'Text', title: 'Placeholder'},
            'extra.title_display': {template: altFieldTemplate, type: 'Select', title: 'Label display', options: [{val:'before',label: 'Above'}, {val: 'none', label: 'None'}], help:'Determines the placement of the component\'s label.'},
            'extra.private': { type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},
          },
        });

        // TextAreaComponent creates textarea component
        App.Models.TextareaComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              rows: '5',
              cols: '15',
              resizable: 0,
              disabled: 0,
              private:0,
              attributes: {
                class: '',
              }
            },
            form_key: 'default_key',
            mandatory: 0,
            name: 'New Textarea',
            pid: '',
            type: 'textarea',
            deleted: 0,
          },
          // Edit form schema
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            mandatory: { type: 'Checkbox', title:'Required'},
            'extra.description': {type: 'Text', title: 'Description Text'},
            'extra.resizable': {type: 'Checkbox', title: 'Resizable'},
            'extra.rows': {type: 'Number', title: 'Height'},
            'extra.cols': {type: 'Number', title: 'Width'},
            'extra.disabled': {type: 'Checkbox', title: 'Disabled'},
            'extra.title_display': {type: 'Select', title: 'Label display', options: [{val:'before',label: 'Above'}, {val: 'none', label: 'None'}], help:'Determines the placement of the component\'s label.'},
            'extra.private': { type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},
            'extra.attributes.class': {type: 'Text', title: 'Class'},
          },
        });

        // SelectComponent extends Component (add multiple models to a collection)
        App.Models.SelectComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              field_prefix: '',
              field_suffix: '',
              items: '',
              aslist:0,
              multiple:0,
              private:0,
              attributes: {
                class: ''
              }
            },
            form_key: 'default_key',
            mandatory: 0,
            name: 'New Select',
            pid: '',
            type: 'select',
          },
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            mandatory: { type: 'Checkbox', title:'Required'},
            'extra.description': {type: 'Text', title: 'Description Text'},
            'extra.items': { editorClass:'required', type: 'List', title: "Options", fieldClass:"option-items", validate:['required'], help:'Options must be entered in the format: key|value '},
            'extra.aslist': { type: 'Checkbox', title:'Select List', help:'Check this option if you want the select component to be displayed as a select list box instead of radio buttons or checkboxes.'},
            'extra.multiple': { type: 'Checkbox', title:'Multiple', help:'Check this option if the user should be allowed to choose multiple values.'},
            'extra.optrand': { type: 'Checkbox', title:'Randomize', help:'Randomizes the order of the options when they are displayed in the form.'},
            'extra.title_display': {template: altFieldTemplate, type: 'Select', title: 'Label display', options: [{val:'before',label: 'Above'}, {val: 'none', label: 'None'}], help:'Determines the placement of the component\'s label.'},
            'extra.private': {template: altFieldTemplate, type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},

          },
          validate: function(attr) {
            var errs = {};
            var message = '';
            _.each(attr["extra.items"], function(value) {
              if(value.indexOf('|') == -1) {
               message =  '<div class = "error">Options must be in the format "key|value"</div>'; 
               errs += 1;
              }
           });
            if(attr["extra.items"].length < 1) {
              message = '<div class = "error">Options cannot be blank</div>';
              errs += 1;
            }
            if(errs.length > 0) {
              $('[name="extra.items"] div.error').remove();
              $('[name="extra.items"]').prepend(message);
              return errs;
            }
            else {
              $('[name="extra.items"] div.error').remove();
            }
          },
        });

        // FieldsetComponent extends Component (add multiple models to a collection)
        App.Models.FieldsetComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              collapsed: 0,
              collapsible: 0,
              description: '',
              title_display: 0,
              private: 0,
            },
            form_key: 'default_key',
            name: '',
            pid: '',
            type: 'fieldset',
            weight: 0,
            payment_html:'',
          },
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Fieldset Name', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            'extra.description': {type: 'Text', title: 'Description'},
            'extra.collapsible': { type: 'Checkbox', title:'Collapsible'},
            'extra.collapsed': { type: 'Checkbox', title:'Collapsed'},
            //'extra.title_display': {template: altFieldTemplate, type: 'Checkbox', title: 'Label display', help:'Determines the placement of the component\'s label.'},
            'extra.private': { template: altFieldTemplate, type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},
          },
        });


        // Number component
        App.Models.NumberComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              disabled: 0,
              field_prefix: '',
              field_suffix: '',
              decimals: 'Automatic',
              excludezero: '',
              integer:0,
              max:'',
              min:'',
              point:'.',
              step:1,
              type:'select',
              private: 0,
              attributes: {
                class: '',
                placeholder: ''
              }
            },
            form_key: 'default_key',
            mandatory: 0,
            name: 'New Number Field',
            pid: '',
            type: 'number',
            select_options:'',
          },
          // Edit form schema
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
             mandatory: { type: 'Checkbox', title:'Required'},
            'extra.description': {type: 'Text', title: 'Description Text'},
            'extra.decimals': {type: 'Select', title: 'Decimal places', help: 'Automatic will display up to 4 decimals places if needed. A value of "2" is common to format currency amounts.', options: ['Automatic', '0','1', '2', '3', '4', '5','6']},
            'extra.excludezero': {type: 'Checkbox', title: 'Exclude Zero', help: 'Exclude entries of zero (or blank) when counting submissions to calculate average and standard deviation.', options: {yes:'1'}},
            'extra.integer': {type: 'Checkbox', title: 'Integer', help: 'Permit only integer values as input. e.g. 12.34 would be invalid.', options: {yes:1}},
            'extra.type': {type: 'Radio', title: 'Element Type', help: '<br />A minimum and maximum value are required if displaying as a select.', options: {textfield:'Text Field', select:'Select List'}},
            'extra.min': {type: 'Number', title: 'Minimum', help: 'Minimum numeric value. e.g. 0 would ensure positive numbers.'},
            'extra.max': {type: 'Number', title: 'Maximum', help: 'Maximum numeric value. This may also determine the display width of your field.'},
            'extra.step': {type: 'Number', title: 'Step', help: 'Limit options to a specific increment. e.g. a step of "5" would allow values 5, 10, 15, etc.'},
            'extra.point': {type: 'Select', title: 'Decimal point', options: [{val:',',label: 'Comma'}, {val: '.', label: 'Period'}]},
            'extra.unique': { type: 'Checkbox', title:'Unique', help:'Check that all entered values for this field are unique. The same value is not allowed to be used twice.'},
            'extra.title_display': {type: 'Select', title: 'Label display', options: [{val:'before',label: 'Above'}, {val: 'none', label: 'None'}], help:'Determines the placement of the component\'s label.'},
            'extra.private': { type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},
            'extra.attributes.class': {type: 'Text', title: 'Class'},
            'extra.attributes.placeholder': {type: 'Text', title: 'Placeholder'},
            'extra.field_prefix': {type: 'Text', title: 'Prefix Text'},
            'extra.field_suffix': {type: 'Text', title: 'Suffix Text'},
          },
          validate: function(attrs) {
            var errs = {};
            if(attrs["extra.type"] == 'select' && attrs["extra.min"] == '' || attrs["extra.max"] =='') {
               alert ("Max and Min are required for select fields");
               errs.number = 'Max and Min are required for select fields';
               return errs;
            }
            if(attrs["extra.step"] == 0) {
               alert ("Step reqires a value");
               errs.step = 'Step reqires a positive value';
               return errs;
            }
          }
        });

        // Number component
        App.Models.DateComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              description: '',
              disabled: 0,
              value:'',
              timezone:'',
              datepicker: 0,
              year_textfield: 0,
              start_date:'',
              end_date:'',
              private: 0,
              attributes: {
                class: 'date-field',
              }
            },
            form_key: 'default_key',
            mandatory: 0,
            name: 'New Date Field',
            pid: '',
            value:'',
            type: 'date',
            date_html:'',
          },
          // Edit form schema
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            mandatory: { type: 'Checkbox', title:'Required' },
            value: {type: 'Text', title: 'Default Value', help:'The default value of the field. Accepts any date in any GNU Date Input Format. Strings such as today, +2 months, and Dec 9 2004 are all valid.'},
            'extra.start_date': {editorClass:'required', type: 'Text', title: 'Start Date', help: "The earliest date that may be entered into the field. Accepts any date in any GNU Date Input Format"},
            'extra.end_date': {editorClass:'required', type: 'Text', title: 'End Date', help:'The earliest date that may be entered into the field. Accepts any date in any GNU Date Input Format' },
            'extra.year_textfield': { type: 'Checkbox', title:'Use a textfield for the year', help:'If checked, the generated date field will use a textfield for the year. Otherwise it will use a select list.'},
            'extra.timezone': {type: 'Radio', title: 'Timezone', help:'If using relative dates for a default value (e.g. "today") base the current day on this timezone.', options: {user:'User Timezone', site:'Site Timezone',}},
            'extra.datepicker': { type: 'Checkbox', title:'Enable popup calendar', help:'Enable a JavaScript date picker next to the date field.'},
            'extra.description': {type: 'Text', title: 'Description Text'},
            'extra.title_display': {type: 'Select', title: 'Label display', options: [{val:'before',label: 'Above'}, {val: 'none', label: 'None'}], help:'Determines the placement of the component\'s label.'},
            'extra.private': { type: 'Checkbox', title:'Private', help:'Private fields are shown only to users with results access.'},
            'extra.attributes.class': {type: 'Text', title: 'Class'},
          },
          validate: function(attrs) {
            var errs = {};
            if(attrs["extra.end_date"] == '' || attrs["extra.start_date"] == '') {
               alert ("Start and end dates are required");
               errs.date = 'Start and end dates are required';
               return errs;
            }
          }
        });

        // Markup creates markup component
        App.Models.MarkupComponent = App.Models.Component.extend({
          defaults: {
            cid: '',
            extra: {
              format: 'full_html',
              attributes: {
                class: '',
              }
            },
            form_key: 'default_key',
            name: 'New Markup',
            pid: '',
            value:'',
            type: 'markup',
          },
          // Edit form schema
          schema: {
            name: {editorClass:'required', type: 'Text', title: 'Label Text', validators: ['required']},
            value: {editorClass:'required', type: 'TextArea', title: 'Markup', validators: ['required']},
            'extra.container': {type: 'Select', title: 'Container', options: {}},
            'extra.attributes.class': {type: 'Text', title: 'Class'},
          },
        });

        /*
         * INSTANTIATE A MODEL COLLECTION. "Collections are ordered sets of models."
         * This is called from the setup below
         */

        App.Collections.Components = Backbone.Collection.extend({
          model: App.Models.Component
        });

        /*
         * DEFINE SOME TEMPLATE HELPER FUNCTIONS
         * The templates themselves live in templates.html
         */

        window.template = function(id) {
          return _.template($('#'+id).html());
        };

        // Append functions to our templates
        App.Templates.template = function(templateString) {
          var templateFn = _.template(templateString);
          return function(context){
            return templateFn(_.extend({}, template.fn, context));
          }
        };

        template.fn = {};    // template.fn holds user-defined handler functions
        //Because we don't want to declare functions in the template itself, but we might occasionally need them.
        //usage example: <%= getId(cid) %>

        //convert webform's key|value option strings into a format backbone understands
        template.fn.optionList = function(items) {
          var options = [];
          _.each(items, function(value) {
            var ops = value.split('|');
            var optionItem = {};
            _.each(ops, function(k){
              if(_.isUndefined(optionItem['val'])) {
               optionItem['val'] = k;
              }
              else {
               optionItem['label'] = k;
              }
            });
            optionItem['label'] = (_.isUndefined(optionItem['label'])) ? optionItem['val'] : optionItem['label'];
            options.push(optionItem);
          });
         // console.log(options)
          return options;
        }

        // grab components existing attributes so they can be passed back to the template
        // so we maintain visual styling after re-rendering by underscore
        template.fn.buildId = function(form_key, type) {
          if(form_key.indexOf('default_key') == -1) {//not new field
            var existing = {};
            if (!_.isUndefined(type)) {
              switch(type) {
                case 'radio':
                case 'checkbox':
                existing.id  = $('[name*="['+ form_key +']"]').closest('.control-group').parent('[id*="edit-submit"]').attr('id');
                existing.name = $('[name*="['+ form_key +']"]').attr('name');
                break;
                case 'markup':
                  var id = form_key.replace(/_/g,'-');
                  existing.id  = id;
                  break;
              }
            }
            else { //everything else
              
              // It's possible in the wild that some themes may use the region/column specific ID for styling.
              // Perhaps we should remove those IDs and classes here, and substitute the form_key, as we do in the
              // new field section below. That way, there will be a visual cue that something has a dependency
              // on a CSS selector tied to a specific position on the page, and when we move an item, the style is removed.
              // For the moment, though, we are preserving the original selectors.
              existing.id  = $('[name*="['+ form_key +']"]').attr('id');
              existing.cls = $('[name*="['+ form_key +']"]').attr('class');
              existing.name = $('[name*="['+ form_key +']"]').attr('name');
              existing.value = $('[name*="['+ form_key +']"]').attr('value');
              existing.size = $('[name*="['+ form_key +']"]').attr('size');
            }
            if (!_.isEmpty(existing) && !_.isUndefined(existing.id)) {
              return existing;            
            }
            else {
              // "this.el" is empty for IE in RenderChild
              var ieWorkAround = {};
              var id = form_key.replace(/_/g,'-');
              ieWorkAround.id  = '-'+id;
              ieWorkAround.cls = '-'+id;
              ieWorkAround.name = '[' + form_key + ']';
              if (!_.isEmpty(ieWorkAround)) {
                return ieWorkAround;            
              }
            }
          }
          else {
            //new fields
            //need to create pseudo-attributes so we can track these items during drag and drop
            var new_field = {};
            var id = form_key.replace(/_/g,'-');
            new_field.id  = id;
            new_field.cls = id;
            new_field.name = '[' + form_key + ']';
            if (!_.isEmpty(new_field)) {
              return new_field;            
            }
          }
        }

         // Prepare the dummy html for payment fields and date fields
         template.fn.stripWrapper = function(html, field) {
          if(field == 'payment') {
            trimmed = $(html).unwrap('fieldset.fundraiser-payment-fields');
          } 
          return trimmed[0].innerHTML;
        }

        /*  DEFINE THE VIEWS
         *  Backbone views don't determine anything about your HTML or CSS for you, but
         *  can be used with any JavaScript templating library. The general idea
         *  is to organize your interface into logical views,
         *  each of which can be updated when the model changes,
         *  without having to redraw the page. Instead of digging into a JSON object,
         *  looking up an element in the DOM, or updating the HTML by hand, you can
         *  bind your view's render function to the model's "change" event —
         *  and now everywhere that model data is displayed in the UI, it is always immediately up to date.
         */

        /*
         * View for a single Component.
         * Handles the creation and update of individual components,
         * not the update of the page component group as a whole
         */
        App.Views.Component = Backbone.View.extend({

          tagName: function(){
            var type = this.model.get('type');
            if(type =='fieldset'  || type == 'payment_method') {
              return 'fieldset';
            }
            else {
              return 'div';
            }
          },

          className: function(){

            var type = this.model.get('type');
            var form_key = this.model.get('form_key');
            if(type =='fieldset' || type == 'payment_fields' || type == 'payment_method') {
              if(form_key.indexOf('default_key') != -1) {
                //new fieldset:
                //create a pseudo class so we can assign unique attributes in reRender
                var fieldsetKey = form_key.replace(/_/g,'-');
                return 'webform-component-fieldset ' + fieldsetKey;
              }
              else {
                //existing fieldset
                return 'webform-component-fieldset';
              }
            }
            else return 'control-group';
          },
          template: App.Templates.template($('#componentTemplate').html()),

          attributes : function () {
            return {};
          },

          initialize: function(){
            // On change reRender existing items
            var type = this.model.get('type');
            if(type == 'number') {
              this.model.on('change', this.makeNumber, this);
            }

            this.model.on('change', this.reRender, this);

            // When a fieldset with children has changed, listen up kids
            this.listenTo(this.model, 'fieldset:renderChild', this.renderChild);
          },

          events: {
            'mouseenter': 'mouseenter',
            'mouseleave': 'mouseleave',
            'click .edit-item': 'edit',
            'click .delete-item': 'destroy',
            'drop' : 'drop',
          },
          // calculate step values for numbr fields
          makeNumber: function(){
            var type = this.model.get('type');
            var model = this.model;
            var extra = model.get('extra')
            var mandatory = model.get('mandatory')
            if(extra.type == 'select' && extra.min != "" && extra.max != "") {
              var options = [];
              var step = Math.abs(extra.step);
              var step = _.isUndefined(step) ? 1 : Number(step);
              var min = Number(extra.min);
              var max = Number(extra.max);
              var flipped =  false;
              if (max < min) {
                min = Number(extra.max);
                max = Number(extra.min);
                flipped = true;
              }
              for (f = min; f <= max; f = f + step) {
                options[f] = f;
              }
              if (flipped) {
                options = options.reverse();
              }
              if(mandatory == 0) {
                options.unshift('- None -');
              }
               var html = '';
              // Apply requisite number formatting.
               _.each(options, function(value, index) {
                if(value != '- None -') {
                  value = value.toFixed(extra.decimals);
                  var point = value.indexOf('.') ? '.' : ',';
                  if(value.indexOf(extra.point) == -1) {
                    if(Number(extra.decimals) > 0) {
                      value = value.replace(point, extra.point);
                    }
                  }
                }
                html += '<option value ="' + value + '">' + value + '</option>';
              });
              model.set('select_options', html, {silent:true});
            }
          },

          mouseenter: function(){
            console.log('MOUSENTER');
            if (typeof(event)) {
              //event.stopPropagation();
            }

            //country dropdown selector JS causes multiple buttons appear, after change
            if($('span.edit').length > 1) {
              $('span.edit').remove();
            }
            $('label.error').hide();
            //append the edit and delete buttons
            var type = this.model.get('type');
            switch (type) {
              case 'fieldset':
              case 'payment_fields':
              case 'payment_method':
                if (type != 'payment_fields' && type != 'payment_method'){
                  var collapse = this.model.get('extra.collapsible');
                  var edit = $(this.$el[0]).find('legend span.edit');
                  if(edit.length == 0 ) {
                    var buttonTarget = this.$el.find('.fieldset-legend');
                    if($('.editor-on').length > 1) {
                      var clone = buttonTarget.clone();
                      clone.css('visibility','hidden').appendTo(this.el);
                      var legendWidth = (clone.css({display: 'inline', width: 'auto'}).width()) + 10;
                      clone.remove();
                    }
                  } 
                }
              break;
              case 'select':
                if((this.model.get('extra.aslist') == true)) {
                  var buttonTarget = this.$el.find('select').last(); //after select
                }
                else {
                  var buttonTarget = this.$el.find('.control-group').last(); //after last radio or checkbox
                }
              break;
               case 'number':
                 if(this.model.get('extra.type') == 'select') {
                   var buttonTarget = this.$el.find('select').last();
                 }
                 else {
                   var buttonTarget = this.$el.find('input').last();
                 }
                 break;
              case 'date':
                var buttonTarget = this.$el.find('select').first();
                break;
              default:
                var buttonTarget = this.$el.find('input, textarea, p, select').last();
            }

            if(!_.isUndefined(buttonTarget)) {
              if($('.sortable-placeholder').length == 0  && type != 'fieldset') {
                buttonTarget.after(App.Templates.template($('#editTemplate').html()));
              }
              else if ($('.sortable-placeholder').length == 0) {
                buttonTarget.append(App.Templates.template($('#editTemplate').html()));
              }
              if($('.editor-on').length > 1 && typeof(legendWidth) !== "undefined" && type == 'fieldset') {
                if(buttonTarget.closest('fieldset').width() > (legendWidth + 30)) {
                  buttonTarget.closest('fieldset').find('legend span.edit').css({position:'absolute', left:legendWidth})
                }
                else {
                  buttonTarget.closest('fieldset').find('legend span.edit').css({position:'absolute', left: 'auto', top: '-17px', right:'5px'})
                }
              }
              else if ($('.editor-on').length > 1  && type != 'fieldset'){
               // buttonTarget.parent().find('span.edit').css({ display:'block', float:'left'})
              }
              var required = Drupal.settings.webform_ipe.required_fields;
              var formKey = this.model.get('form_key');
              if(required.indexOf(formKey) != -1) {
                this.$el.find('i.delete-item').remove();
              }
              if($('.sortable-placeholder').length == 0 ) {
                this.$el.find('span.edit').show(400);
              }
            }
          },

          drop: function(event) {
            if (typeof(event)) {
              event.stopPropagation();
            }
            if($('#webform-ipe-unsaved-edits').length < 1) {
              $('.save-and-preview').show();
              $('#admin-bar').append('<div id = "webform-ipe-unsaved-edits">This form has unsaved changes</div>');
              $('body').addClass('unsaved-ipe');
            }
            this.$el.trigger('update-weight', [this]); //recalculate field weights on drag and drop
          },

          mouseleave: function(){
            this.$el.find('.edit').remove();
          },

          edit: function(e){

            if (e) e.preventDefault();
            // Make sure nested view clicks don't bubble to the parent view
            if ($('body .blockMsg')[0]) return;

            // Create the edit form which is dispalyed in the popup
            var form = new Backbone.Form({
              model: this.model,
            }).render();

            if ($('#editForm form')[0]) {
              $('#editForm form').remove();
            } else {
              $('body').append('<div id="editForm"><button class="save btn btn-primary">Update Form</button></div>');
            }

            // save individual component
            // $(form.el).append('<div id="editNow"><label>Save to live form</label><div><span><input class="checkbox" type="checkbox" checked=""></span><div>If you uncheck this box, items will not be saved until the "Save All" button is clicked.</div></div></div>');

            $('body #editForm').prepend(form.el).html();
            $('body #editForm').show();

            var formHeight = $('body #editForm').actual('height') + 60;
            if (formHeight > 720) {
              formHeight = 720;
            }

            var cid = this.model.get('cid');
            var pid = this.model.get('pid');
            var fkey = this.model.get('form_key');
            var type = this.model.get('type');

            //don't allow options to be edited on recurring field
            if (fkey == "recurs_monthly") {
              $('.option-items').hide();
            }
            $('[name="extra_collapsed"]').click(function(){
              $('[name="extra_collapsible"]').prop('checked', true);
            });

            //hide some items that should not be editable
            var required = Drupal.settings.webform_ipe.required_fields;
            if (required.indexOf(fkey) != -1) {
              var extra = this.model.get('extra');
              _.each(extra, function( value, name ) {
                 $('.backbone-edit.field-'+ name).hide();
              });
            }

            //prepare the "container" select box -  sort the options by cid
            var options = window.componentCollection.where({type: 'fieldset'});
            options = _.sortBy(options, function(option) {
              return Number(option.get('cid'));
            });

            //is this a "form layout" enabled form?
            var has_form_layout =  $('div.fieldset.form-layouts').length;
            if(has_form_layout === 1) {
              $('[name="extra_container"]').append('<option value ="0">No Container</option>');
            }

            //Build the "container" select box
            _.each(options, function(option) {

              var optionCid = option.get('cid');
              var optionName = option.get('name');
              var optionKey = option.get('form_key');

              //if sidebars aren't visible, remove from select list
              sidebar = $('.sidebar[data-cid="' + optionCid + '"]');
              if (sidebar.length == 1) {
                if (sidebar.width() == 0 || sidebar.filter(':visible').length == 0) {
                 return;
                }
              }

              //If nested fieldsets aren't allowed
              var regions = ['left_sidebar','right_sidebar','top_region','right_region','bottom_region','left_region'];
              if (type == 'fieldset' && regions.indexOf(optionKey) != -1) {
                if(cid != optionCid) {
                  $('[name="extra_container"]').append('<option value ="' + optionCid + '">' + optionName + '</option>');
                }
              } else if(type != 'fieldset' && cid != optionCid) {
                $('[name="extra_container"]').append('<option value ="' + optionCid + '">' + optionName + '</option>');
              }

              //if nested fieldsets are allowed
              // if(cid != optionCid) {
              //   $('[name="extra_container"]').append('<option value ="' + optionCid + '">' + optionName + '</option>');
              // }

            });
            //set default value
            if (!_.isUndefined(pid)) {
              $('[name="extra_container"]').val(pid);
            }

            // Create the blockUI
            App.Handlers.blockUIBuilder(400, formHeight, form, form.model.get('name')); //make the popup window
            $('body .blockMsg #blockUIform').append($('#editForm'));
            $('#editForm').find('.required').each(function(){
               $('#editForm label[for="' + this.id + '"]').append(' <span class="form-required" title="This field is required.">*</span>');
            });
          },

          destroy: function(e){
            console.log('DESTROY');
            //we're not actually going to destroy the items
            //as we need them to be present in the model when we
            //pass it back via Ajax to Drupal for processing.
            //So instead we'll just add a "deleted" property to the model
            if (e) {
              e.preventDefault();
              event.stopImmediatePropagation();
            }
            var retVal = confirm("Delete this item? If you change your mind, you can reload the page (without saving) to recover deleted items.");
            if( retVal == false ) {
              return false;
             }
            this.model.set('deleted', '1', {silent: true});
            if (!(this.model.get('type') == 'fieldset')) {
              this.$el.remove();
              //this.model.destroy({wait: true});
            } else {
              this.$el.remove();
              //App.Events.trigger('fieldset:destroy');
              //this.model.destroy({wait: true});
            }
            if(this.model.get('form_key').indexOf('default_key') != -1) {
              this.model.destroy()
            }
            if($('#webform-ipe-unsaved-edits').length < 1) {
              $('.save-and-preview').show();
              $('#admin-bar').append('<div id = "webform-ipe-unsaved-edits">This form has unsaved changes</div>');
              $('body').addClass('unsaved-ipe');

            }
          },

          //made obsolete by the preceding ^^^^
          // remove: function(newpid){
          //   console.log('REMOVE');
          //   if (newpid) {
          //     console.log('new pid: ' + newpid);
          //   }
          //   this.$el.remove();
          // },

          //called from nowhere
          render: function(){
            pid = this.model.get('pid');
            if (this.model.get('type') != 'fieldset') {
              this.$el.html(this.template(this.model.toJSON()));
            } else {
            }
            return this;
          },

          reRender: function(){
            console.log('RERENDER');

            var item = this.model;
            var cid = item.get('cid');
            var model_pid = item.get('pid'); //get parent
            var extra_pid = item.get('extra.container'); //pid value in the edit "container" selector
            model_pid = model_pid == "new" ? "0" : model_pid;
            var type = item.get('type');
            var form_key = item.get('form_key');
            var extra = item.get('extra')

            //new fields need a weight assigned in case the form is saved, but never UI-sorted.
            if(_.isUndefined(item.get('weight'))) {
              item.set('weight', 0, {silent: true});
            }
            
            //is this line even needed? For IE?
            //Shouldn't it come *after* any prepends below?
            var field = this.$el.html(this.template(this.model.toJSON()));

            if (type != 'fieldset' && type != 'payment_fields' && type != 'payment_method') {

              //this block processes changes to PID made via the "container" selector in the edit dialog
              if ((!_.isUndefined(extra_pid) && extra_pid != 0) && model_pid != extra_pid) {
                item.set('pid', extra_pid, {silent: true});
                 $('[data-cid="'+ extra_pid +'"]').prepend(this.el)
                 this.$el.attr('data-pid',extra_pid);
              }
              else if(extra_pid == 0) {
                item.set('pid', 0, {silent: true});
                $('.webform-client-form').prepend(this.el);
                this.$el.attr('data-pid', "0");
              }

              //add phony data attributes needed for sorting to brand new items
              if(form_key.indexOf('default_key') != -1) {
                model_pid = this.model.get('pid');
                this.$el.attr('data-pid', model_pid);
                this.$el.attr('data-cid', cid);
              }

              this.$el.trigger('update-weight', [this]); //recalculate field weights after insert

            } else { // Re-render the fieldset

              if(extra.collapsible == 0) {
                this.$el.removeClass('collapsible');
              }
              if(extra.collapsed == 0) {
                this.$el.removeClass('collapsed');
              }
              if(extra.collapsed == 1) {
                this.$el.addClass('collapsed');
              }
              if(extra.collapsible == 1) {
                //lifted out of collapse.js because calling Drupal.attachBehaviors(); causes fields to disappear.
                this.$el.addClass('collapsible');
                var $fieldset = this.$el;
                var anchor = location.hash && location.hash != '#' ? ', ' + location.hash : '';
                if ($fieldset.find('.error' + anchor).length) {
                  $fieldset.removeClass('collapsed');
                }
                var summary = $('<span class="summary"></span>');
                $fieldset.
                  bind('summaryUpdated', function () {
                    var text = $.trim($fieldset.drupalGetSummary());
                    summary.html(text ? ' (' + text + ')' : '');
                  })
                  .trigger('summaryUpdated');
                var $legend = $('> legend .fieldset-legend', this.$el);

                $('<span class="fieldset-legend-prefix element-invisible"></span>')
                  .append($fieldset.hasClass('collapsed') ? Drupal.t('Show') : Drupal.t('Hide'))
                  .prependTo($legend)
                  .after(' ');
                var $link = $('<a class="fieldset-title" href="#"></a>')
                  .prepend($legend.contents())
                  .appendTo($legend)
                  .click(function () {
                    var fieldset = $fieldset.get(0);
                    // Don't animate multiple times.
                    if (!fieldset.animating) {
                      fieldset.animating = true;
                      Drupal.toggleFieldset(fieldset);
                    }
                    return false;
                  });
                $legend.append(summary);;
              }

              //this block processes changes to PID made via the "container" selector in the edit dialog
              if (!_.isUndefined(extra_pid) && extra_pid !=0) {
                item.set('pid', extra_pid, {silent: true});
                if(extra_pid != model_pid) {
                  $('[data-cid="'+ extra_pid +'"]').prepend(this.el); //this fieldset has a new parent
                }
              }
              else if(model_pid != 0 && model_pid != "new" && _.isUndefined(extra_pid)) {//this would be a child fieldset which has never been edited via the gui
                item.set('pid', model_pid, {silent: true});
                $('[data-cid="'+ model_pid +'"]').prepend(this.el);
              }
              else {
                if((extra_pid == "0" && extra_pid != model_pid)) {
                  item.set('pid', extra_pid, {silent: true});
                  $('.webform-client-form').prepend(this.el); //new fieldset or moving an old one to top level
                }
              }


              //add phony data attributes needed for sorting to brand new items
              if(form_key.indexOf('default_key') != -1) {
                model_pid = this.model.get('pid');
                var fieldsetKey = form_key.replace(/_/g,'-');
                this.$el.attr('data-cid', cid);
                this.$el.attr('data-pid', model_pid);
                this.$el.attr('id', fieldsetKey);
              }

              //remove a duplicate fieldset
              if (type == 'payment_fields') {
                $('#edit-submitted-payment-information-payment-fields-credit--2 .fieldset-wrapper').unwrap();
              }
              var childComponents = this.reorder(window.componentCollection.where({pid: cid}), 'weight'); // Get and re-order child components by weight

              if (childComponents.length > 0 ) {
                _.each(childComponents, function(model, i) {      // Trigger the rerender for each model
                  model.trigger('fieldset:renderChild');
                });
                this.$el.trigger('update-weight', [this]); //recalculate field weights after insert
              }
              else {
                this.$el.trigger('update-weight', [this]); //recalculate field weights after insert
                return;
              }
            }//end fieldset
            return this;
          },

          renderChild: function(){ //causing radios to lose styling
            console.log('RENDER CHILD');

            pid = this.model.get('pid');
            var type = this.model.get('type');

  
            if (type != 'fieldset' && type != 'payment_fields' && type != 'payment_method') {

              // Re-render. We don't really need to rerender, we could just append "this.el", 
              // because the fieldset is the only changed item...

              // BUT 
              // Internet Explorer requires a rerender, because in
              // IE "this.el" is empty, at least when attempting to access it from here!

              // BUT
              // calling reRender from here causes problems for all browsers:
              // You can't $('#select_this_field') from the template and template helper functions anymore:
              // Why? It's no longer in the DOM. You have to re-append it first.

             // So

              $('[data-cid="'+ pid +'"] .fieldset-wrapper').append(this.el);

              if (this.$el.html() == '') {
                 this.$el.html(this.template(this.model.toJSON()));
                //OK, now we have some html for IE - but all the ID's and Atrributes are missing
                // So the buildID function up above has a special section for IE.
              }
      
              // Rebind our component view events
              this.delegateEvents();
            } else {
              // Make it happen recursively for fieldsets
              console.log('re-render child fieldsets');
             //was causing infinite loop for new fieldsets, works now
              this.reRender();
            }
            return this;
          },

          reorder: function(collection, orderBy){
            console.log('REORDER');
            // Sort by weight
            return _.sortBy(collection, function(model) {
              return parseInt(model.get(orderBy), 10);
            });
          },
          addNew: function(){
            console.log('ADD NEW');
            this.$el.html(this.template(this.model.toJSON()));
            this.edit();
            return this;
          },

          //not currently used anywhere
          // reRenderComponent: function(cid){
          //   console.log('RE-RENDER COMPONENT');
          //   $('[data-cid="'+ pid +'"]').append(this.el);
          //   this.delegateEvents();
          //   return this;
          // }
        });

        /*
         * DEFINE A VIEW FOR ALL COMPONENTS,
         * This is called at the end of the "setup" function below.
         * to handle added and deleted items as a group,
         * rather than changes to content of individual items
         * Also fires when adding new item
         */
        App.Views.AllComponents = Backbone.View.extend({
          el: $('body form.webform-client-form'),
          events: {
              'update-weight': 'updateWeight',
              'save' :'save'
            },
          initialize: function(){
            this.collection.on('add', this.addComponent, this);

            console.log('ALLCOMPONENTS_INIT');
            // Since this is our collection view we can render it immediately
            this.render();
          },
          render: function(){
            console.log('ALLCOMPONENTS_RENDER');
            this.collection.each(this.addComponent, this);
            return this;
          },
          addComponent: function(component){
            console.log('ALLCOMPONENTS_ADD');
            var cid = component.get('cid');
            if (cid > 0) { //existing webform components
              if (!$('[data-cid="'+cid+'"]')[0]) return;
              var componentView = new App.Views.Component({ model: component });
              var type = componentView.model.get('type');
              if (type != 'fieldset' && type != 'payment_fields' && type != 'payment_method') {
                componentView.setElement($('div.control-group[data-cid="' + cid + '"]') );
                //console.dir(componentView.el)
              } else {
                if(type == 'payment_fields') {
                  componentView.setElement($('div[data-cid="' + cid + '"]') );
                }
                else {
                  componentView.setElement($('fieldset[data-cid="' + cid + '"]') );
                }
              }

            } else { //new backbone-generated components
              var type = component.get('type');

              //all new items need a pseudo pid so we can track them
              component.set('pid', 0);

              //create a phony and unique form_key so we can track this properly
              //and build CSS ids, classes and HTML attributes in the template
              //this form_key may need adjustment before saving to drupal
              num = $.now();
              component.set('form_key', 'default_key_' + type + '_' + num);

              //we need to set a phony cid so that we can reorder items
              //this cid will have to be removed before saving to Drupal
              component.set('cid', 'default_key_' + type + '_' + num);

              var componentView = new App.Views.Component({ model: component });
              componentView.addNew().el; //start the editor
            }
          },

          save: function(){ //should be in App.Collections.Components
            var comps = {}
             this.collection.each(function (model, index) {
               comps[index] = model.toJSON();
             });
              //comps['ipe_form_action'] = $('div.form-actions').closest('.webform-ipe-container').attr('id').replace('webform-component-','').replace(/-/g, '_');
              var callbackPath = Drupal.settings.webform_ipe.callback;
              $('#admin-bar').append('<p><div class="ajax-progress"><div class="throbber">&nbsp;</div></div></p>');
              $('#webform-ipe-unsaved-edits').hide();
              $('.throbber').show();
              $('.save-and-preview').hide();
              $('body').removeClass('unsaved-ipe');

              $.ajax({
                type: "POST",
                url: callbackPath,
                data: {components: comps},
                dataType: 'json',
                success: function(data) {
                  location.reload(true);
                },
                error: function(xhr, textStatus, error){
                   console.log(error);
                   $('.ajax-progress').remove();
                   $('#webform-ipe-unsaved-edits').show();
                   $('.save-and-preview').show();
                   $('body').addClass('unsaved-ipe');
                   alert('An error occurred. Changes were not saved.')
                }
             });
          },

          updateWeight: function(event, field) {
            console.log('UPDATE WEIGHT');

           /* Update the PIDs and the form element weights (in the model)
            * This can be confusing. jQueryUI keeps an index of items that is
            * not the same as backbone's index of items, which of course is not
            * the same as Drupal's form element weights. So, we're going to ignore
            * all three of those and calculate and rebuild the weights here
            */

            //first, update the parent IDs of moved elements
            var type = field.model.get('type');
            var old_pid = field.model.get('pid');
            var form_key = field.model.get('form_key');

            if (type != 'fieldset' && type != 'payment_fields' && type != 'payment_method') {

              //get the parent fieldset cid
              var fieldset_cid = $(field.$el.parents('.webform-ipe-container')[0]).attr('data-cid');
              //if we've moved to a new fieldset, or to a placeholder pseudo-fieldset, update the pid
              if(old_pid != fieldset_cid && !_.isUndefined(fieldset_cid)) {
                field.model.set('pid', fieldset_cid,  {silent: true});
              }
              else if (_.isUndefined(fieldset_cid)) {
               // ob.model.set('pid', 0,  {silent: true});
              }
            }
            else {
              // We've moved a fieldset, not a field
              if(type != 'payment_fields' && type != 'payment_method') {
                var new_pid = $(field.$el.parents('.webform-ipe-container')[0]).attr('data-cid');
                field.model.set('pid', new_pid,  {silent: true});
              }
            }

            var pid = field.model.get('pid'); //grab the updated pid
            field.$el.attr('data-pid',pid);

            //now update the weights

            this.collection.each(function (eachModel, index) {
              var form_key = eachModel.get('form_key');
              var id = form_key.replace(/_/g,'-');
              var type = eachModel.get('type');
              var pos;
              switch (type) {
                case 'fieldset':
                case 'payment_method':
                  pos = $('[id$="'+id+'"]');
                break;
                case 'payment_fields':
                  pos = $('div[id$="'+id+'"]');
                 break;
                case 'markup':
                  pos = $('div[id$="'+id+'"]').closest('.control-group');
                  break;
                case 'select':
                  var multi = eachModel.get('extra.multiple');
                  if(!_.isUndefined(multi) && multi == "true") {
                    pos = $('div[id$="'+id+'"').closest('.control-group');
                  }
                  else if(form_key == 'recurs_monthly') {
                    pos = $($('[name*="recurs"]').parents('.control-group')[1]);
                  } else {
                    pos = $('[name$="['+form_key+']"]').closest('.control-group');
                  }
                break;
                case 'date':
                  if(form_key.indexOf('default_key') != -1) {
                    pos = $($('label[for*="'+id+'"]').parents('.control-group')[0]);
                  }
                  else {
                    pos = $($('[name*="['+form_key+']"]').parents('.control-group')[0]);
                  }
                break;
                default:
                  pos = $('[name$="['+form_key+']"]').closest('.control-group');
                break;
              }
              var weight = $('.webform-client-form div.control-group').add('.webform-client-form .webform-component-fieldset').index(pos)
              eachModel.set('weight', weight,  {silent: true});
            });

            //now sort everything *in the model* by weight, so that the renderChild function works properly
            this.collection.comparator = function( model ) {
              return model.get( 'weight' );
            }
            this.collection.where({pid: pid}).sort();

            //debug function
            //  this.collection.each(function (model, index) {
            //   console.log(model.get('weight') + model.get('form_key'));
            // });
          }
        });

        /*
         * BUILD THE POPUP BLOCKS
         */
        // Top position helper
        App.Handlers.blockUITop = function(height){
          //console.log('height before: '+height);
          if (height > 630) {
            var height = 630;
          }
         // console.log('height: '+height);
          return ((($(window).height() - height) /2)/$(window).height())*100;
        };
        // Left posiiton helper
        App.Handlers.blockUILeft = function(width){
          return ((($(window).width() - width) /2)/$(window).width())*100;
        };
        /*
         * All blockUI items should be built with this function
         * afterBuild allows for a callback function to be passed
         */
        App.Handlers.blockUIBuilder = function(width, height, form, title, afterBuild) {
          var blockWidth = width || 400;
          var blockHeight = height || 630;
          $.blockUI({
            message: '<div id="blockUIform"><h4>Edit ' + title + '<span class="closeUI"><i class="fa fa-times"></i></span></h4></div>',
            css: {
              overflow: 'auto',
              top: App.Handlers.blockUITop(height) + '%',
              left: App.Handlers.blockUILeft(width) + '%',
              height: blockHeight + 'px',
              width: blockWidth + 'px',
              border: '0px'
            },
            overlayCSS: {
              cursor: 'default'
            },
            onBlock: function(){

              // Save form to model
              $('#editForm .save').click(function(e){

                var success = form.commit({ validate: true });
                console.log("FORM COMMIT")

                //validation failure, keep the window open
                if(!_.isEmpty(success)) {
                  return false;
                }

                var type = form.model.get('type');
                if (type == 'date') {
                  App.Handlers.createDate(type, form);
                }

                //if we've created new fields, add the classes for sorting
                if($('.editor-on').length > 0) {
                  App.Handlers.addSortClasses();
                }

                //show the "save all changes" button
                if($('#webform-ipe-unsaved-edits').length < 1) {
                  $('.save-and-preview').show();
                  $('#admin-bar').append('<div id = "webform-ipe-unsaved-edits">This form has unsaved changes</div>');
                  $('body').addClass('unsaved-ipe');
                }

                // if we were going to allow saving of individual components, it would happen here
                //  if(typeof($('#editNow input:checked').val()) !== "undefined") {
                //   componentsView.save();
                // }

                $.unblockUI();
                return false;
              });

              $('button[data-action="remove"]').on('click', function(){
                //App.Handlers.blockMsgResize('remove');
              });
              // Clicking on the x closes
              $('.closeUI').click(function(){
                $.unblockUI();
                return false;
              });
              // Clicking off closes
              $('.blockOverlay').attr('title','Click to unblock').click(function(){
                $.unblockUI();
              });
              if (typeof(afterBuild) !== 'function') {
                return;
              } else {
                afterBuild();
              }
              App.Handlers.blockMsgResize();
            }
          });
        }
        // Sets default cursor for modals
        $.blockUI.defaults.css.cursor = 'default';

        // Ajax function to build date fields
        App.Handlers.createDate = function(type, form) {
          //if we've changed the date field, go get the new html
          //Called from blockUIbuilder rather than from the view because of rerendering issues on "change"
          if(type == 'date' && (form.model.hasChanged('extra.start_date') || form.model.hasChanged('extra.end_date') || form.model.hasChanged('extra.datepicker') || form.model.hasChanged('extra.year_textfield'))) {
            var callbackPath = Drupal.settings.webform_ipe.date_callback;
            $('#editForm button').before('<div class="ajax-progress" style = "clear:both"><div class="throbber">&nbsp;</div>preparing date preview</div><p>');
            $('.throbber').show();
            $('#editForm .save').hide();

            jQuery.ajax({
              type: "POST",
              url: callbackPath,
              async: false,
              data: {
                extra:form.model.get('extra'),
                name:form.model.get('name'),
                type:'date',
                pid:form.model.get('pid'),
                value:form.model.get('value'),
                mandatory: form.model.get('mandatory'),
                form_key: form.model.get('form_key'),
                weight: form.model.get('weight'),
              },
              dataType: 'json',
              success: function(data) {
                // return data[0];
                  var form_key = form.model.get('form_key');
                  html = $(data[0]);
                  html.find('div.form-item').attr('name', '[' + form_key + ']');
                  html.find('div.form-item').addClass('control-group').removeClass('form-item')
                  //html.find('label').remove();
                  form.model.set('date_html', html[0].innerHTML);
                  $('.ajax-progress').remove();
                  if($('#webform-ipe-unsaved-edits').length < 1) {
                    $('.save-and-preview').show();
                    $('#admin-bar').append('<div id = "webform-ipe-unsaved-edits">This form has unsaved changes</div>');
                    $('body').addClass('unsaved-ipe');
                  }

              $.unblockUI();
              return false;
              },
              error: function(xhr, textStatus, error){
                 form.model.set('date_html', '<div class ="control-group">Could not generate a date component.</div>');
                 console.log(textStatus);
                  $.unblockUI();
                  return false;
              }
            });
            return false;
          }
        }


        /*
         * SETUP INITIAL WEBFORM PAGE LOAD.
         * Create a collection of models present on the webform page.
         */

        // shortcut to components
        window.comps = Drupal.settings.webform_ipe.components;

        // Create a new Collection
        window.componentCollection = new App.Collections.Components;

        /*
         * This iterates through each component in Drupal.settings.webform_webform-ipe.components and
         * builds a model of it depending on the input type. It also adds attributes to DOM elements
         * for the input's cid and pid - this is how we target DOM elements from individual models.
         */

        // most likely, some of these selectors will be too greedy in the wild
        // "*=" is needed in some places rather than "$=" because some fields
        // have non-standard "name" structures
        _.each(window.comps, function(item){
          if (!item || item.type == 'hidden') return;
          switch (item.type) {
            case 'fieldset':
            case 'payment_fields':
            case 'payment_method':
              var fieldsetKey = item.form_key.replace(/_/g,'-');
              //webform fieldsets
              $('fieldset[id$="'+fieldsetKey+'"]').attr('data-cid',item.cid);
              $('fieldset[id$="'+fieldsetKey+'"]').attr('data-pid',item.pid);

              //form_layout containers
              $('div[id$="'+fieldsetKey+'"]').attr('data-cid',item.cid);
              $('div[id$="'+fieldsetKey+'"]').attr('data-pid',item.pid);
              //if (item.form_key == 'left_sidebar' || item.form_key == 'right_sidebar' || item.form_key == 'top_region' || item.form_key == 'right_region' || item.form_key == 'bottom_region' || item.form_key == 'left_region') return;
              var newModel = new App.Models.FieldsetComponent(item);
              break;
            case 'textfield':
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-cid',item.cid);
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-pid',item.pid);
              var newModel = new App.Models.TextComponent(item);
              break;
            case 'email':
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-cid',item.cid);
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-pid',item.pid);
              var newModel = new App.Models.MailComponent(item);
              break;
            case 'textarea':
              if(item.form_key != 'markup') {
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-cid',item.cid);
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-pid',item.pid);
              var newModel = new App.Models.TextareaComponent(item);
              }
              else {
                $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-cid',item.cid);
                $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-pid',item.pid);
                var newModel = new App.Models.MarkupComponent(item);
              }
              break;
            case 'markup':
              var id = item.form_key.replace(/_/g,'-');
              $('div[id$="'+id+'"]').parent().attr('data-cid',item.cid);
              $('div[id$="'+id+'"]').parent().attr('data-pid',item.pid);
              var newModel = new App.Models.MarkupComponent(item);
              break;
            case 'number':
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-cid',item.cid);
              $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-pid',item.pid);
              var newModel = new App.Models.NumberComponent(item);
              break;
            case 'date':
                $('[name*="['+item.form_key+']"]').closest('.control-group').parents('.control-group').attr('data-cid',item.cid);
                $('[name*="['+item.form_key+']"]').closest('.control-group').parents('.control-group').attr('data-pid',item.pid);
                $('[name*="['+item.form_key+']"]').closest('.control-group').parents('.control-group').addClass('date-field');
              var newModel = new App.Models.DateComponent(item);
              break;
            case 'select':
              if(item.extra.aslist == true) {
                $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-cid',item.cid);
                $('[name$="['+item.form_key+']"]').closest('.control-group').attr('data-pid',item.pid);
              }
              else {
                $('[name*="['+item.form_key+']"]').closest('.control-group').parents('.control-group').attr('data-cid',item.cid);
                $('[name*="['+item.form_key+']"]').closest('.control-group').parents('.control-group').attr('data-pid',item.pid);
              }
                var newModel = new App.Models.SelectComponent(item);
              break;
            default:
              var newModel = new App.Models.Component(item);
              break;
          }
          console.log('')
          window.componentCollection.add(newModel);
        });
        /*
         * shows the component collection after building
         */

        /*
         * After iterating through each component, create its model and add it to the model collection, we then pass that collection into a view
         * which controls all components.
         * There are two Views, one for individual components (to handle events on that input)
         * and one for the collection as a whole (to handle added and deleted items).
         */
        var componentsView = new App.Views.AllComponents({ collection: window.componentCollection });

        /*
         * ADD ADMIN BAR AND TOGGLE TO THE WEBFORM PAGE
         * The actual content of the bar lives in template.html
         */

        $('body').prepend(_.template($('#adminBar').html()));
  
        $('.accordion-header','#admin-bar').each(function(i){
          App.Vars['accordion-'+i] = false;
        });
        /*
         * Admin bar open/close of accordion menus
         */
        $('.accordion-header','#admin-bar').click(function(e){
          var index = $(this).closest('.accordion').index();
          if (App.Vars['accordion-'+index] == true) return;
          App.Vars['accordion-'+index] = true;
          e.preventDefault();
          if ($(this).hasClass('closed')) {
            $(this).removeClass('closed');
            $(this).siblings('.accordion-options').animate({
              height: 'toggle'
            }, 400,'swing',function(){ App.Vars['accordion-'+index] = false; });
          } else {
            $(this).addClass('closed');
            $(this).siblings('.accordion-options').animate({
              height: 'toggle'
            }, 400, 'swing',function(){ App.Vars['accordion-'+index] = false; });
          }
        });
        // Initially close items
        $('.accordion-header','#admin-bar').each(function(){
          var index = $(this).closest('.accordion').index();
          if (App.Vars['accordion-'+index] == true) return;
          App.Vars['accordion-'+index] = true;

          if ($(this).hasClass('closed')) {
            $(this).siblings('.accordion-options').animate({
              height: 'toggle'
            }, 400, 'swing',function(){ App.Vars['accordion-'+index] = false; });
          }
        });

        /*
         * DEFINE THE ADMIN BAR DROPDOWN MENU CLICK EVENTS
         */
        $('.add-textfield','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.TextComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-email','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.MailComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-select','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.SelectComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-textarea','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.TextareaComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-fieldset','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.FieldsetComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-markup','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.MarkupComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-number','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.NumberComponent();
          window.componentCollection.add(newModel);
        });
        $('.add-date','#admin-bar').click(function(e){
          e.preventDefault();
          var newModel = new App.Models.DateComponent();
          window.componentCollection.add(newModel);
        });

        $('.save-and-preview','#admin-bar').click(function(e){
           e.preventDefault();
           componentsView.save();
          $('.save-and-preview','#admin-bar').css('background', '#ccc');
        });

        // Menu fixed position
        $('.menu-fixed','#admin-bar').click(function(e){
          e.preventDefault();
          var checkBox = $("input#admin-bar-fixed", this);
          checkBox.attr('checked', !checkBox.attr('checked'));
          var adminBar = $(this).closest('#admin-bar');
          if (adminBar.hasClass('fixed')) {
            adminBar.removeClass('fixed');
          } else {
            adminBar.addClass('fixed');
          }
        });

        // toggle empty sidebars
        $('.sidebars-hide','#admin-bar').click(function(e){
          e.preventDefault();
          var hideBox = $("input#sidebars-hide", this);
          $('.sidebars-hide','#admin-bar').toggleClass('active')
          hideBox.attr('checked', !hideBox.attr('checked'));
          var hideBar = $('div.fieldset.right-sidebar, div.fieldset.left-sidebar');
          hideBar.each(function( index ) {
            if (!$(this).hasClass('show') && $('.sidebars-hide','#admin-bar').hasClass('active')) {
              $('body').addClass('ipe-both-sidebars');
              $(this).css({'min-height': '600px', 'width': '270px'}).addClass('show').show()
            } else if(!$('.sidebars-hide','#admin-bar').hasClass('active')){
              var num = $(this).not(':has("*")').length;
              $(this).not(':has("*")').removeClass('show').hide();
              if(num == 0) {
                $('body').removeClass('ipe-both-sidebars');
              } else if (num == 1) {
                $('body').removeClass('ipe-both-sidebars');
                $('body').addClass('ipe-one-sidebar');
              }
            }
          });
          App.Handlers.reorderItems();
          App.Handlers.reorderFieldsets();
        });
        

        //This class needs to be added on page load, not in functions below  
        $('.webform-component-fieldset, div.fieldset.form-layouts, div.fieldset.sidebar').not('[id$="payment-fields"]').addClass('webform-ipe-container');
        var has_form_layout =  $('div.fieldset.form-layouts.webform-ipe-container').length;
        if(has_form_layout == 0) {
          $('.webform-client-form').wrap('<div class = "fieldset form-layouts webform-ipe-container" data-cid = "0"></div>');
        }

        /*
         * RE-ORDER ITEMS AND FIELDSETS
         */

        $('#sidebar-toggle').hide();

        $('.reorder-items-fieldsets', '#admin-bar').click(function(e) {
          e.preventDefault();
          if ($(this).parent().is('.active')) {
            App.Handlers.sortOff($(this));
            $('#sidebar-toggle').hide();
            var hideBox = $("input#sidebars-hide");
            $('.sidebars-hide','#admin-bar').removeClass('active')
            hideBox.attr('checked', false);
            $('div.fieldset.right-sidebar, div.fieldset.left-sidebar').not(':has("*")').removeClass('show').hide();

          } else {
            App.Handlers.sortOn($(this));
            $('#sidebar-toggle').show();
          }
        });
        $('.dropdown-toggle').click(function(e) {
          e.preventDefault();
          if($('.reorder-items-fieldsets').parent().hasClass('active')) {
            App.Handlers.sortOff($('.reorder-items-fieldsets'));
          }
        });

        /*
         * Re-order Items
         */
        App.Handlers.reorderItems = function() {
          $('.ipe-inner').sortable({
            connectWith: '.ipe-inner, #block-system-main div.form-layouts, div.fieldset.sidebar.show',
            cancel: '.disabled, .dummy',
            placeholder: 'sortable-placeholder',
            items: 'div.control-group:not(".disabled")',
            cursorAt: { left: 5, top: 15 },
            start: function(event, ui) {
              ui.item.css({'height':'50px', 'overflow':'hidden'})
              ui.item.css({'width':'120px', 'overflow':'hidden'})
            },
            stop: function (event, ui) {
              ui.item.css({'height':'auto', 'overflow':'visible'})
              ui.item.css({'width':'auto', 'overflow':'hidden'})
              ui.item.trigger('drop');
              $('div.fieldset.form-layouts:not(:has("*"))').css('min-height', '60px');
              if($(this).find(".control-group").length == 0 &! $(this).hasClass('fake-wrapper')) {
                $(this).append('<div class = "dummy" style="height: 40px"></div>')
              }
              ui.item.siblings(".dummy").remove();
              App.Handlers.wrapOrphans();
              if(!$('.sidebars-hide','#admin-bar').hasClass('active')) {
                $('.fieldset.sidebar').not(':has("*")').removeClass('show').hide()
              }
            },
          });

          $('.webform-component-fieldset').disableSelection();
        };
        /*
         * Re-order Fieldsets
         */
        App.Handlers.reorderFieldsets = function() {
          $('.ipe-outer').sortable({
            cancel:'.disabled',
            placeholder: 'sortable-placeholder',
            connectWith: '.ipe-outer',
            items: '.ipe-inner, div.control-group:not("fieldset.webform-component-fieldset div.control-group"):not(".disabled")',
            //items: '.ipe-inner',
            handle: 'legend',
            cursorAt: { left: 5, top: 15 },
            start: function(event, ui) {
              height = $(ui.item[0]).height();
              ui.item.css({'height':'20px', 'overflow':'hidden'})
              ui.item.css({'width':'120px', 'overflow':'hidden'})
            },
            stop: function (event, ui) {
              ui.item.css({'height':'auto', 'overflow':'visible'})
              ui.item.css({'width':'auto', 'overflow':'hidden'})
              ui.item.trigger('drop');
              $('div.fieldset.form-layouts:not(:has("*"))').css('min-height', '60px')
              ui.item.siblings(".dummy").remove();
              App.Handlers.wrapOrphans();
              if(!$('.sidebars-hide','#admin-bar').hasClass('active')) {
                $('.fieldset.sidebar').not(':has("*")').removeClass('show').hide()
              }
              //App.Handlers.setItems();
             },
          });
          $('#block-system-main div.form-layouts').disableSelection();
        };
         
        //dynamically swap items to eliminate fieldset jitter 
     //    App.Handlers.setItems = function() {
     //      $('fieldset.webform-component-fieldset legend').on('mousedown', function(){
     //        $(".ipe-outer").sortable( 'option', 'items', 'fieldset.webform-component-fieldset, div.control-group:not("fieldset.webform-component-fieldset div.control-group")');
     //      });

     //      $('.ipe-outer div.control-group:not("fieldset.webform-component-fieldset div.control-group")').on('mousedown', function(){
     //        $( ".ipe-outer" ).sortable( "option", "items", "fieldset.webform-component-fieldset, div.dummy, div.control-group:not('.disabled')");
     //      });
     //    };

        //turn the sort on and off
        App.Handlers.sortOn = function(button) {
          button.parent().toggleClass('active');
          //turn sorting on

          //$('div.fundraiser_submit_message').wrap('<div class = "ipe-action sortable"><div>');
          //$('div.form-actions').appendTo('.fundraiser_submit_message');
  
          $('div.fieldset.form-layouts').not(':has("*")').show()
          $('div.fieldset.sidebar').addClass('show').css('min-height', '600px').show()
          $('div.fieldset.sidebar').not(':has("*")').removeClass('show').hide();
          App.Handlers.addSortClasses();
          App.Handlers.wrapOrphans();
          App.Handlers.reorderItems(); //initialize the sortables
          App.Handlers.reorderFieldsets();
          //App.Handlers.setItems();
          $('.ipe-inner').sortable("option","disabled", false);
          $('.ipe-outer').sortable("option","disabled", false);
            // $('.layout-row .webform-ipe-container.editor-on.show, .layout-row > .webform-ipe-container.editor-on, .layout-row > .webform-ipe-container.editor-on > .webform-ipe-container').sortable("option","disabled", false);
            // $('.layout-row > .webform-ipe-container.editor-on, .layout-row .webform-ipe-container.sidebar.editor-on.show').sortableNew('enable');
        }

        App.Handlers.sortOff = function(button) {
          button.parent().toggleClass('active');
          //disable sorting
          //$('.span3.region-empty').not(':has("*")').css({'min-height': '0px', 'width': '0px'})
          // $('.layout-row .webform-ipe-container.editor-on.show, .layout-row > .webform-ipe-container.editor-on, .layout-row > .webform-ipe-container.editor-on > .webform-ipe-container').sortable("option","disabled", true);
          // $('.layout-row > .webform-ipe-container.editor-on, .layout-row  .webform-ipe-container.sidebar.editor-on').sortableNew('disable');
          $('div.fieldset.form-layouts, div.fieldset.sidebar').not(':has("*")').hide();
          $('.webform-component-fieldset, div.fieldset.form-layouts, div.fieldset.sidebar, body').removeClass('editor-on');
          $('.ipe-inner').sortable("option","disabled", true);
          $('.ipe-outer').sortable("option","disabled", true);
          $('.webform-component-fieldset').css('width', '')
        }

        App.Handlers.addSortClasses = function() {
          $('.webform-component-fieldset, div.fieldset.form-layouts, div.fieldset.sidebar').not('[id$="payment-fields"]').addClass('webform-ipe-container');
          $('.webform-component-fieldset, div.fieldset.form-layouts, div.fieldset.sidebar, body').addClass('editor-on');
          $('.webform-component-fieldset  div.control-group, div.fieldset div.control-group').not('.fundraiser-payment-fields div.control-group').addClass('sortable');
          $('#block-system-main div.form-layouts, div.fieldset.sidebar').addClass('ipe-outer');
          $('fieldset.webform-component-fieldset').addClass('ipe-inner');
          $('.webform-component-fieldset').disableSelection();
          $('div.control-group input[type="radio"]').parent().addClass('disabled');
          $('div.control-group input[type="checkbox"]').parent().addClass('disabled');
          $('div.control-group input[type="checkboxes"]').parent().addClass('disabled');
          $('[id*="payment-fields"]').addClass('disabled');
          $('[id*="payment-fields"] .control-group').addClass('disabled');
          $(".date-field .control-group").addClass('disabled');
          $('.fundraiser-payment-fields').addClass('disabled');
          $('.disabled').removeClass('sortable');
        }

        App.Handlers.wrapOrphans = function() {
          var orphan = $('.ipe-outer div.control-group:not("fieldset.webform-component-fieldset div.control-group"):not(".disabled")');
          orphan.each(function(){
            if(!$(this).parent().hasClass('fake-wrapper')) {
              $(this).wrap('<div class = "ipe-inner fake-wrapper"></div>');
            }
          });
          $('.fake-wrapper').not(':has("*")').remove();
          App.Handlers.reorderItems();
          App.Handlers.reorderFieldsets();
        }  

        /** popup previews **/

        $('body').wrapInner('<div class = "zoot"></div>');
        $('.landscape-preview').magnificPopup({
          items: {
            src: '.zoot',
            type: 'inline',
          },
          mainClass: 'tabletland-preview',
          callbacks: {
            close: function() {
              $('.zoot').removeClass('mfp-hide');
            },
          },
                  });

        $('.portrait-preview').magnificPopup({
          items: {
            src: '.zoot',
            type: 'inline',
          },
          mainClass: 'portrait-preview',
          callbacks: {
            close: function() {
              $('.zoot').removeClass('mfp-hide');
            },
          },

        });
        $('.desktop-preview').magnificPopup({
          items: {
            src: '.zoot',
            type: 'inline',
          },
          mainClass: 'desktop-preview',
          callbacks: {
            close: function() {
              $('.zoot').removeClass('mfp-hide');
            },
          },

        });
        $('.phone-preview').magnificPopup({
          items: {
            src: '.zoot',
            type: 'inline',
          },
          mainClass: 'phone-preview',
          callbacks: {
            close: function() {
              $('.zoot').removeClass('mfp-hide');
            },
          },

        });

        $('.ipe-popup').on('mousedown', function(e){
          $.magnificPopup.close()
          $(this).magnificPopup('open')
        });
      }); // End window.ready
    }
  }
})(jQuery);
