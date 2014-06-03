<?php
/* Start the count. */
$zebra = 0;
$num_items = count($types);
$i = 0;
?>

<?php foreach ($types as $type) : ?>

  <?php
  // Define odd / even for use in the definition list.
  $zebra_class = ($zebra % 2) ? 'even' : 'odd';
  $i++;

  // Define first / last.
  if ($i == 1) {
    $attributes = 'first';
  }

  if ($i == $num_items) {
  $attributes = 'last';
  }

  ?>
  <div class="types-wrapper <?php print $zebra_class; ?> <?php print $attributes ; ?>">
    <h2 class="types"><?php print $type->name; ?></h2>

    <div class="buttons-wrapper">

      <!-- bootstrap drop list widget -->
      <div class="btn-group">
        <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
          <?php print t('Options'); ?>
          <span class="caret"></span>
        </button>

        <?php // @todo make the menu below dynamic or contextual? ?>
        <ul class="dropdown-menu views-widget-dropdown">
          <li><a href="/admin/reports/salesforce/donations"><?php print t('Search Donations'); ?></a></li>
          <li><a href="/springboard/asset-library"><?php print t('Templates & Wrappers'); ?></a></li>
          <li><a href="#"><?php print t('Donation Settings'); ?></a></li>
        </ul>
      </div><!--// btn-group -->

      <a href="/springboard/add/<?php print preg_replace('/_/', '-', $type->type); ?>" class="button add-button"><?php print t('Create'); ?> <?php print $type->name; ?></a>

    </div><!-- // buttons-wrapper -->

    <?php print($tables[$type->type]); ?>
    <?php $zebra++; ?>
  </div>
<?php endforeach; ?>
