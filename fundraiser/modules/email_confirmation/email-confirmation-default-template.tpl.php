<table style="font-family: verdana,arial,helvetica; font-size: small;" align="center" bgcolor="#006699" border="0" cellpadding="1" cellspacing="0" width="95%">
   <tbody><tr>
     <td>

       <table style="font-family: verdana,arial,helvetica; font-size: small;" align="center" bgcolor="#FFFFFF" border="0" cellpadding="5" cellspacing="0" width="100%">
                 <tbody><tr valign="top">
           <td>
             <table style="font-family: verdana,arial,helvetica; font-size: small;" width="100%">
               <tbody><tr>
                 <td>
                   <img>    
              </td>
                 <td width="98%">

                   <div style="padding-left: 1em;">
                   <span style="font-size: large;"><?php print $site_name?></span><br>
                                     </div>
                 </td>
                 <td nowrap="nowrap">
                     <br>                </td>
               </tr>
             </tbody></table>

           </td>
         </tr>

         <tr valign="top">
           <td>
                                       <p><b>Thanks for your order,  <?php print $first_name?>!</b></p>


               <p><b>Want to manage your order online?</b><br>

               If you need to check the status of your order, please visit our  
home page at <a hre<?php print $site_url?>" target="_blank"><?php print $site_name?></a> and click on "My  
account" in the menu or login with the following link:              <br><br><a href="<?php print $site_url?>/user" target="_blank"><?php print $site_url?></a></p>

             <table style="font-family: verdana,arial,helvetica; font-size: small;" border="0" cellpadding="4" cellspacing="0" width="100%">
               <tbody><tr>
                 <td colspan="2" style="color: white;" bgcolor="#006699">
                   <b>Purchasing Information:</b>

                 </td>
               </tr>
               <tr>
                 <td nowrap="nowrap">
                   <b>E-mail Address:</b>
                 </td>
                 <td width="98%">
                   <a href="mailto:<?php print $order->primary_email ?>" target="_blank"><?php print $order->primary_email ?></a>                </td>

               </tr>
               <tr>
                 <td colspan="2">

                   <table style="font-family: verdana,arial,helvetica; font-size: small;" cellpadding="0" cellspacing="0" width="100%">
                     <tbody><tr>
                       <td valign="top" width="50%">
                         <b>Billing Address:</b><br>
                         <?php print $order->billing_first_name . ' ' . $order->billing_last_name?><br>
                         <?php print $order->billing_street1?><br>
                         <?php 
                         if ($order->billing_street2) {
                           print $order->billing_street2 . '<br>';  
                         }
                         ?>
                         <?php print $order->billing_city .  ', ' . $order->billing_zone . ' ' . $order->billing_postal_code?><br>

                         <br>
                         <br>
                       </td>
                                           </tr>
                   </tbody></table>

                 </td>
               </tr>

               <tr>
                 <td nowrap="nowrap">
                   <b>Order Grand Total:</b>
                 </td>
                 <td width="98%">
                   <b><?php print money_format('%.2n', $order->order_total)?></b>
                 </td>
               </tr>

               <tr>
                 <td nowrap="nowrap">
                   <b>Payment Method:</b>
                 </td>
                 <td width="98%">
                   <?php print $order->payment_method?>                </td>
               </tr>

               <tr>
                 <td colspan="2" style="color: white;" bgcolor="#006699">
                   <b>Order Summary:</b>
                 </td>
               </tr>


               <tr>
                 <td colspan="2">

                   <table style="font-family: verdana,arial,helvetica; font-size: small;" border="0" cellpadding="1" cellspacing="0" width="100%">
                     <tbody><tr>
                       <td nowrap="nowrap">
                         <b>Order #:</b>
                       </td>
                       <td width="98%">
                         <a href="<?php print $site_url ."/user/" . $order->uid . "/order/" . $order->order_id; ?>" target="_blank"><?php print $order->order_id?></a>     
                   </td>

                     </tr>

                     <tr>
                       <td nowrap="nowrap">
                         <b>Order Date: </b>
                       </td>
                       <td width="98%">
                         <?php print date('r', time());?>                      </td>

                     </tr>


                     <tr>
                       <td nowrap="nowrap">
                         Products Subtotal:&nbsp;
                       </td>
                       <td width="98%">
                         <?php print money_format('%.2n', $order->order_total)?>                      </td>

                     </tr>


                     <tr>
                       <td>&nbsp;</td>
                       <td>------</td>
                     </tr>

                     <tr>
                       <td nowrap="nowrap">

                         <b>Total for this Order:&nbsp;</b>
                       </td>
                       <td>
                         <b><?php print money_format('%.2n', $order->order_total)?></b>
                       </td>
                     </tr>

                     <tr>

                       <td colspan="2">
                         <br><br><b>Products on order:&nbsp;</b>

                         <table style="font-family: verdana,arial,helvetica; font-size: small;" width="100%">

                                                     <tbody><tr>
                             <td nowrap="nowrap" valign="top">
                             </td>

                             <td width="98%">
                               <b><?php print $order->form_title?> - <?php print money_format('%.2n', $order->order_total)?></b>
                                                             <br>
                                                             <br>
                             </td>
                           </tr>
                                                   </tbody></table>

                       </td>
                     </tr>
                   </tbody></table>

                 </td>
               </tr>

                             <tr>
                 <td colspan="2">

                   <hr noshade="noshade" size="1"><br>

                                     <p><b>Where can I get help with reviewing  
my order?</b><br>
                   To learn more about managing your orders on <a href="<?php print $site_url?>" target="_blank"><?php print $site_name?></a>, please visit our <a href="<?php print $site_url?>" target="_blank">help page</a>.                  <br></p>

                                     <p>Please note: This e-mail message is an  
automated notification. Please do not reply to this message.</p>

                   <p>Thanks again for shopping with us.</p>

                                     <p><b><a href="<?php print $site_url?>" target="_blank"><?php print $site_name?></a></b><br><b></b></p>
                                   </td>
               </tr>

             </tbody></table>
           </td>

         </tr>
       </tbody></table>
     </td>
   </tr>
</tbody></table>
