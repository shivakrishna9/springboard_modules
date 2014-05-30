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
its features. Path aliases to various modules configuration and related pages
are utilized to create the appearance of a contained platform separate from 
Drupal. 

Adding a new page to the Springboard UX (utilizing a path alias) requires 3 
steps:

1. Create new alias in springboard_admin_inbound_path_alter (implementation of hook_inbound_path_alter in springboard_admin.module)
   - Set $path and $original_path in hook_url_inbound_alter(&$path, $original_path, $path_language). 
       - $path: The new Springboard path being constructed that begins with 'springboard/'. For example 'springboard/node/%/form-components'is an alias for 'node/%/webform'. 
       - $original_path: The standard Drupal path you are altering. Example: 'node/%/webform'.
       - $path_language: The language of the path.
  - Example:
     if (preg_match([regex for alias], $path, $matches)) {
       $path = [actual path];
     }
   - 
2. Add redirect in springboard_admin_init (implementation of hook_init in springboard_admin.module)
   - If user reaches a page via the Drupal admin path, redirect them to the 
     Springboard alias.
   - Be aware of the potential for infinite redirect loops.
   - Example:
     if (preg_match([regex for Drupal path], $path, $matches)) {
      drupal_goto([Springboard path], array('query' => $query, 'alias' => TRUE));
     }
3. Add a link to springboard_admin_menu (springboard_admin.menu.inc)

Springboard Custom Pages
------------------------------------------------------------------------------
