<?php

/**
 * @file
 * Defines the inline entity form controller for Commerce Line Items.
 */

class PageWrappersFileInlineEntityFormController extends EntityInlineEntityFormController {

  public function tableFields($bundles) {
    $fields['name'] = array(
      'type' => 'property',
      'label' => t('File Name'),
      'formatter' => 'file_default',
      'weight' => 0,
    );
    $fields['owner'] = array(
      'type' => 'property',
      'label' => t('Owner'),
      'formatter' => 'text_default',
      'weight' => 1,
    );
    $fields['url'] = array(
      'type' => 'property',
      'label' => t('URL'),
      'formatter' => 'text_default'
    );
    $fields['timestamp'] = array(
      'type' => 'property',
      'label' => t('Last Updated'),
      'formatter' => 'date_default',
      'settings' => array('format_type' => 'medium'),
      'weight' => 2,
    );
    return $fields;
  }

  /**
   * Overrides EntityInlineEntityFormController::tableFields().
   */
  public function entityForm($entity_form, &$form_state) {
    $bundle = file_type_load($entity_form['#entity']->type);

    $file = $entity_form['#entity'];
    $file_extensions = file_type_get_valid_extensions($bundle);
    $options = array(
      'file_extensions' => implode(' ', $file_extensions),
    );

    module_load_include('inc', 'file_entity', 'file_entity.pages');
    $entity_form['upload'] = array(
      '#type' => 'managed_file',
      '#title' => t('Upload a new file'),
      '#upload_location' => file_entity_upload_destination_uri($options),
      '#upload_validators' => file_entity_get_upload_validators($options),
      '#progress_indicator' => 'bar',
      '#required' => TRUE,
      '#pre_render' => array('file_managed_file_pre_render', 'file_entity_upload_validators_pre_render'),
      '#default_value' => isset($file->fid) ? $file->fid : NULL,
      '#weight' => -1,
    );

    field_attach_form('file', $file, $entity_form, $form_state);

    return $entity_form;
  }

  /**
   * Overrides EntityInlineEntityFormController::settingsForm().
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);

    // Adding existing entities is not supported for line items.
    $form['allow_existing']['#access'] = TRUE;

    return $form;
  }

  public function entityFormSubmit(&$entity_form, &$form_state) {
    // The Drupal core 'managed file' is already created when the file is originally
    // uploaded. The file entity associated with that managed file will use the same file ID (fid)
    // as its entity ID. Creating a "new" file entity, then, is really just updating the fields
    // related to the entity ID already created in the file_managed table.
    if(isset($entity_form['upload']['#file'])){
      // Replace the generic "new" entity object with the actual file entity.
      $entity_form['#entity'] = $entity_form['upload']['#file'];
    }
    parent::entityFormSubmit($entity_form, $form_state);
  }
}