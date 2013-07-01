Hi!
Welcome to the wonderful world of Litle. Here's what I've got so far.

a) This module is deriving it's footprint from the Authorize.net commerce module as the next closest parallel. So you'll see that scattered through here, if need be pull a copy of Auth's dev branch and copy and paste as you convert. :) Ideally if you can create a module that would be acceptable for use in contrib, that would rock.

b) Litle has good documentation as it turns out! I've included that under /docs - It *ALSO* has a fully developed object implementation of their service. So instead of having to construct all the curl goodies, we just use their object placed under a lib path. I've got wrappers to the object to make that process easy.

c) Standard Litle uses a hardcoded file to provide user, password, merchant ID and all that goodness to the object. But! It also allows for the query passed into it's system to override that. Which is exactly what _commerce_litle_get_litle_config() provides to the calls from the gateway information set for it.

d) I've wrapped all of the Litle objects interfaces under conviniant to use functions. Please use those as it will be much easier to programatically make a call to Litle in those circumstances without having to gin up a form submit.

e) Primary pieces left to do:
   Mostly the brass tacks of going from a form submit + config, to object call. You've got point A and point B, just need to map the form submit values into an object to place the call, and handle the result.
   Administrative controls under admin.inc for handling refunds etc. As above, point A and point B are set.
   Final polishes - Coder (minor) standards, and any wierd form layouts.