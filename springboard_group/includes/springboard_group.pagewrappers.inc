<?php
/*

function springboard_group_available_page_wrappers_alter($wrppers) {

  // Optionally filter available wrappers by organic groups the user is enrolled in.
  if (module_exists('og') && variable_get('page_wrappers_filter_by_og', FALSE)) {
    $wrapper_nids = array_keys($wrappers);
    $user_groups = og_get_entity_groups();
    $result = db_select('og_membership', 'o')
      ->fields('o', array('gid', 'etid'))
      ->condition('entity_type', 'node')
      ->condition('etid', $wrapper_nids, 'IN')
      ->execute();
    $results = $result->fetchAll();
    $wrappers_og = array(t('-- Select --'));
    foreach ($results as $data) {

      // to avoid dupes we make sure the wrapper hasn't already been added to wrappers_og
      if (in_array($data->gid, $user_groups['node']) && !in_array($data->etid, array_keys($wrappers_og))) {
        $wrappers_og[$data->etid] = $wrappers[$data->etid];
      }
    }
    $wrappers = $wrappers_og;
  }

}

function page_wrappers_og_ajax_callback($form, $form_state) {
  return $form['springboard_display']['page_wrappers_settings']['page_wrappers']['wrapper_nid'];
}
*/
/**
 * List page wrappers by group.
 *
 * @param $nid
 * Group id.
 *
 * @return array associative array of page wrapper titles keyed by node id
 * or FALSE if none are found.
 */
//function _page_wrappers_list_wrappers_by_group($nid) {
//   $wrappers = array();
//   // SELECT n.nid, n.title
//   // FROM node n
//   // INNER JOIN og_membership o ON o.etid = n.nid
//   // WHERE n.type = 'page_wrapper' AND o.gid = 3
//   // GROUP BY n.nid;
//
//  $query = db_select('node', 'n');
//  $query->addField('n', 'nid', 'nid');
//  $query->addField('n', 'title', 'title');
//  $query->join('og_membership', 'o', 'o.etid=n.nid');
//  $query->condition('n.type', 'page_wrapper');
//  $query->condition('o.gid', $nid);
//  $query->groupBy('n.nid');
//  $results = $query->execute();
//  while ($record = $results->fetchAssoc()) {
//    $wrappers[$record['nid']] = $record['title'];
//  }
//  return count($wrappers) ? $wrappers : FALSE;
//}
