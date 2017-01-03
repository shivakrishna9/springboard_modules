<?php
/**
 * @file
 * Legislator lookup results form.
 *
 * Available variables:
 * - $phone_only -  whether to only show phone numbers.
 * - $legislators_processed: an array containing the legislator result:
 *
 *   $legislators_processed[$legislator_type] = array(
 *     'label' => The legislative chamber.
 *     'legislators[] => array(
 *        'name' => legislator name,
 *        'role' => title and district name.
 *        'main phone' => phone
 *        'main_fax' => fax
 *        'email' => email
 *        'youtube' => youtube
 *        'facebook' => facebook
 *        'twitter' => twitter
 *        'website' => website
 *        'addresses[] => array(
 *              'office_type' => Capitol, District, etc.
 *              'street' => building and street
 *              'phone' => phone
 *              'fax' => fax
 *         )
 *      )
 *   )
 *
 */
?>
<div id="sba-legislators">
  <!-- Legislator type loop -->
  <?php foreach ($legislators_processed as $role => $legislators): ?>
  <div class="sba-legislator-role">
    <h2><?php print $legislators['label']; ?></h2>
    <!-- Legislator details loop -->
    <?php foreach ($legislators['legislators'] as $legislator): ?>
    <div class="sba-legislator-details">
      <!-- Name -->
      <h3 class="sba-leg-name"><?php print $legislator['name']; ?></h3>
      <!-- Title and District name -->
      <div class="sba-legislator-role"><?php print $legislator['role']; ?></div>
      <!-- Phone  -->
      <div class="sba-legislator-main-phone">Phone: <?php print $legislator['main_phone']; ?></div>

      <?php if (empty($phone_only)): ?>
        <!-- Fax  -->
        <div class="sba-legislator-main-fax">Fax: <?php print $legislator['main_fax']; ?></div>
        <!-- Email -->
        <?php if (!empty($legislator['email'])): ?>
        <div class="sba-legislator-email"><a class="sba-legislator-email" href="mailto:<?php print $legislator['email']; ?> '" itemprop="email"><?php print $legislator['email'];?></a></div>
        <?php endif;?>
        <!-- Facebook -->
        <?php  if (!empty($legislator['facebook'])): ?>
        <div class="sba-legislator-email"><a class="sba-legislator-facebook" href="<?php print $legislator['facebook'] ?>">Facebook</a></div>
        <?php endif;?>
        <!-- Twitter -->
        <?php if (!empty($legislator['twitter'])): ?>
          <div class="sba-legislator-email"> <a class="sba-legislator-twitter" href="<?php print $legislator['twitter'] ?>">Twitter</a></div>
        <?php endif;?>
        <!-- Youtube -->
        <?php if (!empty($legislator['youtube'])): ?>
          <div class="sba-legislator-email"> <a class="sba-legislator-youtube" href="<?php print $legislator['youtube'] ?>">Youtube</a></div>
        <?php endif;?>
        <!-- Contact form or website -->
        <?php if (!empty($legislator['contact_form'])): ?>
          <div class="sba-legislator-email"> <a class="sba-legislator-contact_form" href="mailto:<?php print $legislator['contact_form'] ?>" itemprop="contact_form"><?php print $legislator['contact_form'] ?></a></div>
        <?php elseif(!empty($legislator['website'])): ?>
          <div class="sba-legislator-email"><a class="sba-legislator-website" href="<?php print $legislator['website'] ?>" itemprop="website">Website</a></div>
        <?php endif;?>

      <?php
        // If not phone only.
        endif;
      ?>

      <!-- Office addresses Loop -->
      <?php if (empty($phone_only)): ?>
        <div class="sba-leg-lookup-expand"><a class="leg-lookup-show" href="#">Show more</a></div>
        <div class="leg-lookup-addresses">
        <h4><?php print $legislator['office_label'];?></h4>
        <?php foreach ($legislator['addresses'] as $address): ?>
          <div class="leg-lookup-address">
            <div class="leg-lookup-address-<?php print $address['office_type'];?>">
              <strong><?php print $address['office_type'];?></strong><br/>
              <div class="leg-lookup-address-street"><?php print $address['street'];?></div>
              <div class="sba-legislator-phone">Phone: <?php print $address['phone'];?></div>
              <div class="sba-legislator-phone">Fax: <?php print $address['fax'];?></div>
            </div>
         </div>
        <?php endforeach; ?>
      </div>
      <?php
        // If not phone only.
      endif;
      ?>
      <!-- End addresses loop -->

    </div>
    <?php endforeach; ?>
    <!-- End legislator details loop -->

  </div>
  <?php endforeach; ?>
  <!-- End legislator type loop -->
</div>
