<?php

/**
 * @file
 * Contains a Views field handler to take care of displaying edit links
 * as fields
 */


class sba_message_handler_edit_link_field extends sba_message_handler_link_field {
  function construct() {
    parent::construct();
    $this->additional_fields['type'] = 'type';
  }


  function render($values) {
    $type = $values->{$this->aliases['type']};
    
    //Creating a dummy sba_message to check access against
    $dummy_sba_message = (object) array('type' => $type);
    if (!sba_message_access('edit', $dummy_sba_message)) {
      return;
    }
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
    $sba_message_id = $values->{$this->aliases['sba_message_id']};
    if (is_numeric(arg(1)) && arg(2) == 'messages') {
      $nid = arg(1);
    }
    else {
      $nid = '';
    }
    return l($text, 'admin/springboard/sba_messages/sba_message/' . $sba_message_id . '/edit', array('query' => array('field_sba_alert_id' => $nid)));
  }
}
