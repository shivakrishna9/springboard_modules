<?php

$filename = getcwd() . '/names_test.csv';
$url = 'https://43.sb.local/sb-api/forms/submit.json?form_id=2&api_key=121b07fd4a002101040f3abab2344034';

$amounts = array(10, 25, 50, 100, 250);

if(($handle = fopen($filename, 'r')) !== FALSE)
{
  while(($row = fgetcsv($handle, 0, ',')) !== FALSE)
  {
    $first_name = trim($row[0]);
    $last_name = trim($row[1]);
    $email = trim($row[2]);
    $address = trim($row[3]);
    $city = trim($row[4]);
    $state = trim($row[5]);
    $zip = trim($row[6]);

    $submission = array(
      'amount' => 10,
      'other_amount' => '',
      'quantity' => '1',
      'first_name' => $first_name,
      'last_name' => $last_name,
      'mail' => $email,
      'address' => $address,
      'city' => $city,
      'country' => 'US',
      'state' => $state,
      'zip' => $zip,
      'payment_method' => 'credit',
      'payment_fields' => array(
        'bank account' => array(
          'accNum' => '',
          'routingNum' => '',
        ),
        'credit' => array(
          'card_number' => '4111111111111111',
          'expiration_date' => array(
            'card_expiration_month' => '10',
            'card_expiration_year' => '2014',
          ),
          'card_cvv' => '111',
          'card_type' => FALSE,
        ),
      ),
      'recurs_monthly' => array(
        'recurs' => FALSE,
      ),
    );

    $payload = json_encode($submission);
    print_r($payload);

    //open connection
    $ch = curl_init();

    //set the url, number of POST vars, POST data
    curl_setopt($ch,CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
    curl_setopt($ch,CURLOPT_POSTFIELDS, $payload);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, FALSE);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('X-CSRF-TOKEN: bTQ5V4d9ArYCSQORv7mdcV5Qgyc7WAKcqmKYexhvpIU', 'Content-Type: application/json', 'Content-Length: ' . strlen($payload)));
    //execute post
    $result = curl_exec($ch);
    print_r($result);
    //close connection
    curl_close($ch);

  }

  fclose($handle);
}

//curl -i -k -H 'X-CSRF-TOKEN: bTQ5V4d9ArYCSQORv7mdcV5Qgyc7WAKcqmKYexhvpIU' -H "Content-Type: application/x-www-form-urlencoded" -d 'amount=10&first_name=Allen&last_name=APITest&mail=allen.freeman@jacksonriver.com&address=104 Briarburn Lane&city=Holly Springs&country=US&state=NC&zip=27540&payment_method=credit&card_number=4111111111111111&card_expiration_date=6&card_expiration_year=2014&card_cvv=111&recurs_monthly=false' https://43.sb.local//sb-api/forms/submit\?api_key\=121b07fd4a002101040f3abab2344034\&form_id\=2