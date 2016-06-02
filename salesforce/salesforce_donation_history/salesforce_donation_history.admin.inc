<?php
/**
 * @file
 * Admin form definition for salesforce_donation_history.
 */

function salesforce_donation_history_admin() {
  $form = array();
  $form['salesforce_donation_history_query'] = array(
    '#type' => 'textarea',
    '#title' => t('Opportunities query'),
    '#default_value' => variable_get('salesforce_donation_history_query', DONATION_HISTORY_DEFAULT_SOQL),
    '#description' => "Enter the Salesforce SOQL query used to get opportunities. The query must end in <samp>WHERE AccountId = ':accountId'</samp>. Do not include a LIMIT clause.",
  );
  $form['#validate'][] = 'salesforce_donation_history_admin_validate';
  $form = system_settings_form($form);
  return $form;
}

/**
 * Validation callback for salesforce_donation_history_admin form.
 */
function salesforce_donation_history_admin_validate($form, &$form_state){
  // $result = salesforce_donation_history_test_query($form_state['input']['salesforce_donation_history_query']);
  if (!salesforce_donation_history_test_query($form_state['input']['salesforce_donation_history_query'])) {
    form_set_error('salesforce_donation_history_query', t('There was a problem performing this query. A sample record could not be found.'));
  }
}

/**
 * Perform a test query of the given SOQL.
 *
 * Replaces the WHERE claue of the SOQL query with LIMIT 1. This should return
 * the first available result, including all of the fields included in the
 * query.
 */
function salesforce_donation_history_test_query($query) {
  $soql = strtr($query, array("WHERE AccountId = ':accountId'" => 'LIMIT 1'));
  // @TODO maybe do this in a try/catch to avoid unexpected error message if the
  // SOQL is malformed.
  $sfapi = salesforce_get_api();
  $soap = new SalesforceSoapPartner($sfapi);
  $response = $soap->query($soql);
  $result = new QueryResult($response);
  // Print createQueryResultTable($response->records, 0);.
  if ($result->size != 1) {
    return FALSE;
  }
  return TRUE;
}

function createQueryResultTable($records, $rowNum) {
  $table = "<table id='query_results'>\n";
  // Call shared recusive function for header printing.
  $table .= "<tr><th>&nbsp;</th><th>";
  if ($records[0] instanceof SObject) {
    $table .= implode("</th><th>", getQueryResultHeaders($records[0]));
  } else {
    $table .= implode("</th><th>", getQueryResultHeaders(new SObject($records[0])));
  }
  $table .= "</th></tr>\n";
  // Print the remaining rows in the body.
  foreach ($records as $record) {
    // Call shared recusive function for row printing.
      $table .= "<tr><td>" . $rowNum++ . "</td><td>";
    if ($record instanceof SObject) {
      $row = getQueryResultRow($record);
    } else {
      $row = getQueryResultRow(new SObject($record));
    }
    for ($i = 0; $i < count($row); $i++) {
      if ($row[$i] instanceof QueryResult && !is_array($row[$i])) $row[$i] = array($row[$i]);
      if (isset($row[$i][0]) && $row[$i][0] instanceof QueryResult) {
        foreach ($row[$i] as $qr) {
          $table .= createQueryResultTable($qr->records, 1);
          if ($qr != end($row[$i])) $table .= "</td><td>";
        }
      }
      else {
        $table .= $row[$i];
      }
      if ($i + 1 != count($row)) {
        $table .= "</td><td>";
      }
    }
    $table .= "</td></tr>\n";
  }
  $table .= "</table>";
  return $table;
}

function getQueryResultHeaders($sobject, $tail="") {
  if (!isset($headerBufferArray)) {
    $headerBufferArray = array();
  }
  if (isset($sobject->Id) && !isset($sobject->fields->Id)) {
    $headerBufferArray[] = $tail . "Id";
  }
  if (isset($sobject->fields)) {
    foreach ($sobject->fields as $fieldname => $fieldvalue) {
      if ($fieldvalue instanceof SObject) {
        $recurse = getQueryResultHeaders($fieldvalue, $tail . htmlspecialchars($fieldvalue->type,ENT_QUOTES) . ".");
        $headerBufferArray = array_merge($headerBufferArray, $recurse);
      }
      else {
        $headerBufferArray[] = $tail . htmlspecialchars($fieldname,ENT_QUOTES);
      }
    }
  }
  if (isset($sobject->queryResult)) {
    if (!is_array($sobject->queryResult)) $sobject->queryResult = array($sobject->queryResult);
    foreach ($sobject->queryResult as $qr) {
      $headerBufferArray[] = $qr->records[0]->type;
    }
  }
  return $headerBufferArray;
}

function getQueryResultRow($sobject, $escapeHtmlChars=true) {
  if (!isset($rowBuffer)) {
    $rowBuffer = array();
  }
  if (isset($sobject->Id) && !isset($sobject->fields->Id)) {
    $rowBuffer[] = $sobject->Id;
  }
  if (isset($sobject->fields)) {
    foreach ($sobject->fields as $datum) {
      if ($datum instanceof SObject) {
        $rowBuffer = array_merge($rowBuffer, getQueryResultRow($datum,$escapeHtmlChars));
      } else {
        $rowBuffer[] = ($escapeHtmlChars ? htmlspecialchars($datum,ENT_QUOTES) : $datum);
      }
    }
  }
  if (isset($sobject->queryResult)) {
      $rowBuffer[] = $sobject->queryResult;
  }
  return $rowBuffer;
}
