<?php

class SalesforceContact {
  public function UsernameToken($username, $password) {
  	 $this->Authenticated = true;
  	 	/*
      // Store username for logging
      $this->Username = $username;
      $auth = new $this->AuthClass( $username, $password, get_class($this) ); // You will have to Define method AuthClass
      if ( $auth->IsValid() ){
          $this->Authenticated = true;
      } else {
          $this->ThrowSoapFault( 'auth' );
      }
      */
  }

  /**
  * Update profile method for incoming SOAP message
  * @return unknown_type
  */
	function sync_mem_profiles($message) {
		$this->msg = "Success";
		return $this;
	} // End of sync_mem_profiles
}