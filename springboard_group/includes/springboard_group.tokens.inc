<?php

/**
 * Implements hook_token_info().
 */
function springboard_group_token_info() {
  $info['types']['springboard_group'] = array(
    'name' => t('Springboard Group'),
    'description' => ('Springboard Group tokens.'),
    'needs-data' => 'node',
  );
  $info['tokens']['springboard_group']['group_names'] = array(
    'name' => t('Group Names'),
    'description' => t('The names of the groups which own this content. (Ex: A, B and C.'),
    'dynamic' => FALSE,
  );
  $info['tokens']['springboard_group']['group_names_with_the'] = array(
    'name' => t('Group Names with "the".'),
    'description' => t('The names of the groups which own this content, with the article "the". (Ex: the A, the B and the C.)'),
    'dynamic' => FALSE,
  );
  $info['tokens']['springboard_group']['group_names_with_the_ucfirst'] = array(
    'name' => t('Group Names with "the", first instance uppercase.'),
    'description' => t('The names of the groups which own this content, with the article "the", first instance uppercase. (Ex: The A, the B and the C.)'),
    'dynamic' => FALSE,
  );
  return $info;
}

/**
 * Implements hook_tokens().
 */
function springboard_group_tokens($type, $tokens, array $data = array(), array $options = array()) {

  $replacements = [];
  if ($type == 'springboard_group' && !empty($data['node'])) {
    $node = $data['node'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        // Simple key values on the node.
        case 'group_names':
          $groups = og_get_entity_groups('node', $node);
          $titles = [];
          if (!empty($groups['node'])) {
            foreach ($groups['node'] as $nid) {
              $group = node_load($nid);
              $titles[] = check_plain($group->title);
            }
          }
          $titles = implode(', ', $titles);
          $pos = strrpos($titles, ',');
          if ($pos !== FALSE) {
            $titles = substr_replace($titles, ' and', $pos, strlen(','));
          }
          $replacements[$original] = $titles;
          break;

        case 'group_names_with_the':
          $groups = og_get_entity_groups('node', $node);
          $titles = [];
          if (!empty($groups['node'])) {
            foreach ($groups['node'] as $nid) {
              $group = node_load($nid);
              $pos = strpos($group->title, 'The');
              if ($pos !== FALSE && $pos == 0) {
                $group->title = substr_replace($group->title, '', 0, 3);
              }
              $titles[] = 'the ' . check_plain($group->title);
            }
          }
          $titles = implode(', ', $titles);
          $pos = strrpos($titles, ',');
          if ($pos !== FALSE) {
            $titles = substr_replace($titles, ' and', $pos, strlen(','));
          }
          $replacements[$original] = $titles;
          break;

        case 'group_names_with_the_ucfirst':
          $groups = og_get_entity_groups('node', $node);
          $titles = [];
          if (!empty($groups['node'])) {
            foreach ($groups['node'] as $nid) {
              $group = node_load($nid);
              $pos = strpos($group->title, 'The');
              if ($pos !== FALSE && $pos == 0) {
                $group->title = substr_replace($group->title, '', 0, 3);
              }
              $titles[] = 'the ' . check_plain($group->title);
            }
          }
          $titles = implode(', ', $titles);
          $pos = strrpos($titles, ',');
          if ($pos !== FALSE) {
            $titles = substr_replace($titles, ' and', $pos, strlen(','));
          }
          $replacements[$original] = ucfirst($titles);
          break;

        // Any variables present on the Group nodes can be replaced.
        default:
          $groups = og_get_entity_groups('node', $node);
          $content = "";
          if (!empty($groups['node'])) {
            // This will not work if there are multiple $groups because there is no accommodation for listing multiple
            // multiple values and the current scope is for nodes with a single group membership. If specific tokens
            // are needed for multi-value lists, specific processing will need to be added for those tokens (for
            // example, the group_names token above.
            foreach ($groups['node'] as $nid) {
              $group = node_load($nid);
              // If the element is a field, render it.
              if (is_array($group->$name)) {
                $field_render = field_view_field('node', $group, $name, 'teaser');
                $content .= is_array($field_render) ? drupal_render($field_render) : '';
              // If the element is a string, output as is.
              } else if (is_string($group->$name)) {
                $content .= $group->$name;
              }
            }
          }
          $replacements[$original] = $content;
          break;
      }
    }
  }
  return $replacements;
}