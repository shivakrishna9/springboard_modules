<?php

/**
 * Adds functionality required by the Market Source module to the Mobile Detect
 * library.
 */
class Market_Source_Mobile_Detect extends Mobile_Detect {

  /**
   * Gets an array of device properties from the current user agent string.
   *
   * @return array
   */
  public function getDeviceProperties() {
    $properties = array();

    $phone = $this->matchDeviceProperty($this->getPhoneDevices());
    $tablet = $this->matchDeviceProperty($this->getTabletDevices());

    if (!empty($phone)) {
      $properties['type'] = 'Phone';
      $properties['name'] = $phone;
    }
    else if (!empty($tablet)) {
      $properties['type'] = 'Tablet';
      $properties['name'] = $tablet;
    }

    $properties['os'] = $this->matchDeviceProperty($this->getOperatingSystems());
    $properties['browser'] = $this->matchDeviceProperty($this->getBrowsers());

    return $properties;
  }

  /**
   * Attempts to match a known device property (OS, browser, etc) to the
   * current user agent string.
   *
   * @param array $options
   *   Array of device property names mapped to associated regex strings.
   *
   * @return mixed
   *   String if property matches or NULL otherwise.
   */
  public function matchDeviceProperty($options) {
    foreach ($options as $name => $regex) {
      if ($this->match($regex)) {
        return $name;
      }
    }

    return NULL;
  }

}
