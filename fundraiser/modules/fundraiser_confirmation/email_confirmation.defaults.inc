<?php

/**
 * Returns default starter text message.
 */
function email_confirmation_default_text_message() {
  $message = 'On behalf of [site-name], thank you for your generous contribution.';
  
  return $message;
}

/**
 * Returns default starter text template.
 */
function email_confirmation_default_text_template() {
  $template = '
[confirmation-message-text]

Donation information:
---------------------
Email Address:
[order-email]

Billing Address:
[order-billing-address]

Donation Total:
[order-total]

Payment Method:
[order-payment-method]


Donation Summary:
----------------------
Donation #: [order-link]

Total Donation: [order-total]

--------------------------------------------------------------
Please note: this e-mail message is an automated notification. Please do not reply to this message.

Your organization name here
organization address line 1
organization address line 2
city, state/province, postal code

Tel: telephone number
Fax: fax number
Email: organization email
';

  return $template;
}

/**
 * Returns default starter html email message. 
 */
function email_confirmation_default_html_message() {
  $message = '
  <p><span>&nbsp;</span></p>
    <p><span>On behalf
    of [site-name], thank you for your generous contribution.</span></p>
  ';
  return $message;
}

/**
 * Returns default starter html email template.
 */
function email_confirmation_default_html_template() {

  $template = '
<html>
<head></head>
<body>
<tbody><tr>
    <td style="padding: 3.75pt;" valign="top">
    <table style="width: 100%;" border="0" cellpadding="0" width="100%">
     <tbody><tr>
      <td style="padding: 0.75pt;">
      <p class="MsoNormal" style="text-align: center;" align="center"><span>[site-logo]<span style="padding-left:10px">[site-name]</span></span></p>
      </td>
      <td style="padding: 0.75pt;" nowrap="nowrap"></td>
     </tr>

    </tbody></table>
    </td>
   </tr>
   <tr>
    <td style="padding: 3.75pt;" valign="top">
    [confirmation-message-html]
    <table style="width: 100%;" border="0" cellpadding="0" cellspacing="0" width="100%">

     <tbody><tr>
      <td colspan="2" style="background: none repeat scroll 0% 0% rgb(114, 46, 33); padding: 3pt;">
      <p class="MsoNormal"><b><span style="color: white;">Donation Information:</span></b><span style="color: white;"> </span></p>
      </td>
     </tr>
     <tr>
      <td style="padding: 3pt;" nowrap="nowrap">
      <p class="MsoNormal"><b><span>E-mail
      Address:</span></b><span> </span></p>

      </td>
      <td style="width: 98%; padding: 3pt;" width="98%">
      <p class="MsoNormal"><span><a href="mailto:[order-email]" target="_blank">[order-email]</a>
      </span></p>
      </td>
     </tr>
     <tr>
      <td colspan="2" style="padding: 3pt;">

      <table style="width: 100%;" border="0" cellpadding="0" cellspacing="0" width="100%">
       <tbody><tr>
        <td style="width: 50%; padding: 0in;" valign="top" width="50%">
        <p class="MsoNormal"><b><span>Billing
        Address:</span></b><span><br>
        [order-address]
        </span></p>

        </td>
       </tr>
      </tbody></table>
      </td>
     </tr>
     <tr>
      <td style="padding: 3pt;" nowrap="nowrap">
      <p class="MsoNormal"><b><span>Donation
      Total:</span></b><span> </span></p>

      </td>
      <td style="width: 98%; padding: 3pt;" width="98%">
      <p class="MsoNormal"><b><span>[order-total]</span></b><span> </span></p>
      </td>
     </tr>
     <tr>
      <td style="padding: 3pt;" nowrap="nowrap">
      <p class="MsoNormal"><b><span>Payment
      Method:</span></b><span> </span></p>

      </td>
      <td style="width: 98%; padding: 3pt;" width="98%">
      <p class="MsoNormal"><span>[order-payment-method]</span></p>
      </td>
     </tr>
     <tr>
      <td colspan="2" style="background: none repeat scroll 0% 0% rgb(114, 46, 33); padding: 3pt;">
      <p class="MsoNormal"><b><span style="color: white;">Donation Summary:</span></b><span style="color: white;"> </span></p>

      </td>
     </tr>
     <tr>
      <td colspan="2" style="padding: 3pt;">
      <table style="width: 100%;" border="0" cellpadding="0" cellspacing="0" width="100%">
       <tbody><tr>
        <td style="padding: 0.75pt;" nowrap="nowrap">
        <p class="MsoNormal"><b><span>Donation
        #:</span></b><span> </span></p>

        </td>
        <td style="width: 98%; padding: 0.75pt;" width="98%">
        <p class="MsoNormal"><span>[order-link]</span></p>
        </td>
       </tr>
       <tr>
        <td style="padding: 0.75pt;" nowrap="nowrap">
        <p class="MsoNormal"><span>Subtotal:&nbsp;

        </span></p>
        </td>
        <td style="width: 98%; padding: 0.75pt;" width="98%"></td>
       </tr>
       <tr>
        <td style="padding: 0.75pt;">
        <p class="MsoNormal"><span>&nbsp;</span></p>
        </td>
        <td style="padding: 0.75pt;">

        <p class="MsoNormal"><span>------</span></p>
        </td>
       </tr>
       <tr>
        <td style="padding: 0.75pt;" nowrap="nowrap">
        <p class="MsoNormal"><b><span>Total
        Donation:&nbsp;</span></b><span>
        </span></p>
        </td>

        <td style="padding: 0.75pt;">
        <p class="MsoNormal"><b><span>[order-total]</span></b><span> </span></p>
        </td>
       </tr>
      </tbody></table>
      </td>
     </tr>
     <tr>

      <td colspan="2" style="padding: 3pt;">
      <div class="MsoNormal" style="text-align: center;" align="center"><span>
      <hr style="color: rgb(160, 160, 160);" align="center" noshade="noshade" size="1" width="100%">
      </span></div>

      <p><b><span style="font-size: 13.5pt;">Please
      note: This e-mail message is an automated notification. Please do not
      reply to this message.</span></b></p>
      <p><b><span style="font-size: 13.5pt;">&nbsp;</span></b></p>
      <div>
      <div>
      <div>
      <div>
      <p style="text-align: center;" align="center"><b><span style="font-size: 13.5pt;">Your organization name here <br>
      Organization address line 1<br>

      Oranization address line 2<br>
      City, State, Postal Code<br>
      Tel: Organization telephone number<br>
      Fax: Organization fax number<br>
      E-mail: <a href="mailto:">Your organization email</a></span></b></p>

      </div>
      </div>
      </div>
      </div>
      <p><b><span style="font-size: 13.5pt;">&nbsp;</span></b></p>
      </td>
     </tr>
    </tbody></table>
    </td>
 </tr>
</tbody>
</body>
</html>
  ';

return $template; 

}
