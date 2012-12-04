<?php
/**
 * @file
 */

/**
 * Implementation of hook_fieldgroup_default_groups().
 */
function page_wrappers_fieldgroup_default_groups() {
  $groups = array();

  // Exported group: group_files
  $groups['page_wrapper-group_files'] = array(
    'group_type' => 'standard',
    'type_name' => 'page_wrapper',
    'group_name' => 'group_files',
    'label' => 'Files',
    'settings' => array(
      'form' => array(
        'style' => 'fieldset',
        'description' => '',
      ),
      'display' => array(
        'description' => '',
        'label' => 'above',
        'teaser' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'full' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        '4' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
        'token' => array(
          'format' => 'fieldset',
          'exclude' => 0,
        ),
      ),
    ),
    'weight' => '-3',
    'fields' => array(
      '0' => 'field_css_file',
      '1' => 'field_js_file',
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Files');

  return $groups;
}
