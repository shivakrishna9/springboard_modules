(function ($) {

  Drupal.behaviors.fundraiserDesignations = {
    attach: function(context, settings) {
        new Drupal.fundraiserDesignations();
    }
  };

  /**
   * Set the amounts on page load
   */
  Drupal.fundraiserDesignations = function() {
    this.fundGroups = $('.designation-group-wrapper');
    this.cart = $('.fundraiser-designation-cart-wrapper');
    this.cartTemplate = '<tr class="cart-fund-row"><td class="fund-cancel">x</td><td class="fund-name"></td><td class="fund-amount"></td> </tr>';
    this.addListener();
    this.cancelButton();
  }

  /**
   * Set the amounts on select
   */
  // Iterate over each field in the settings and add listener
  Drupal.fundraiserDesignations.prototype.addListener = function() {

    var self = this;
    $.each(self.fundGroups, function(key, item) {

      var fundGroup = $(item);
      var fundGroupId = item['id'].replace('designation-group-', '')
      var selector = fundGroup.find('select');
      var defaultAmts = $('div[id*="default-amounts-"]', fundGroup);
      var recurAmts = $('div[id*="recurring-amounts-"]', fundGroup);
      var otherAmt = $('input[type="text"]', fundGroup);

      otherAmt.keyup(function() {
        //@todo clear radios
      });

      $('input[type="submit"]', fundGroup).click(function() {
        button = $(this);
        self.addFund(button, fundGroupId, selector, defaultAmts, recurAmts, otherAmt);
        return false;
      });
    });
  };

  Drupal.fundraiserDesignations.prototype.addFund = function(button, fundGroupId, selector, defaultAmts, recurAmts, otherAmt) {
    var self = this;
    self.validateFund(fundGroupId, selector, defaultAmts, recurAmts, otherAmt);
    var amt = 0;
    if (defaultAmts.length > 0 && defaultAmts.is(':visible')) {
      amt = $(defaultAmts).find('input:checked').val();
    }
    if (recurAmts.length > 0 && recurAmts.is(':visible')) {
      amt = $(recursAmts).find('input:checked').val();
    }
    if (otherAmt.val()) {
      amt = otherAmt.val();
    }

    var fundId = selector.val()
    var fundName = $('option:selected', selector).text();
    var newRow = $(self.cartTemplate);
    $('.fund-amount', newRow).text(amt)
    $('.fund-name', newRow).text(fundName);
    newRow.insertBefore('.cart-total-row').hide().show(300);
    if($(".cart-fund-empty").is(':visible')) {
      $(".cart-fund-empty").hide();
    }
    self.cancelButton();
    self.setAmounts();

  }

  Drupal.fundraiserDesignations.prototype.validateFund = function(fundGroupId, selector, defaultAmts, recurAmts, otherAmt) {
    var self = this;
  }

  /**
   * Set the amounts on select
   */
    // Iterate over each field in the settings and add listener
  Drupal.fundraiserDesignations.prototype.cancelButton = function() {
    var self = this;
    $.each($('tr', self.cart), function(key, row) {
      $('.fund-cancel', this).click(function() {
         $(row).hide(300, function(){
           $(row).remove();
           self.setAmounts();
           if ($('tr.cart-fund-row', self.cart).length == 1) {
             $(".cart-fund-empty").show(100);
           }
         });
      });
    });
  };

  /**
   * Update the various HTML elements with our amounts
   */
  Drupal.fundraiserDesignations.prototype.setAmounts = function() {
    var self = this,
    total = self.calcTotal();
    console.log(total);
    $('#cart_total').val(Drupal.settings.fundraiser.currency.symbol + (total).formatMoney(2, '.', ','));
    $("input[name='submitted[amount]']").val((total).formatMoney(2, '.', ''));
  }

  /**
   * Calculate the total amount
   */
  Drupal.fundraiserDesignations.prototype.calcTotal = function() {
    var self = this;
    var total = 0;
    $.each($('td.fund-amount', self.cart), function(i, price) {
      total = total + parseInt($(price).text());
    });
    return total;
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
