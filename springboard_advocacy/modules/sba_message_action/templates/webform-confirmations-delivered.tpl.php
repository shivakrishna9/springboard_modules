<div id="sba-delivered-messages">
  <h3><?php print $delivered_title ?></h3>
  <div id = "delivered-messages-container">
    <p><?php print $message_count ?></p>
    <br />
    <ul class = "message-recipient-list">
      <?php foreach($targets as $target): ?>
        <li>
          <?php print $target['person']; ?>
          <?php if(!empty($target['link'])): ?>
              (<?php print $target['link']; ?>)
          <?php endif;?>
        </li>
      <?php endforeach; ?>
    </ul>
  </div>
</div>

