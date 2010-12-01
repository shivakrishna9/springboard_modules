<?php
// $Id: fundraiser.options.inc,v 1.0.0.0 2010/11/22 03:19:49 pcave Exp $

/**
 * @file
 * A collection of built-in select list options for Fundraiser.
 */

/**
 * Private implementation of hook_webform_select_options_info().
 *
 * @see webform_webform_select_options_info().
 */
function _fundraiser_webform_select_options_info() {
  $items = array();

  $items['uc_states'] = array(
    'title' => t('Ubercart defined states'),
    'options callback' => 'fundraiser_options_uc_states',
    'file' => 'includes/fundraiser.options.inc',
  );
  
  $items['uc_countries'] = array(
    'title' => t('Ubercart defined countries'),
    'options callback' => 'fundraiser_options_uc_countries',
    'file' => 'includes/fundraiser.options.inc',
  );

  return $items;
}

/**
 * Options list containing a list of ubercart defined states
 */
function fundraiser_options_uc_states() {
  return array();
}

/**
 * Options list containing a list of ubercart defined countries
 */
function fundraiser_options_uc_countries() {
  return array();
}

