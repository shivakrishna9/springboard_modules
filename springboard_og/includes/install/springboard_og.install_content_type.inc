<?php

/**
 * Install the springboard_group content type.
 */
function _springboard_og_install_content_type() {

  module_load_include('inc', 'springboard_og', 'includes/install/springboard_og.install_fields');

  $type = node_type_set_defaults();
  $type->name = t('Springboard Group');
  $type->type = 'springboard_group';
  $type->description = t('Allows administrators to create affiliate groups.');
  $type->title_label = t('Title');
  $type->has_title = TRUE;
  $type->base = 'node_content';
  $type->custom = TRUE;
  $type->modified = TRUE;
  $type->module = 'springboard_og';
  $type->locked = TRUE;

  node_type_save($type);
  node_types_rebuild();
  springboard_og_install_fields();

  drupal_set_message(t('The content type Springboard Group has been added.'));
  watchdog('springboard_og', 'Added content type Springboard Group.');

}