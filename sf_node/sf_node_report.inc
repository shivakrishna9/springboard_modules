<?php
/**
 * @file
 */
/**
 * Form to configure the download of CSV files.
 */
function sf_node_results_download_form(&$form_state, $node) {
  $form = array();

  $form['node'] = array(
    '#type' => 'value',
    '#value' => $node,
  );

  $form['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimited text format'),
    '#description' => t('This is the delimiter used in the CSV/TSV file when downloading Salesforce Errors. Using tabs in the export is the most reliable method for preserving non-latin characters. You may want to change this to another character depending on the program with which you anticipate importing results.'),

    '#options' => array(
      ','  => t('CSV'),

    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Download'),
  );

  return $form;
}

function sf_node_results_download_form_submit(&$form, &$form_state) {
  sf_node_results_download($form_state['values']['node'], $form_state['values']['delimiter']);
}


function sf_node_results_download($node, $options = array()) {

  // Put the name of all fields
  $csv_output = array('Timestamp', 'Drupal Type', 'Error', 'Fieldmap');

  $columns = array('error_time', 'drupal_type', 'error', 'fieldmap');

    foreach ($csv_output as $i => $value) {
      $out .= '"' . $value . '",';
    }
    $out .="\n";
  $result = db_query("SELECT * FROM {salesforce_management_api_error} WHERE did = %d", $node);

    // Add all values in the table
    while ($l = db_fetch_array($result)) {
    foreach ($columns as $j => $value) {
      $out .= '"' . $l["$value"] . '",';
    }
    $out .="\n";
    }
    // Output to browser with appropriate mime type, you choose ;)
    header("Content-type: text/x-csv");
    header("Content-Disposition: attachment;filename=SF-Error-Report(" . date("Y-m-d") . ").csv");
    echo $out;
    exit;

}
