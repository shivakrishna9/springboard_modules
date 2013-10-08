<?php
/**
 * @file
 * Admin pages for managing default form layouts.
 */

/**
 * Page callback for listing form layouts.
 */
function form_layout_list() {
  $output = array();

  $output['add'] = array(
    '#markup' => l('Add a form layout.', form_layout_menu_base_path() . '/add'),
  );

  $layouts = form_layout_load();

  foreach ($layouts as $id => $layout) {
    $output[$id]['name'] = array(
      '#markup' => '<h2>' . $layout['name'] . '</h2>',
    );

    $output[$id]['table'] = array(
      '#theme' => 'table',
      '#header' => array('Region', 'Content'),
      '#rows' => form_layout_table_rows($layout),
    );

    $output[$id]['links'] = array(
      '#theme' => 'links',
      '#links' => array(
        'edit' => array(
          'title' => 'Edit',
          'href' => form_layout_menu_base_path() . '/' . $id . '/edit',
        ),
        'delete' => array(
          'title' => 'Delete',
          'href' => form_layout_menu_base_path() . '/' . $id . '/delete',
        ),
      ),
    );
  }

  return $output;
}

/**
 * Page callback for adding form layouts
 */
function form_layout_add() {

  $form = form_layout_form();

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add',
  );

  return $form;
}

/**
 * Page callback for editing form layouts.
 */
function form_layout_edit($form, &$form_state, $layout) {

  $machine_name = arg(form_layout_menu_base_path_arg_position());
  $form = form_layout_form($machine_name, $layout);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Edit',
  );

  return $form;
}

/**
 * All purpose form for adding/editing form layouts.
 *
 * Fill in the params to get an edit form.  Otherwise you get an add form.
 * Does not include a submit button.
 *
 * @param string|null $machine_name
 * @param array|null $layout
 *
 * @return array  Form API
 */
function form_layout_form($machine_name = NULL, $layout = NULL) {

  $form = array();

  $form['title'] = array(
    '#title' => 'Layout name',
    '#type' => 'textfield',
    '#required' => TRUE,
    '#maxlength' => 255,
    '#default_value' => isset($layout['name']) ? $layout['name'] : '',
  );

  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#machine_name' => array(
      'exists' => 'form_layout_machine_name_exists',
    ),
    '#default_value' => isset($machine_name) ? $machine_name : '',
    '#disabled' => !empty($machine_name),
  );

  $elements = array('donation', 'donor_information', 'billing_information', 'payment_information');

  $form['elements'] = array(
    '#type' => 'fieldset',
    '#title' => 'Elements',
  );

  foreach ($elements as $element) {
    $form['elements'][$element] = array(
      '#title' => $element . ' position',
      '#type' => 'select',
      '#options' => form_layout_get_region_options(),
      '#default_value' => isset($layout['layout']) ? form_layout_find_region_value_by_element($element, $layout['layout']) : 'top_region',
    );
  }

  return $form;
}

/**
 * Submit handler for adding form layouts.
 */
function form_layout_add_submit($form, &$form_state) {
  form_layout_submit($form_state['values']);

  drupal_set_message('Form layout created.');
  $form_state['redirect'] = form_layout_menu_base_path();
}

/**
 * Submit handler for editing form layouts.
 */
function form_layout_edit_submit($form, &$form_state) {
  form_layout_submit($form_state['values']);

  drupal_set_message('Form layout updated.');
  $form_state['redirect'] = form_layout_menu_base_path();
}

/**
 * Helper for the add/edit submit handlers
 *
 * @param array $values  The $form_state['values'] array from the submit handler.
 */
function form_layout_submit($values) {
  // build the layout definition array
  $elements = array('donation', 'donor_information', 'billing_information', 'payment_information');
  $layout_definiiton = array();
  foreach ($elements as $element) {
    $layout_definiiton[$values[$element]][] = $element;
  }

  $machine_name = $values['machine_name'];

  $layout['name'] = $values['title'];
  $layout['layout'] = $layout_definiiton;

  form_layout_save($machine_name, $layout);
}

/**
 * Page callback for the delete confirm form.
 */
function form_layout_delete_confirm($form, &$form_state, $layout) {

  $machine_name = arg(form_layout_menu_base_path_arg_position());
  $form['machine_name'] = array(
    '#type' => 'value',
    '#value' => $machine_name,
  );

  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $layout['name'])),
    form_layout_menu_base_path(),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for delete form.
 */
function form_layout_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    form_layout_delete($form_state['values']['machine_name']);
    drupal_set_message('The form layout was deleted.');
  }

  $form_state['redirect'] = form_layout_menu_base_path();
}

/**
 * Generate the rows for a table that shows the content of each region.
 *
 * @param array $layout
 *
 * @return array  The rows with region and content.
 */
function form_layout_table_rows($layout) {
  $rows = array();

  foreach ($layout['layout'] as $region_name => $region_contents) {
    $rows[] = array($region_name, implode(', ', $region_contents));
  }

  return $rows;
}

/**
 * Machine name exists callback.
 *
 * @param string $name
 *
 * @return bool  Whether the machine name already exists.
 */
function form_layout_machine_name_exists($name) {
  // special case
  if ($name == 'none') {
    return TRUE;
  }

  $layouts = variable_get('form_layout', array());

  return (in_array($name, array_keys($layouts)));
}

/**
 * Finds the region name for a given element name.
 *
 * @param string $needle  The element name you're looking for.
 * @param array $haystack  The layout definition array.
 *
 * @return string  The region name
 */
function form_layout_find_region_value_by_element($needle, $haystack) {
  foreach ($haystack as $region) {
    if (isset($region[$needle])) {
      return $region[$needle];
    }
  }
  return '';
}
