<?php

/**
 * @file stand alone PHP script used to serve goal widgets.
 */
$goal_ids = $_GET['goal_ids'];
$ids = explode(',', $goal_ids);
$data = array();
foreach ($ids as $id) {
  // TODO: add configurable code to explicitly declare file directory path.
  // this prevents having to move this file to the webform_goals file subdirectory.
  $filename = 'template-' . (int) $id . '.txt';
  if (file_exists($filename)) {
    $file = file_get_contents($filename);
    $data[$id] =  unserialize($file);
    $data['status'] = 'success';
  }
}
if (empty($data['status'])) {
  $data['status'] = 'fail';
  $data['data'] = 'null';
}
$data = json_encode($data);
echo $_GET['jsonp_callback'] . '(' . print_r($data, TRUE) . ');';

?>
