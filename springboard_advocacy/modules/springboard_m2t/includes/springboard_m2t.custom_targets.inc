<?php


function springboard_m2t_custom_targets() {
 $header = array('Name', 'Title', 'Organization', 'Email', 'Phone', 'Address', 'Actions');
  $rows = array();

  $loader = new AdvocacyApiClientLoader();
  $client = $loader->getClient();
  $targets = $client->getCustomTargets();

  foreach ($targets as $target) {
    $links = array();
    $links[] = l('edit', 'target/' . $target->id . '/edit');
    $links[] = l('view', 'view/' . $target->id);
    $links = implode(' | ', $links);
    $rows[] = array(
      'data' => array(
        array('data' => $target->first_name . ' ' . $target->last_name),
        array('data' => $target->salutation),
        array('data' => ''),
        array('data' => $target->email),
        array('data' => ''),
        array('data' => ''),
        array('data' => $links),
      ),
    );
  }

  $rows = springboard_m2t_pager_default_initialize($rows, 1);

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  $output .= theme('pager', array('quantity' => 1));
  return $output;
}

//modified version of pager.inc function pager_default_initialize()
function springboard_m2t_pager_default_initialize($rows, $limit = 9, $element = 0) {
 global $pager_page_array, $pager_total, $pager_total_items, $pager_limits;

  $page = pager_find_page($element);

  // We calculate the total of pages as ceil(items / limit).
  $pager_total_items[$element] = count($rows);
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min($page, ((int) $pager_total[$element]) - 1));
  $pager_limits[$element] = $limit;

  return array_slice($rows, $pager_page_array[$element] * $limit, $limit, TRUE);
}