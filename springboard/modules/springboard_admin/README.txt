Overview
------------------------------------------------------------------------------
The Springboard Admin module, along with the Springboard Backend theme, creates
a contorlled user experience, separate from the Drupal administration, for users
of the Springboard platform. Following is a summary of the main features of the
Springboard UX and guidelines for editing and expanding each.

Springboard Custom Theme
------------------------------------------------------------------------------


Springboard Dashboard
------------------------------------------------------------------------------


Springboard Admin Menu
------------------------------------------------------------------------------


Springboard Path Aliases
------------------------------------------------------------------------------
Springboard incorporates a number of custom and contributed modules to provide
its features. Within Springboard, path aliases are used to Sprinboard module 
configuration pages and other administrative pages to create the appearance of a
 contained platform separate from 
Drupal. 

Adding a new page to the Springboard UX (utilizing a path alias) requires 3 
steps:

1. Create new alias in springboard_admin_inbound_path_alter (implementation of
 hook_inbound_path_alter in springboard_admin.module)
   - Set $path and $original_path 
       - $path: The new Springboard path being constructed that begins with
         'springboard/'. For example 'springboard/node/%/form-components' is an 
         alias for 'node/%/webform'. 
       - $original_path: the standard Drupal path you are altering. 
         Example: 'node/%/webform'.
       - $path_language: the language of the path.
  - Example:
     if (preg_match([regex for alias], $path, $matches)) {
       $path = [actual path];
     }
   - 
2. Add redirect in springboard_admin_init (implementation of hook_init in 
    springboard_admin.module)
   - If a user reaches a page via the Drupal admin path, redirect them to the 
     Springboard alias.
   - The user must be logged in with a role that is set to see the Springboard 
     User Experience at /springboard/settings/springboard.
   - Be aware of the potential for infinite redirect loops.
   - Set path to match via regex and $goto_path
       - regex path: the original Drupal path to be redirected.
       - $goto_path: the new Springboard alias.
   - Example:
     if (preg_match([regex for Drupal path], $path, $matches)) {
      $goto_path = '[Springboard path]' . $matches[1];
     }

3. Add a link to springboard_admin_menu (springboard_admin.menu.inc)
   - Add your new menu link to the Springboard navigation 
     in springboard_admin.menu.inc.
   - Example:
    $items['admin/springboard/settings']['_children']['logout'] = array(
      'link_path' => 'user/logout',
      'link_title' => 'Logout',
      'menu_name' => 'springboard_admin_menu',
      'expanded' => 1,
      'customized' => 1,
      'weight' => 3,
    );

Springboard Custom Pages
------------------------------------------------------------------------------
