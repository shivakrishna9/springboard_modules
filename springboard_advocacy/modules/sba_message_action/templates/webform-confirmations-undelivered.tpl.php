<div id="sba-undeliverable-messages">
  <h3><?php print $undeliverable_title ?></h3>
  <div id = "undeliverable-messages-container">

    <p><?php print $message_count ?></p>

    <?php foreach($messages as $message): ?>
      <div class="sba-undeliverable-messages-message">
        <div class="sba-undeliverable-messages-message-recipient">
          <?php print $message['target'] ?>
        </div>
        <div class="sba-undeliverable-messages-message-subject">
          <?php print $message['subject'] ?><?php print $message['show_link'] ?>
        </div>
        <div class="sba-message-body" style="display:none">'
          <?php print $message['message'] ?>
        </div>
      </div>
      <br /><br />
    <?php endforeach; ?>

  </div>
</div>
<p>&nbsp;</p>

<script language="javascript">
  jQuery('.sba-message-show-message').on('click', function () {
    var message = jQuery(this).parents('.sba-undeliverable-messages-message-subject').siblings('.sba-message-body');
    message.toggle('slow', function () {
      var link = jQuery(this).parents('.sba-undeliverable-messages-message-subject').siblings('.sba-message-show-message');
      if (message.is(':visible')) {
        link.text('Hide message')
      }
      if (!message.is(':visible')) {
        link.text('Show message')
      }
    });
    return false;
  })
</script>

