<?php
/**
 * @file
 * Field definitions for Springboard Group.
 */

/**
 * Implements hook_field_info().
 *
 * Field definition for the group social media fields.
 */
function springboard_group_field_info() {
  return array(
    // We name our field as the associative name of the array.
    'field_springboard_group_social' => array(
      'label' => t('Social Media Links'),
      'description' => t('Creates a field for social media links and icon'),
      'default_widget' => 'field_springboard_group_social_widget',
      'default_formatter' => 'field_springboard_group_social_formatter',
      'settings' => array(
        'uri_scheme' => variable_get('file_default_scheme', 'public'),
        'default_image' => 0,
      ),
      'instance_settings' => array(
        'file_extensions' => 'png gif jpg jpeg',
        'file_directory' => '',
        'max_filesize' => '',
        'alt_field' => 0,
        'title_field' => 0,
        'max_resolution' => '',
        'min_resolution' => '',
        'default_image' => 0,
      ),
    ),
  );
}

/**
 * @return array
 */
function springboard_group_field_formatter_info() {
  return array(
    'field_springboard_group_social_formatter' => array(
      'label' => t('xxx'),
      'field types' => array('field_springboard_group_social'),
    ),
  );
}


/**
 * Implements hook_field_widget_info().
 */
function springboard_group_field_widget_info() {
  return array(
    'field_springboard_group_social_widget' => array(
      'label' => t('Social media links fields'),
      'field types' => array('field_springboard_group_social'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'preview_image_style' => 'thumbnail',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function springboard_group_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $elements['#delta'] = $delta;
  switch ($instance['widget']['type']) {
    case 'field_springboard_group_social_widget':
      //$field['settings']['display_field'] = 0;
      $elements += array(
        '#type' => 'fieldset',
        '#title' => '',
        //'#element_validate' => array('springboard_group_social_widget_validate'),
        '#delta' => $delta,
        '#collapsible' => FALSE,
      );

      $elements += image_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      foreach (element_children($elements) as $delta) {
        $value = isset($items[$delta]['url']) ? $items[$delta]['url'] : '';
        $elements[$delta]['url'] = array(
          '#type' => 'textfield',
          '#title' => t('URL'),
          '#size' => 60,
          '#maxlength' => 255,
          '#default_value' => $value,
          '#description' => '',
        );

        if ($instance['required'] == 1) {
          $elements['url']['#required'] = 1;
        }
      }
      break;
  }

  $elements['#file_upload_title'] = t('Add new social media link');
  $element = $elements;
  return $element;
}


/**
 * @param $item
 * @param $field
 * @return bool
 */
function springboard_group_field_is_empty($item, $field) {
  return empty($item['url']);
}

/**
 * Implements hook_field_instance_settings_form().
 */
function springboard_group_field_instance_settings_form($field, $instance) {
  if ($field['type'] == 'field_springboard_group_social') {
    return file_field_instance_settings_form($field, $instance);
  }
}

/**
 * Implements hook_field_widget_settings_form().
 */
function springboard_group_field_widget_settings_form($field, $instance) {
  $form = image_field_widget_settings_form($field, $instance);
  return $form;
}
