<?php

class gateway_manager_gateway_ui extends ctools_export_ui {

  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);
    $item = $form_state['item'];

    $form['info']['name']['#title'] = t('Machine name');
    $form['info']['admin_title']['#required'] = TRUE;

    $gateways = _gateway_manager_list();
    $gateway_types = array(NULL => '- ' . t('select') . ' -');
    foreach ($gateways as $gateway_name => $gateway) {
      $gateway_types[$gateway_name] = $gateway['title'];
    }
    $form['gateway_type'] = array(
      '#type' => 'select',
      '#title' => t('Gateway type'),
      '#description' => t('Select the gateway type for this payment gateway. !locked',
        array('!locked' => !empty($item->gateway_type) ? t('Gateway type cannot be changed.') : '')),
      '#options' => $gateway_types,
      '#default_value' => $item->gateway_type,
      '#disabled' => !empty($item->gateway_type),
    );

    // If type has been selected, get the settings form for that type.
    if (!empty($item->gateway_type)) {
      $title = isset($gateways[$item->gateway_type]) ?
        $gateways[$item->gateway_type]['title'] : t('Unknown');

      $form['config'] = array(
        '#type' => 'fieldset',
        '#title' => t('@title settings', array('@title' => $title)),
        '#weight' => 4,
        '#collapsible' => TRUE,
      );

      // Call the settings function for this gateway.
      $settings_function = $gateways[$item->gateway_type]['settings'];
      if (!empty($settings_function) && function_exists($settings_function)) {
        $gateway_settings = $settings_function($item->config);
        if (is_array($gateway_settings)) {
          $form['config'] += $gateway_settings;
        }
      }
      else {
        $form['config']['#description'] = t('No additional settings required for this gateway type.');
      }
    }
  }

  function edit_form_submit(&$form, &$form_state) {
    global $user;
    $item = $form_state['item'];

    // Add the author uid, if missing.
    if (empty($item->uid)) {
      $form_state['values']['uid'] = $user->uid;
    }
    // Add created timestamp, if missing.
    if (empty($item->created)) {
      $form_state['values']['created'] = time();
    }
    // Update the updated timestamp.
    $form_state['values']['updated'] = time();

    // Serialize everything else.
    $config = array_diff_key($form_state['values'], (array) $item);
    $config = array_diff_key($config, array_flip(array('created', 'updated', 'op', 'submit', 'delete', 'form_build_id', 'form_token', 'form_id')));
    $form_state['values']['config'] = $config;

    parent::edit_form_submit($form, $form_state);
  }

  function list_table_header() {
    return array(
      array('data' => t('Admin title')),
      array('data' => t('Gateway type')),
      array('data' => t('Created by')),
      array('data' => t('Created')),
      array('data' => t('Updated')),
      array('data' => t('Storage')),
      array('data' => t('Operations')),
    );
  }

  function list_sort_options() {
    return array(
      'disabled' => t('Name'),
      'admin_title' => t('Admin title'),
      'storage' => t('Storage'),
    );
  }

  function list_build_row($item, &$form_state, $operations) {
    // Set up sorting.
    switch ($form_state['values']['order']) {
      case 'disabled':
        $this->sorts[$item->name] = $item->name;
        break;
      case 'admin_title':
        $this->sorts[$item->name] = $item->admin_title;
        break;
      case 'storage':
        $this->sorts[$item->name] = $item->type . $item->name;
        break;
    }

    // Load the author info.
    $author = isset($item->uid) ? user_load($item->uid) : new stdClass();
    $author->uid = $item->uid;

    // Load the gateways.
    $gateways = _gateway_manager_list();

    // Add a row.
    $this->rows[$item->name] = array(
      'data' => array(
        array('data' => check_plain($item->admin_title)),
        array('data' => check_plain(isset($gateways[$item->gateway_type]) ? $gateways[$item->gateway_type]['title'] : $item->gateway_type)),
        array('data' => check_plain($author->name)),
        array('data' => !empty($item->created) ? format_date($item->created) : ''),
        array('data' => !empty($item->updated) ? format_date($item->updated) : ''),
        array('data' => check_plain($item->type)),
        array('data' => theme('links', $operations)),
      ),
      'title' => check_plain($item->admin_title),
      'class' => !empty($item->disabled) ? 'ctools-export-ui-disabled' : 'ctools-export-ui-enabled',
    );
  }

  function list_search_fields() {
    return array(
      'name',
      'type',
    );
  }

}
