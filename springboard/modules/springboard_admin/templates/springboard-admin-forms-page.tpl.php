<?php foreach ($types as $type) : ?>
<h2><?php print $type->name; ?>s</h2>
   <a href="/node/add/<?php print preg_replace('/_/', '-', $type->type); ?>" class="button">Create <?php print $type->name; ?></a>
   <?php print($tables[$type->type]); ?>
<?php endforeach; ?>
