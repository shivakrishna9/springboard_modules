<?php

/**
 * @file Springboard Social Pull settings.
 */

/**
 * The administative settings form.
 */
function _sba_social_pull_admin_settings_form($form, &$form_state) {
  // Show information about previous fetches:
  /*$form['ssp_status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status report'),
    '#description' => _sba_social_pull_get_status_report(),
  );
  $form['ssp_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
  );*/
  $form['ssp_oauth_test'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth testing tool'),
  );
  $form['ssp_oauth_test']['ssp_oauth_test_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Run OAuth Test',
  );

  return $form;
}

/**
 * Submission of the administrative form.
 */
function _sba_social_pull_admin_settings_form_submit($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case 'Run OAuth Test':
      sba_social_pull_twitter_redirect();
      //_sba_social_pull_execute_oauth_test();
      break;
  }
}


/**
 * Performs an OAuth authentication test.
 */
function _sba_social_pull_execute_oauth_test() {
  require 'OAuth.php';
  $consumer_key  = 'GJhi6b9Rss1Hy0K2dLVlHlivM';
  $consumer_secret = 'WlqOLNvlI9FoHphHUDw5PhvfkjCPQTLEmDFYb1iNdvbPLANKjE';
  $url = 'https://api.twitter.com/oauth/authenticate';
  $args = array();

  $oauth =  new OAuth($consumer_key, $consumer_secret,
    OAUTH_SIG_METHOD_HMACSHA1, OAUTH_AUTH_TYPE_URI);
  $requestTokenInfo = $oauth->getRequestToken('https://api.twitter.com/oauth/request_token');
  drupal_set_message('<pre>' . print_r($requestTokenInfo, 1) . '</pre>');
/*$consumer = new DrupalOAuthConsumer($cc_key, $cc_secret);
$request = DrupalOAuthRequest::from_consumer_and_token($consumer, NULL,"GET", $url, $args);
$request->sign_request(new OAuthSignatureMethod_HMAC_SHA1(), $consumer, NULL);
$url = sprintf("%s?%s", $url, OAuthUtil::build_http_query($args));
$ch = curl_init();
$headers = array($request->to_header());
curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
$rsp = curl_exec($ch);
$results = json_decode($rsp);
drupal_set_message('<pre>' . print_r($rsp, 1) . '</pre>');*/
}

/**
 * Helper function to return a formatted statistics report
 * to be displayed on the settings form.
 */
function _sba_social_pull_get_status_report() {
  $out = '';

  // Gather data about the most recent fetch attempt:
  $last_pull_vars = array(
    'timestamp' => t('Last pull time'),
    'cause' => t('Last pull cause'), // Cron, callback, action, or manual
    'quantity' => t('Last pull quantity'), // How many items were fetched.
    'duration' => t('Last pull duration'), // How long did the fetch take?
    'hashtags' => t('Last pull hashtags'), // Which hashtags were queried?
  );
  foreach ($last_pull_vars as $var_suffix) {
    $last_pull_data[$var_suffix] = variable_get('sba_social_pull_last_pull_' . $var_suffix, 'N/A');
  }
  $out = theme('table', array('header' => $last_pull_vars, 'rows' => array($last_pull_data)));


  return $out;
}
