<?php

function springboard_messages_messages($node) {
   $header = array('Internal Name', 'Subject', 'Recipient Group Title', 'Actions');
  $rows = array();

  if (!empty($node->messages)) {
    foreach ($node->messages as $message) {
      $links = array();
      $links[] = l('edit', 'node/' . $message->id . '/edit');
      $links = implode(' | ', $links);
      $rows[] = array(
        'data' => array(
          array('data' => $message->internal_name),
          array('data' => $message->subject),
          array('data' => $message->group),
          array('data' => $links),
        ),
      );
    }
  }
  
  $build['pager_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no messages found in the db'),
  );

  // Attach the pager theme.
  $build['pager_pager'] = array('#theme' => 'pager');

  return $build;
}
