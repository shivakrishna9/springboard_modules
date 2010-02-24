<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the webform_user module.
 */

/**
 * Form builder. Select the salesforce object relationship field.
 */ 
function webform_user_field_selection_form(&$form_state = NULL, $node) {
	// only show the related field drop down if the webform has been related to a salesforce object
	if (!empty($node->sf_webform_object) && $node->is_webform_user) {
		$form['webform_user_related_field'] = array(
			'#prefix' => t("<div>This webform is currently mapped to the <strong>$node->sf_webform_object</strong> object. Please select the field in this object that references the <strong>$node->related_object</strong> object.</div>"), 
			'#type' => 'select',
			'#options' => _get_object_relation_fields($node->sf_webform_object),
			'#title' => t('Lookup field'),
			'#required' => TRUE,
			'#default_value' => $node->foreign_key_field,
			'#description' => t("The field in the $node->sf_webform_object object that references the $node->related_object."),
		);
		
		$form['nid'] = array(
			'#type' => 'hidden',
			'#value' => $node->nid,
		);
		
		$form['vid'] = array(
			'#type' => 'hidden',
			'#value' => $node->vid,
		);
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Save',
		);
	}
	else {
		$form['message'] = array(
			'#value' => 'The webform must first be mapped to a Salesforce object before the relationship field can be selected. Click the Salesforce mapping tab to create the map.',
		);
	}
	return $form;
}

/**
 * Form submission handler.
 */
function webform_user_field_selection_form_submit($form, &$form_state) {
	$foreign_key_field = $form_state['values']['webform_user_related_field'];
	$nid = $form_state['values']['nid'];
	
	db_query("UPDATE {webform_user} SET foreign_key_field = '%s' WHERE nid = %d", $foreign_key_field, $nid);
	drupal_set_message(t('Relationship field successfully set to !field', array('!field' => $foreign_key_field)));
}