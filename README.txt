
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * Release Notes

INTRODUCTION
------------

Current Maintainer: Jackson River http://jacksonriver.com

INSTALLATION
------------

This project contains **only** the Springboard-specific modules: it is not a ready-to use Springboard package.

You should only download or checkout this project if you have an existing Drupal site where you'd like to install Springboard functionality.  If that is the case, see the documentation on GoSpringboard.com for instructions: [Installing In An Existing Drupal Site](http://www.gospringboard.com/documentation/installing-springboard/installing-existing-drupal-site)

If you are creating a new site from scratch, you have two options:

* The easy way: [follow the installation instructions on GoSpringboard.com](http://www.gospringboard.com/documentation/installing-springboard/installing-distribution)
* The nerdy way: Checkout the [Springboard-Build](https://github.com/JacksonRiver/Springboard-Build) project, and use [drush make](http://drupal.org/project/drush_make) to install it.


3.3 RELEASE NOTES
-----------------

New modules:

Webform Dupe Cop - this is a new module that polices webform submissions for duplicate entries. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/186

Springboard Versions - this module tracks module versions for Springboard modules. Used by JR developers to quickly identify any Springboard modules that need to be updated. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/185

Sage managed gateway - gateway manager integration for Sage payment gateways. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/181

Gateway Switcher - utility module that permits switching payment gateway assignments in bulk. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/180

Webform Add to All - utility module that permits bulk adding webform components to multiple forms simultaneously. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/150

Sustainer Pause - this module users with recurring donations to pause payments without cancelling. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/191

Sustainer Upsell - this module prompts users with recurring donations to increase their donations. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/189

Feature changes/new features:

Batch sizes by type - the batch size is now configurable per type of integration. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/182

Market Source fundraiser dependency removed Market source is no longer dependent upon Fundraiser. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/163

Webform User fundraiser dependency removed Webform User is no longer dependent upon Fundraiser. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/144

Sustainer order failure message - When a sustaining order fails a comment is now added to the order in Ubercart. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/171/files

Other amount error message - a new error message has been added to donation forms. This error is triggered when a user selects "other amount", fills in the other amount text field, and then changes the ask amount radio button to some other value. This message is intended to clarify user intent. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/158

VBO Salesforce resync Resync Webform submission to Salesforce using Views Bulk Operation. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/157

Autocomplete disabled for CC and CVV fields browser autocomplete has been turned off for Credit Card Number and CVV fields on donation forms. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/151

Billing updates now supported for managed and unmanaged gateways. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/149

Donation Form internal titles are no longer automatically overwritten when the node title is changed. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/148

Queue processor cron lock The standalone cron process for queue processor now actively prevents concurrent cron processes. This change protects against deadlocks and duplicate records syncing to salesforce. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/147

Donation form reference transactions This change allows donation processing with stored payment gateway reference id's instead of user-supplied CC details. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/145

Fundraiser now supports multiple content types It is now possible to attach Fundraiser donation forms to multiple content types. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/143

Passing additional webform field data to Authorize.net Donation forms can now be configured to pass as many as five additional fields to authorize.net when a credit transaction is processed. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/137

Webform User profile fields configurable by node type site administrators may now configure which profile fields have webform components added by Webform User on a per-content-type basis. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/133

Optional Market Source field persistence site administrators may now configure persistence settings for individual Market Source fields. Field values may persist for a single pageview or for the duration of a user session. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/156

Webform Token Selector now supports ckeditor. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/119

Bugfix:

Webform User UID 0 bug - under certain conditions profile values were being set for the anonymous user account. This fix ignores anonymous account profile values. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/162

Confirmation Template list now optionally filtered by OG access. When OG filtering is turned on edit links were being displayed for templates that should not have been editable due to OG permissions. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/167

Fundraiser js problems on validation error Fixes problems with javascript that were encountered after a donation form is submitted with validation errors. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/161

Fundraiser missing gateway configuration data Config data for managed gateways was not being returned to fundraiser_process_recurring_donations() when the charge function for the donation was determined. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/160/files

Protect node settings during node_save() minor change to Fundraiser that attempts to protect node values from being overwritten with NULL when a node is being saved without all of it's properties set. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/157

Email confirmation "from name" punctuation issues From names that included puctuation were failing. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/152/files

Webform User now creates users when webform is set to redirect. Previously a user account would not be created for an anonymous submission under these conditions. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/142

Anonymous users seeing incorrect default country. On donation forms, under certain conditions anonymous users were seeing the country select box default to "Afghanistan" regardless of what the component default was set to. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/140

Fixed ask amount validation Ask amount validation was only being applied to the "other amount" field. Now applied to the ask amount field as well if it is a textbox (no ask amounts configured). Pull request: https://github.com/JacksonRiver/springboard_modules/pull/136

Market source campaign ID A misnamed constant prevented campaign information from being registered. Naming corrected. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/192

Developer notes - additional code changes:

fundraiser_order_decline() modified - this change is required for future module development and should have no impact on existing site implementations. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/176

Pagewrapper js files now added with drupal_add_js(). This change gets our code in line with Drupal community best practices. No impact on existing site implementations. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/173/files

Secure Prepopulate welcome messages refactored to support template preprocessing. This allows 3rd party modules to alter the welcome message as needed. This change is required for future module development and should have no impact on existing site implementations. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/172

New confirmation page template variables have been added to Fundraiser's implementation of confirmation pages. This change allows 3rd party modules to easily alter or extend the confirmation page message and is required for future module development. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/169

Fundraiser order table theme changes. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/166/files

Webform Reorder This legacy module was removed from product. Advances in contrib modules and core have deprecated this module, which is no longer in use on any of our client sites. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/159

Email trim Email addresses submitted via a donation form now have whitespace trimmed before validation is performed. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/154

Removed drush integration Legacy drush code was removed from the Salesforce Management API module. This code is no longer used during our normal build process. Pull request: https://github.com/JacksonRiver/springboard_modules/pull/153

Updated fundraiser_node_settings to pass parent $form_state. $form_state from the parent form is required to utlize posted values during ahah form rebuild. Pull request: $form_state from the parent formm required to utlize posted values during ahah form rebuild.

