/**
 * Set up the handler to change the currency symbol when the user changes their currency selection
 */
Drupal.behaviors.currencySwitcher =  {
  attach: function(context) { (function($) {

    // First find out what the current donation type is.
    var currentCode = Drupal.settings.fundraiser_multi_currency.original;
    // On change of the input update the labels for all fields.
    $('select[id$="-currency"]', context).change(function() {
      // Check what was selected
      var newCode = $(this).val();
      // If the list of currencies is available.
      if (Drupal.settings.fundraiser_multi_currency.currencies[newCode] !== undefined
        && Drupal.settings.fundraiser_multi_currency.currencies[currentCode] !== undefined ) {
        // Load the symbol for the currency type
        var oldSymbol = Drupal.settings.fundraiser_multi_currency.currencies[currentCode].symbol;
        var newSymbol = Drupal.settings.fundraiser_multi_currency.currencies[newCode].symbol;
        // Update the other field.
        $('div[id$="--other-amount"]', context).find('.field-prefix').text(newSymbol);
        // Minimum amount description under Other field
        $('[id$="--other-amount"]', context).find('div.description').each(function() {
          var $this = $(this);
          $this.html($this.html().replace(oldSymbol, newSymbol));
        });
        // Loop over each ask amount and update the symbol
        $('div[id$="--amount"]', context).find('.form-radios .form-item label').each(function() {
          var $this = $(this);
          $this.html($this.html().replace(oldSymbol, newSymbol));
        });
        // Update the previous currency val so we know what to replace next time
        Drupal.settings.fundraiser_multi_currency.original = newCode;
        currentCode = newCode;
      }    
    });

  })(jQuery); }
};
