(function ($) {
  Drupal.behaviors.fundraiserDesignations = {
    attach: function(context, settings) {

      if (Drupal.settings.fundraiserDesignations) {
        new Drupal.fundraiserDesignations();
      }
    }
  };

  /**
   * Set the amounts on page load
   */
  Drupal.fundraiserDesignations = function() {
    this.productPrices = Drupal.settings.fundraiserDesignations.productPrices;
    this.setAmounts();
    this.productSelect();
  }

  /**
   * Set the amounts on select
   */
  // Iterate over each field in the settings and add listener
  Drupal.fundraiserDesignations.prototype.productSelect = function() {
    var self = this;
    $.each(self.productPrices, function(productId, price) {
      $('#product-' + productId + '-product-quant').change(function() {
        self.setAmounts();
      });
    });

    $('#fundraiser-products-extra-donation').keyup(function() {
      self.setAmounts();
    });

  }

  /**
   * Update the various HTML elements with our amounts
   */
  Drupal.fundraiserDesignations.prototype.setAmounts = function() {
    var self = this,
    total = self.calcTotal(),
    extra = self.calcExtra(),
    totalQuantity = self.calcQuantity();
    $('#fundraiser-products-total-cost').text(Drupal.settings.fundraiser.currency.symbol + (total).formatMoney(2, '.', ','));
    $('#fundraiser-products-extra-donation-display').text(Drupal.settings.fundraiser.currency.symbol + (extra).formatMoney(2, '.', ','));
    $("input[name='submitted[amount]']").val((total).formatMoney(2, '.', ''));
    $('#fundraiser-products-total-quant').text(totalQuantity);
    $.each(self.productPrices, function(productId, price) {
      $('#product-' + productId + '-products-total').text(price.currency.symbol + (self.calcField(productId, price.amount)).formatMoney(2, '.', ','));
    });
  }

  /**
   * Calculate the value of a field
   */
  Drupal.fundraiserDesignations.prototype.calcField = function(productId, price) {
    return $('#product-' + productId + '-product-quant').val() * parseFloat(price.replace(/\,/g,''));
  }

  /**
   * Calculate the total amount
   */
  Drupal.fundraiserDesignations.prototype.calcTotal = function() {
    var self = this;
    var total = 0;
    $.each(self.productPrices, function(productId, price) {
      total = total + (parseFloat(price.amount.replace(/\,/g,'')) * $('#product-' + productId + '-product-quant').val());
    });
    if ($('#fundraiser-products-extra-donation').val()){
      total = total + parseFloat($('#fundraiser-products-extra-donation').val().replace(/\,/g,''));
    }
    return total;
  }

  /**
   * Calculate the quantity of products selected
   */
  Drupal.fundraiserDesignations.prototype.calcQuantity = function() {
    var self = this;
    var quantity = 0;
    $.each(self.productPrices, function(productId, price) {
       quantity = quantity + Number($('#product-' + productId + '-product-quant').val());
    });
    return quantity;
  }

  /**
   * Format values as money
   *
   * http://stackoverflow.com/a/149099/1060438
   */
  Number.prototype.formatMoney = function(c, d, t) {
    var n = this, c = isNaN(c = Math.abs(c)) ? 2 : c, d = d == undefined ? "," : d, t = t == undefined ? "." : t, s = n < 0 ? "-" : "", i = parseInt(n = Math.abs(+n || 0).toFixed(c)) + "", j = (j = i.length) > 3 ? j % 3 : 0;
    return s + (j ? i.substr(0, j) + t : "") + i.substr(j).replace(/(\d{3})(?=\d)/g, "$1" + t) + (c ? d + Math.abs(n - i).toFixed(c).slice(2) : "");
  };

})(jQuery);
