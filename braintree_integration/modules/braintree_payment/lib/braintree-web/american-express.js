!function(e){if("object"==typeof exports&&"undefined"!=typeof module)module.exports=e();else if("function"==typeof define&&define.amd)define([],e);else{var r;r="undefined"!=typeof window?window:"undefined"!=typeof global?global:"undefined"!=typeof self?self:this,(r.braintree||(r.braintree={})).americanExpress=e()}}(function(){return function e(r,t,o){function n(s,c){if(!t[s]){if(!r[s]){var a="function"==typeof require&&require;if(!c&&a)return a(s,!0);if(i)return i(s,!0);var E=new Error("Cannot find module '"+s+"'");throw E.code="MODULE_NOT_FOUND",E}var u=t[s]={exports:{}};r[s][0].call(u.exports,function(e){var t=r[s][1][e];return n(t?t:e)},u,u.exports,e,r,t,o)}return t[s].exports}for(var i="function"==typeof require&&require,s=0;s<o.length;s++)n(o[s]);return n}({1:[function(e,r,t){"use strict";function o(e){this._client=e.client}var n=e("../lib/error"),i=e("../lib/deferred"),s=e("./errors"),c=e("../lib/throw-if-no-callback");o.prototype.getRewardsBalance=function(e,r){return c(r,"getRewardsBalance"),r=i(r),e.nonce?void this._client.request({method:"get",endpoint:"payment_methods/amex_rewards_balance",data:{_meta:{source:"american-express"},paymentMethodNonce:e.nonce}},function(e,t){e?r(new n({type:s.AMEX_NETWORK_ERROR.type,code:s.AMEX_NETWORK_ERROR.code,message:"A network error occurred when getting the American Express rewards balance.",details:{originalError:e}})):r(null,t)}):void r(new n({type:s.AMEX_NONCE_REQUIRED.type,code:s.AMEX_NONCE_REQUIRED.code,message:"getRewardsBalance must be called with a nonce."}))},o.prototype.getExpressCheckoutProfile=function(e,r){return c(r,"getExpressCheckoutProfile"),r=i(r),e.nonce?void this._client.request({method:"get",endpoint:"payment_methods/amex_express_checkout_cards/"+e.nonce,data:{_meta:{source:"american-express"},paymentMethodNonce:e.nonce}},function(e,t){e?r(new n({type:s.AMEX_NETWORK_ERROR.type,code:s.AMEX_NETWORK_ERROR.code,message:"A network error occurred when getting the American Express Checkout nonce profile.",details:{originalError:e}})):r(null,t)}):void r(new n({type:s.AMEX_NONCE_REQUIRED.type,code:s.AMEX_NONCE_REQUIRED.code,message:"getExpressCheckoutProfile must be called with a nonce."}))},r.exports=o},{"../lib/deferred":5,"../lib/error":7,"../lib/throw-if-no-callback":8,"./errors":2}],2:[function(e,r,t){"use strict";var o=e("../lib/error");r.exports={AMEX_NONCE_REQUIRED:{type:o.types.MERCHANT,code:"AMEX_NONCE_REQUIRED"},AMEX_NETWORK_ERROR:{type:o.types.NETWORK,code:"AMEX_NETWORK_ERROR"}}},{"../lib/error":7}],3:[function(e,r,t){"use strict";function o(e,r){var t;return E(r,"create"),r=s(r),null==e.client?void r(new n({type:c.INSTANTIATION_OPTION_REQUIRED.type,code:c.INSTANTIATION_OPTION_REQUIRED.code,message:"options.client is required when instantiating American Express."})):(t=e.client.getConfiguration().analyticsMetadata.sdkVersion,t!==a?void r(new n({type:c.INCOMPATIBLE_VERSIONS.type,code:c.INCOMPATIBLE_VERSIONS.code,message:"Client (version "+t+") and American Express (version "+a+") components must be from the same SDK version."})):void r(null,new i(e)))}var n=e("../lib/error"),i=e("./american-express"),s=e("../lib/deferred"),c=e("../errors"),a="3.5.0",E=e("../lib/throw-if-no-callback");r.exports={create:o,VERSION:a}},{"../errors":4,"../lib/deferred":5,"../lib/error":7,"../lib/throw-if-no-callback":8,"./american-express":1}],4:[function(e,r,t){"use strict";var o=e("./lib/error");r.exports={CALLBACK_REQUIRED:{type:o.types.MERCHANT,code:"CALLBACK_REQUIRED"},INSTANTIATION_OPTION_REQUIRED:{type:o.types.MERCHANT,code:"INSTANTIATION_OPTION_REQUIRED"},INCOMPATIBLE_VERSIONS:{type:o.types.MERCHANT,code:"INCOMPATIBLE_VERSIONS"},METHOD_CALLED_AFTER_TEARDOWN:{type:o.types.MERCHANT,code:"METHOD_CALLED_AFTER_TEARDOWN"},BRAINTREE_API_ACCESS_RESTRICTED:{type:o.types.MERCHANT,code:"BRAINTREE_API_ACCESS_RESTRICTED",message:"Your access is restricted and cannot use this part of the Braintree API."}}},{"./lib/error":7}],5:[function(e,r,t){"use strict";r.exports=function(e){return function(){var r=arguments;setTimeout(function(){e.apply(null,r)},1)}}},{}],6:[function(e,r,t){"use strict";function o(e,r){return r=null==r?"":r,e.reduce(function(e,t){return e[t]=r+t,e},{})}r.exports=o},{}],7:[function(e,r,t){"use strict";function o(e){if(!o.types.hasOwnProperty(e.type))throw new Error(e.type+" is not a valid type.");if(!e.code)throw new Error("Error code required.");if(!e.message)throw new Error("Error message required.");this.name="BraintreeError",this.code=e.code,this.message=e.message,this.type=e.type,this.details=e.details}var n=e("./enumerate");o.prototype=Object.create(Error.prototype),o.prototype.constructor=o,o.types=n(["CUSTOMER","MERCHANT","NETWORK","INTERNAL","UNKNOWN"]),r.exports=o},{"./enumerate":6}],8:[function(e,r,t){"use strict";var o=e("./error"),n=e("../errors");r.exports=function(e,r){if("function"!=typeof e)throw new o({type:n.CALLBACK_REQUIRED.type,code:n.CALLBACK_REQUIRED.code,message:r+" must include a callback function."})}},{"../errors":4,"./error":7}]},{},[3])(3)});
