/**
 * Set up the handler to change the currency symbol when the user changes their currency selection
 */
Drupal.behaviors.currencySwitcher = function(context) {
  // Find the wrapper for the currency field
  // Since the ID of the element will change if it's moved in the structure of
  // the webform, use the "attribute ends with" selector to find it
  var currency_component = $('div[id$="-currency"]', context);

  // Check the current currency code to save for later
  if (!Drupal.previousCurencyCode) {
    // If currency selection is radio buttons, find the one that's checked
    if (currency_component.hasClass('webform-component-radios')) {
      var previous_currency_code = $(':radio:checked', currency_component).val();
    }
    // Otherwise, it's a dropdown, so just get val()
    else {
      var previous_currency_code = $(':input', currency_component).val();
    }

    if (Drupal.settings.currencies[previous_currency_code]) {
      Drupal.previousCurencyCode = previous_currency_code;
    }
  }

  // On change of the input (whether dropdown or radio button)
  $(':input', currency_component).change(function() {
    // Check what was selected
    var currency_code = $(this).val();

    // If their are details available about the currency...
    if (Drupal.settings.currencies[currency_code]) {
      // Load the symbol for the currency type
      var currency_detail = Drupal.settings.currencies[currency_code];

      var oldSymbol = Drupal.settings.currencies[Drupal.previousCurencyCode].symbol;
      var newSymbol = currency_detail.symbol


      // Find the Other amount field
      // Since the ID of the element will change if it's moved in the structure of
      // the webform, use the "attribute ends with" selector to find it
      var other_amount_field_prefix = $('div[id$="amount-wrapper"]', context).find('.field-prefix');
      other_amount_field_prefix.text(newSymbol);

      // Minimum amount description under Other field
      var min_field = $('div[id$="amount-wrapper"]', context).find('div.description');
      min_field.each(function() {
        var $this = $(this);
        $this.html($this.html().replace(oldSymbol, newSymbol));
      });


      // Find the ask amounts
      // If at top level, id is webform-component-amount
      // If within fieldset, id will end with --amount
      var ask_amount_container = $('div[id$="--amount"], #webform-component-amount', context);
      var amount_field_prefixes = ask_amount_container.find('.form-radios label');

      // Replace the previous currency symbol of each row with the new one
      // Check which was checked so it can be re-checked after the text replace
      var selectedRadio = ask_amount_container.find(':radio:checked', context).val();

      // Loop over each ask amount and update the symbol
      amount_field_prefixes.each(function() {
        var $this = $(this);
        $this.html($this.html().replace(oldSymbol, newSymbol));
      });

      // Reset the checked radio button
      ask_amount_container.find(':radio[value=' + selectedRadio + ']').attr('checked', 'checked');

      // Update the previous currency val so we know what to replace next time
      Drupal.previousCurencyCode = currency_code;
    }
  });
};