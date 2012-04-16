<?php

/**
 * @file
 * Theme related functions for the fundraiser module.
 * These are isolated to make them easy to locate and maintain.
 */

/**
 * Implementation of hook_theme().
 */
function fundraiser_theme($existing, $type, $theme, $path) {
  return array(
    'fundraiser_mapping_options' => array(
      'render element' => 'element',
    ),
    // Shows ask amounts as table in fundraiser setting forms.
    'fundraiser_ask_amounts' => array(
      'render element' => 'element',
    ),
    // Shows CC year and data as a merged widget.
    'fundraiser_credit_card_expiration_date' => array(
      'render element' => 'element',    
    ),
    // Shows confirmaiton pages.
    'fundraiser_confirmation' => array(
      'variables' => array('node' => NULL, 'message' => NULL),
      'template' => 'templates/fundraiser-confirmation',
      'pattern' => 'fundraiser_confirmation_[0-9]+',
    ),
  );
}

/**
 * Implements hook_theme_registry_alter().
 */
function fundraiser_theme_registry_alter(&$theme_registry) {
  // Tell the theme registry to look in this module's directory for theme files.
  $theme_registry['page']['theme path'] = drupal_get_path('module', 'fundraiser') . '/templates';
}

/**
 * Implements hook_preprocess_page().
 * Add a template suggestion that renders a blank page template for our confirmation page token picker.
 */
function fundraiser_preprocess_page(&$variables) {
  if (arg(2) == 'confirmation') {
    // Add the node to $variables.
    $variables['node'] = node_load(arg(1));
  }
}

/**
 * Implemenation of hook_preprocess_fundraiser_confirmation().
 * http://api.drupal.org/api/drupal/modules--system--theme.api.php/function/hook_preprocess/7
 */
function fundraiser_preprocess_fundraiser_confirmation(&$vars) {
  $message = $vars['message'];
  if (!empty($message)) {
    // Set the message for the template.
    $vars['confirmation_message'] = $message;
  }
}

/**
 * Theme Fundraiser mapping form.
 */
function theme_fundraiser_mapping_options($element) {
  $element = $element['element'];
  $rows = array();
  foreach (element_children($element) as $cid) {
    if (!empty($element[$cid])) {
      $row = array();
      $row[] = $element[$cid]['#label'];
      $row[] = drupal_render($element[$cid]);
      $rows[] = $row;
    }
  }
  $header = array("Webform Component", "Profile Field");
  $output = drupal_render($form['info']);
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'fundraiser_mapping_form')));
  return $output;
}

/**
 * Theme the credit card expiration date form field. This places month and year beside each other.
 */
function theme_fundraiser_credit_card_expiration_date($element) {
  $element = $element['element'];
  $element['card_expiration_month']['#title'] = t('Expiration date');
  $element['card_expiration_year']['#title'] = '';
  $month = drupal_render($element['card_expiration_month']);
  $year = drupal_render($element['card_expiration_year']);
  preg_match('#<select.*/select>#ms', $year, $matches);
  $month = str_replace('</select>', "</select>$matches[0]", $month);
  return '<div class=\'expiration-date-wrapper clear-block\'>' . $month . '</div>';
}

/** 
 * Theme the ask amounts table on webform creation.
 */
function theme_fundraiser_ask_amounts($element) {
  $element = $element['element'];
  $header = array( t('Ask amount'), t('Label'), t('Remove when saved') );
//  $header = array( t('Ask amount'), t('Label'));
  $rows = array();
  foreach (element_children($element) as $index) {
    $amount = drupal_render($element[$index]['amount']);
    $label = drupal_render($element[$index]['label']);
    $remove = drupal_render($element[$index]['remove']);
    $rows[] = array($amount, $label, $remove);
//    $rows[] = array($amount, $label);
  }
  return theme('table', array('header' => $header, 'rows' => $rows));
}
