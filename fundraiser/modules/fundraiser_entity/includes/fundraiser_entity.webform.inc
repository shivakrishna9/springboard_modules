<?php

/**
 * Menu callback form, provides user interface to set up form fields to donation entity fields mapping.
 *
 * Webform User can map webform components to user profile fields. Values added to the user profile for these fields
 * will be used to populate default values for webform components they are mapped to.
 *
 *  Example: we have a user profile field "First Name". We map this to a webform component "First Name" with a form key of first_name.
 *  Note that the name and form key of the component are irrelevant, all that matters is a relationship has been established between
 *  this profile field and this webform component.
 *
 *  If an anonymous user submits a webform a new user account is created using the email address provided. Whatever value the user entered
 *  in the "First Name" webform component will be used to set the value of the "First Name" profile field as well.
 *
 *  For existing users, mapped profile fields are used to provide default values for webform components. For example, given the mapping
 *  described above, if an existing user views a webform, the "First Name" component field will contain a default value that matches
 *  the value in the "First Name" field in that user's profile.
 */
function fundraiser_entity_webform_mapping_form($form, $form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'mapping' => array(),
  );
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  $form['info'] = array(
    '#value' => t('This form allows you to map the submitted form values to Donation entity fields. ' .
      'When the webform form is submitted, a donation entity will be created with the values from the form.'),
    '#weight' => 0,
  );

  $header = array(
    t('Webform Component'),
    t('Donation Field'),
  );
  // The NULL option and any core account fields have to be handled separately.
  $options = array(
    '' => t('--Select One--'),
  );

  $fields = _fundraiser_entity_get_donation_fields();

  foreach ($fields as $name => $label) {
    $options[$name] = $label;
  }

  // we like alphabetized dropdowns.
  asort($options);
  $form['mapping'] = array(
    '#prefix' => '<div id="fields">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => array(),
  );
  foreach ($node->webform['components'] as $cid => $component) {
    if (isset($node->map[$cid])) {
      $default = $node->map[$cid];
    }
    else {
      $default = 0;
    }
    if ($component['type'] !== 'fieldset') {
      $select = array(
        '#id' => 'component-' . $cid,
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $default,
      );
      $form['mapping'][$cid] = array(
        'component_cid' => &$select,
      );
      $form['mapping']['#rows'][] = array(
        array('data' => $component['name']),
        array('data' => &$select),
      );
      unset($select);
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}
