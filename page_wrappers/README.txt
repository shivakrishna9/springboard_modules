
-- SUMMARY --

Page Wrappers are a quick way to modify the look and feel of select webforms on your site without resorting to complex changes to your site
theme. This module implements the "page wrapper" content type, which lets you define the markup, styles and JavaScript used to render one or more
webforms on your site. 



-- REQUIREMENTS --

* Node Clone module

* Features module

* CCK
  - Fieldgroup
  - filefield

* Views module

-- INSTALLATION

* This module should be enabled via the Features admin screen: admin/build/features

-- CONFIGURATION --

Once the module is enabled, you may create as many page wrapper nodes as you need to theme your webforms. A page wrapper may be selected on the webform edit screen.

-- TROUBLESHOOTING --

* Webform not rendering correctly, missing CSS or JavaScript: 

Check the file permissions on your files folder. During installation Page Wrapper attempts to create multiple folders:

- files/page_wrappers/css : uploaded stylesheets
- files/page_wrappers/js : uploaded JavaScript files 
- files/page_wrappers/tpl : contains .tpl (template) files

If one or more of these folders is missing or Drupal is unable to write to them, this will lead to broken page wrappers and incorrectly themed webforms.

-- CONTACT --
Current maintainers:
* Phillip Cave (pcave) - http://drupal.org/user/379888
* Allen Freeman (afreeman) - http://http://drupal.org/user/450370
* Sarah Hood (Bastlynn) - http://drupal.org/user/275249


This project has been sponsored by:
* Jackson River
  Cutting edge websites for progressive non-profit organizations.
  Visit http://jacksonriver.com for more information.

