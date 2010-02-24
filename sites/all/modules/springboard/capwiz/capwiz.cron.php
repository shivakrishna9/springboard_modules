<?php
// $Id$

/**
 * @file capwiz.cron.php
 * Commands to execute with cron
 *
 * @author Derek Dunagan <derek.dunagan@jacksonriver.com>
 */


// If run directly via cron, bootstrap only as much Drupal as we need
if (!function_exists('variable_get')) {

  require_once './includes/bootstrap.inc';
  require_once './includes/common.inc';
  require_once './includes/cache.inc';
  require_once './includes/module.inc'; // Used to invoke hook_watchdog() and hook_user()
  require_once './includes/mail.inc'; // Used to send users emails of their new account
  require_once './includes/path.inc'; // Allows correct URLs to be in emails
  require_once './includes/unicode.inc'; // Needed for mime header encoding of emails

  drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

  // t() needs the default language set.
  $GLOBALS['language'] = language_default();

  // Get our Capwiz variables and any variables needed by new user email messages.
  $result = db_query('SELECT * FROM {variable} WHERE name LIKE "capwiz_%%" OR name LIKE "user_mail_%%" OR name IN ("site_name", "site_mail", "clean_url", "user_email_verification")');
  while ($variable = db_fetch_object($result)) {
    $GLOBALS['conf'][$variable->name] = unserialize($variable->value);
  }

  // Load just the modules we need, but make sure they are enabled.
  //
  // TODO: When capwiz.cron.php is run directly, any non-core modules that
  //       implement hook_watchdog() would not get "hooked." Because
  //       Drupal does not have a hook that alerts all modules when a
  //       module is enabled or disabled, there is not a clean,
  //       behind-the-scenes way to keep track of which enabled modules
  //       implement it. One possible workaround is to create checkboxes,
  //       for each enabled module that implements it, on the Capwiz
  //       settings form, and the user can check the ones that need to
  //       handle log messages when Capwiz syncs via a direct crontab.
  //       This is definitely not ideal, but it's the best direction I
  //       can think of so far.
  $module_list = array();
  $modules_to_load_if_enabled = "'user', 'profile', 'dblog', 'syslog', 'capwiz'"; // We need user because watchdog needs it.
  $result = db_query("SELECT name, filename FROM {system} WHERE type = 'module' AND name IN ($modules_to_load_if_enabled) AND status = 1");
  while ($module = db_fetch_array($result)) {
    require_once './' . $module['filename'];
    $module_list[$module['name']] = $module;
  }
  module_list(FALSE, TRUE, FALSE, $module_list);

  // Refresh the static cache for all hooks, so they include our new module list.
  module_implements(NULL, TRUE, TRUE);

  // Log in the root user after hooks have been refreshed, so hook_user() works.
  $GLOBALS['user'] = user_load(1);


  /**
   * Let theme functions just call their default implementation.
   *
   * syslog.module needs this.
   */
  function theme() {

    $arguments = func_get_args();
    $hook = array_shift($args);

    $function = "theme_$hook";
    if (function_exists($function)) {
      return call_user_func_array($function, $arguments);
    }

    // Do not try to simulate the template system.
    // If this becomes a problem, address it then.
    return NULL;
  }
}

// Get all new/updated accounts from Capwiz.
capwiz_merge_accounts();

// Get all new advocacy actions taken by Capwiz users.
capwiz_save_actions();

