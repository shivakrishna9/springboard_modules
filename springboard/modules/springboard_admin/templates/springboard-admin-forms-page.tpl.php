<?php foreach ($types as $type) : ?>
<h2><?php print $type->name; ?>s</h2>
   <a href="/node/add/<?php print preg_replace('/_/', '-', $type->type); ?>" class="button button-create button-left">Create <?php print $type->name; ?></a>
   <?php print($tables[$type->type]); ?>
   <a class="button more-button" href="/admin/springboard/<?php print ($springboard_type == 'fundraiser') ? 'donation-forms' : 'forms'; ?>/<?php print $type->type; ?>">
     <?php print t('View all');?> <?php print $type->name; ?><?php print t('s');?> </a>
<?php endforeach; ?>
