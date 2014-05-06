<h2>Donation Forms</h2>
<?php foreach ($types as $type) : ?>
   <h3><?php print $type->name; ?>s</h3>
   <?php print($tables[$type->type]); ?>
<?php endforeach; ?>