<?php

class SpringboardP2pPersonalCampaignProgressField {

  public function validate($field, $langcode, $items, &$errors) {

  }

  public function isEmpty($item) {
    return FALSE;
  }

  public function presave(&$items) {
    foreach ($items as $delta => $item) {
      $record = array(
        'submissions' => $items[$delta]['campaign_progress']['submissions'],
        'amount' => $items[$delta]['campaign_progress']['amount'],
      );

      $items[$delta] = $record;
    }

  }

  // Probably not needed.
  public function delete(&$items) {

  }

  // Not needed.
  public function save($goal_set_id, $rows) {

  }

  // Not needed.
  public function load($goal_set_id) {

  }

  public function widgetForm($instance, $items, $delta, $element) {
    $widget = $element;

    switch ($instance['widget']['type']) {
      case 'springboard_p2p_fields_personal_campaign_progress_widget':

        // Make this a fieldset with the three text fields.
        $widget += array(
          '#type' => 'fieldset',
          '#title' => t('Campaign progress'),
          // #delta is set so that the validation function will be able
          // to access external value information which otherwise would be
          // unavailable.
          '#delta' => $delta,
        );

        $widget['submissions'] = array(
          '#type' => 'textfield',
          // '#disabled' => TRUE,
          '#default_value' => isset($items[$delta]['submissions']) ? $items[$delta]['submissions'] : 0,
          '#title' => t('Submissions'),
        );

        $widget['amount'] = array(
          '#type' => 'textfield',
          // '#disabled' => TRUE,
          '#default_value' => isset($items[$delta]['amount']) ? $items[$delta]['amount'] : 0,
          '#title' => t('Amount'),
        );

        break;
    }

    $element['campaign_progress'] = $widget;
    return $element;
  }

  public function widgetError($element, $error) {

  }

  public function formatterView($entity_type, $entity, $items, $display) {
    $element = array();

    if ($display['type'] == 'springboard_p2p_fields_personal_campaign_progress_default') {
      $campaign = $entity->field_p2p_campaign[$entity->language][0]['entity'];
      $goal_set_id = $campaign->field_p2p_campaign_goals[$campaign->language][0]['goal_set_id'];
      $goals_field = new SpringboardP2pCampaignGoalsField();
      $goals = $goals_field->load($goal_set_id);
      dpm($goals);
      dpm($entity->type);
      if (isset($goals[$entity->type])) {
        $goal = $goals[$entity->type];
        dpm($goal);
        if ($goal['enabled']) {
          $goal_type = $goal['type'];
          dpm($goal_type);
        }
      }

      foreach ($items as $delta => $value) {
        $element[$delta]['campaign_progress'] = array(
          '#markup' => '<div>' . $items[$delta]['submissions'] . ' submissions and $ ' . $items[$delta]['amount'] . ' raised.',
        );
      }
    }

    return $element;
  }

}
