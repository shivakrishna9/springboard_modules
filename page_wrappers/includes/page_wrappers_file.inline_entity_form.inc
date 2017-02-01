<?php

/**
 * @file
 * Defines the inline entity form controller for Commerce Line Items.
 */

class PageWrappersFileInlineEntityFormController extends EntityInlineEntityFormController
{


  public function tableFields($bundles)
  {
    $fields['filename'] = array(
      'type' => 'field',
      'label' => t('File Name'),
      'formatter' => 'file_default',
      'weight' => 4,
    );
    $fields['title'] = array(
      'type' => 'field',
      'label' => 'Title',
      'formatter' => 'default',
    );
    return $fields;
  }

  /**
   * Overrides EntityInlineEntityFormController::tableFields().
   */
  public function entityForm($entity_form, &$form_state)
  {

    $bundle = file_type_load($entity_form['#entity']->type);

    $file = $entity_form['#entity'];
    $extra_fields = field_info_extra_fields('file', $file->type, 'form');
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

    $entity_form['filename'] = array(
      '#type' => 'hidden',
      '#title' => t('filename'),
      '#required' => FALSE,
      '#default_value' => isset($file->filename) ? $file->filename : NULL,
      '#needs_save' => TRUE,
    );

    $entity_form['title'] = array(
      '#type' => 'text',
      '#title' => t('Title'),
      '#required' => FALSE,
      '#default_value' => isset($file->title) ? $file->title : NULL,
    );

    field_attach_form('file', $file, $entity_form, $form_state);
    
    return $entity_form;


  }

  /**
   * Overrides EntityInlineEntityFormController::settingsForm().
   */
  public function settingsForm($field, $instance)
  {
    $form = parent::settingsForm($field, $instance);

    // Adding existing entities is not supported for line items.
    $form['allow_existing']['#access'] = TRUE;

    return $form;
  }

  public function entityFormSubmit(&$entity_form, &$form_state)
  {
    $file = $entity_form['upload']['#file'];
    if($entity_form['#entity']->filename == null) {
      $entity_form['#entity']->filename = $file->filename;

      if (isset($form_state['values']['page_wrappers_css']) && $form_state['values']['page_wrappers_css'][$entity_form['#parent_language']]['form']['upload'] == $file->fid) {
        $form_state['values']['page_wrappers_css'][$entity_form['#parent_language']]['form']['filename'] = $file->filename;
    }
    }

  }

  public function save($entity, $context)
  {
    dpm($entity);
  }
}