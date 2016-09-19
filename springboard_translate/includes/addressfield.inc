<?php

/**
 * Implements INCLUDE_sb_translate_plugin_info().
 */
function addressfield_sb_translate_include_info() {
  return array(
    'description' => t('Addressfield-based state/province options.'),
  );
}

/**
 * Implements INCLUDE_sb_translate_manager_form_data_alter().
 */
function addressfield_sb_translate_manager_form_data_alter(&$form, &$form_data, $view, $node) {
  if (!isset($view->result)) {
    return;
  }
  foreach ($view->result as $result) {
    list($textgroup, $nid, $cid, $sub_field) = explode(':', $result->locales_source_location);
    if (isset($node->webform['components'][$cid]['form_key']) && $node->webform['components'][$cid]['form_key'] == 'state') {
      foreach ($node->webform['components'] as $field_data) {
        if ($field_data['form_key'] == 'country' && isset($field_data['value'])) {
          $form['translation_manager']['#default_country_code'] = $field_data['value'];
          return;
        }
      }
    }    
  }
}


/**
 * Implements INCLUDE_sb_translate_manager_row_label_alter().
 */
function addressfield_sb_translate_manager_row_label_alter(&$form, $id, &$row_label) {
  // Add country filtering for the "State" field:
  if (function_exists('fundraiser_commerce_get_countries') && isset($form['#translation_field_data'][$id]['form_key'])
    && $form['#translation_field_data'][$id]['form_key'] == 'state') {
    $country_options = _sb_translate_get_enabled_countries();
    $row_label .= '&nbsp;<select class="trans-manager-state-selector">' .
      '<option value="all-countries">- View All States -</option>';
    foreach ($country_options as $key => $value) {
      $selected = (isset($form['#default_country_code']) && $key == $form['#default_country_code']) ? TRUE : FALSE;
      $row_label .= '<option ' . ($selected ? 'selected="selected" ' : '') . 'value="' . $key . '">' . $value . '</option>';
    }
    $row_label .= '</select>';
  }
}

/**
 * Implements INCLUDE_sb_translate_manager_row_pre_render().
 */
function addressfield_sb_translate_manager_row_pre_render(&$form, $id, $sub_field, &$item) {
  $enabled_countries = _sb_translate_get_enabled_countries();
  if (empty($enabled_countries)) {
    return;
  }
  if (isset($form['#translation_field_data'][$id]['form_key'])
    && $form['#translation_field_data'][$id]['form_key'] == 'state') {
    foreach ($enabled_countries as $enabled_country_code => $enabled_country) {
      if (strpos($sub_field, 'option;' . $enabled_country_code . ';') !== FALSE) {
        list($label, $country_code, $state_code) = explode(';', $sub_field);
        $item['#attributes']['country_code'] = $country_code;
        return; 
      }
    }
  }
}
