<?php

class springboard_tags_export_ui extends ctools_export_ui {

  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    $form['tag'] = array(
      '#type' => 'textarea',
      '#title' => t('Tag'),
      '#default_value' => $form_state['item']->tag,
    );

    $form['placement'] = array(
      '#type' => 'select',
      '#title' => t('Placement'),
      '#default_value' => $form_state['item']->settings['placement'],
      '#options' => _springboard_tags_placement_options(),
    );

    // Visibility options.
    $form['visibility'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );

    // Fundraiser visibility options.
    $form['visibility']['fundraiser'] = array(
      '#type' => 'fieldset',
      '#title' => t('Fundraiser Visibility'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#access' => module_exists('fundraiser'),
    );

    $form['visibility']['fundraiser']['confirmation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show only on donation form confirmation pages'),
      '#default_value' => $form_state['item']->settings['visibility']['fundraiser']['confirmation'],
    );

    // Fundraiser visibility options.
    $form['visibility']['user'] = array(
      '#type' => 'fieldset',
      '#title' => t('User Visibility'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#access' => module_exists('fundraiser'),
    );

    $role_options = array_map('check_plain', user_roles());
    $form['visibility']['user']['roles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Show tag for specific user roles'),
      '#default_value' => $form_state['item']->settings['visibility']['user']['roles'],
      '#options' => $role_options,
      '#description' => t('Show this tag only for the selected role(s). If you select no roles, the tag will be visible to all users.'),
    );

    // Per-path visibility.
    $form['visibility']['path'] = array(
      '#type' => 'fieldset',
      '#title' => t('Path Visibility'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['visibility']['path']['page_specific'] = array(
      '#type' => 'radios',
      '#title' => t('Include tag on specific pages'),
      '#options' => array(
        BLOCK_VISIBILITY_NOTLISTED => t('All pages except those listed'),
        BLOCK_VISIBILITY_LISTED => t('Only the listed pages'),
      ),
      '#default_value' => $form_state['item']->settings['visibility']['path']['page_specific'],
    );

    $form['visibility']['path']['pages'] = array(
      '#type' => 'textarea',
      '#title' => 'Pages',
      '#default_value' => $form_state['item']->settings['visibility']['path']['pages'],
      '#description' => t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard."),
    );

    // Per-node type visibility.
    $form['visibility']['node'] = array(
      '#type' => 'fieldset',
      '#title' => t('Content Visibility'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['visibility']['node']['type'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Show tag for specific content types'),
      '#default_value' => $form_state['item']->settings['visibility']['node']['type'],
      '#options' => node_type_get_names(),
      '#description' => t('Show this tag only on nodes of the given type(s). If you select no types, there will be no type-specific limitation.'),
    );
  }

  function edit_form_submit(&$form, &$form_state) {
    parent::edit_form_submit($form, $form_state);

    // Since items in our settings are not in the schema, we have to do these manually:
    $form_state['item']->settings['placement'] = $form_state['values']['placement'];
    $form_state['item']->settings['visibility'] = $form_state['values']['visibility'];
  }

  function list_form(&$form, &$form_state) {
    parent::list_form($form, $form_state);
    $form['top row'] = $form['top row'] + array(
      '#access' => FALSE,
    );
  }

  /**
   * Determine if a row should be filtered out.
   *
   * @return
   *   TRUE if the item should be excluded.
   */
  function list_filter($form_state, $item) {
    // Disable the filtering.
    return FALSE;
  }

  function list_sort_options() {
    return array(
      'disabled' => t('Enabled, title'),
      'title' => t('Title'),
      'name' => t('Name'),
      'storage' => t('Storage'),
    );
  }

  function list_build_row($item, &$form_state, $operations) {
    // Set up sorting
    switch ($form_state['values']['order']) {
      case 'disabled':
        $this->sorts[$item->name] = empty($item->disabled) . $item->admin_title;
        break;
      case 'title':
        $this->sorts[$item->name] = $item->admin_title;
        break;
      case 'name':
        $this->sorts[$item->name] = $item->name;
        break;
      case 'storage':
        $this->sorts[$item->name] = $item->type . $item->admin_title;
        break;
    }

    $ops = theme('links__ctools_dropbutton', array('links' => $operations, 'attributes' => array('class' => array('links', 'inline'))));

    $this->rows[$item->name] = array(
      'data' => array(
        array('data' => check_plain($item->admin_title), 'class' => array('ctools-export-ui-title')),
        array('data' => check_plain($item->name), 'class' => array('ctools-export-ui-name')),
        array('data' => $ops, 'class' => array('ctools-export-ui-operations')),
      ),
      'title' => check_plain($item->admin_description),
      'class' => array(!empty($item->disabled) ? 'ctools-export-ui-disabled' : 'ctools-export-ui-enabled'),
    );
  }

  function list_table_header() {
    return array(
      array('data' => t('Title'), 'class' => array('ctools-export-ui-title')),
      array('data' => t('Name'), 'class' => array('ctools-export-ui-name')),
      array('data' => t('Operations'), 'class' => array('ctools-export-ui-operations')),
    );
  }

}

