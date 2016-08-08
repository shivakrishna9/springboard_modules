<?php
/**
 * @file
 * Fundraiser functions related to nodes.
 */

/**
 * Implements hook_node_access().
 */
function fundraiser_node_access($node, $op, $account) {
  // Without an idea who the owner is or type is we can't assess access. So
  // don't rock the boat.
  if (isset($node->type) && isset($node->uid) && fundraiser_is_donation_type($node->type)) {
    $is_author = $account->uid == $node->uid;
    switch ($op) {
      case 'create':
        return user_access('create donation form', $account) ? NODE_ACCESS_ALLOW : NODE_ACCESS_IGNORE;
      case 'update':
        return ((user_access('edit own donation form', $account) && $is_author) ||
          user_access('edit any donation form', $account)) ? NODE_ACCESS_ALLOW : NODE_ACCESS_IGNORE;
      case 'delete':
        return ((user_access('delete own donation form', $account) && $is_author) ||
          user_access('delete any donation form', $account)) ? NODE_ACCESS_ALLOW : NODE_ACCESS_IGNORE;
    }
  }
}

/**
 * Implements hook_node_type_insert().
 *
 * Add Fundraiser-related fields when a new node type is created.
 */
function fundraiser_node_type_insert($type) {
  if (fundraiser_is_donation_type($type->type)) {
    module_load_include('inc', 'fundraiser', 'includes/fundraiser.fields');
    _fundraiser_add_default_fields($type->type);
  }
}

/**
 * Implements hook_node_type_update().
 *
 * Update Fundraiser-related fields when a new node type is created.
 */
function fundraiser_node_type_update($type) {
  if (fundraiser_is_donation_type($type->type)) {
    module_load_include('inc', 'fundraiser', 'includes/fundraiser.fields');
    _fundraiser_add_default_fields($type->type);
  }
}

/**
 * Node API Functions. Handle for each case of node operation.
 */

/**
 * Implements hook_node_delete().
 */
function fundraiser_node_delete($node) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    _fundraiser_delete_fundraiser($node->nid);
    _fundraiser_delete_tracking($node->nid);
  }
}

/**
 * Implements hook_node_insert().
 */
function fundraiser_node_insert($node) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    _fundraiser_create_fundraiser($node);
  }
}

/**
 * Implements hook_node_load().
 */
function fundraiser_node_load($nodes, $types) {
  foreach ($nodes as $node) {
    // If this isn't a fundraiser type, ignore it.
    if (fundraiser_is_donation_type($node->type)) {
      // Get the fundraiser information.
      // This also loads additional data presented from other modules to FR.
      // AKA fundraiser_webform and fundraiser_profile data.
      $fundraiser = _fundraiser_get_fundraiser_by_nid($node->nid);
      // Merge data from fundraiser with node.
      $fundraiser = (array) $fundraiser;
      foreach ($fundraiser as $key => $value) {
        $nodes[$node->nid]->$key = $value;
      }
    }
  }
}

/**
 * Implements hook_node_presave().
 */
function fundraiser_node_presave($node) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type) && isset($node->amount_wrapper)) {
    // Check the submission to remove the donation amounts marked and
    // (optionally) set the default.
    $node->default_amount = NULL;
    foreach ($node->amount_wrapper['donation_amounts'] as $index => $donation_amount) {
      // If the remove marker is set.
      if (isset($donation_amount['remove']) && $donation_amount['remove'] == 1) {
        unset($node->amount_wrapper['donation_amounts'][$index]);
      }
      unset($node->amount_wrapper['donation_amounts'][$index]['remove']);
      // Or if the values aren't set.
      if (empty($donation_amount['label']) && empty($donation_amount['amount'])) {
        unset($node->amount_wrapper['donation_amounts'][$index]);
      }
      unset($node->amount_wrapper['donation_amounts'][$index]['remove']);
      // If the default amount box is checked.
      if (!empty($donation_amount['default_amount'])) {
        $node->default_amount = $donation_amount['amount'];
      }
    }
    // Set the donation amounts where we expect them to be for processing.
    // These are buried under amount_wrapper because we set 'TREE' on that in
    // the form.
    $node->donation_amounts = $node->amount_wrapper['donation_amounts'];
    $node->show_other_amount = $node->amount_wrapper['show_other_amount'];
    $node->minimum_donation_amount = is_numeric($node->amount_wrapper['minimum_donation_amount']) ?
      $node->amount_wrapper['minimum_donation_amount'] : variable_get('fundraiser_default_minimum', 10.00);
    unset($node->amount_wrapper);
  }
}

/**
 * Implements hook_node_update().
 */
function fundraiser_node_update($node) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    _fundraiser_update_fundraiser($node);
  }
}

/**
 * Implements hook_node_validate().
 */
function fundraiser_node_validate($node, $form, &$form_state) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    // If each given amount if numeric, we're ok.
    // Since the form is set to TREE at this point, we need to account for that
    // in our addressing path.
    foreach ($node->amount_wrapper['donation_amounts'] as $index => $donation_amount) {
      if (isset($donation_amount['amount']) && !empty($donation_amount['amount'])) {
        if (!is_numeric($donation_amount['amount'])) {
          form_set_error('donation_amounts', t('Ask amounts must be numeric, you entered "@s".', array("@s" => $donation_amount['amount'])));
          break;
        }
      }
    }
  }
}

/**
 * Implements hook_node_view().
 */
function fundraiser_node_view($node, $view_mode, $langcode) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($node->type)) {
    // Track the pageview.
    if ($view_mode == 'full') {
      _fundraiser_update_tracking_value($node->nid, 'pageviews');
    }

    // Messaging checks. We want to hide warning/admin messages if user can't
    // edit the current node.
    if ($view_mode == 'full' && node_access("update", $node)) {
      // Set a reminder to turn off development mode on the page view.
      if (variable_get('fundraiser_development_mode', 0)) {
        $link = l(t('turn off'), 'admin/config/system/fundraiser');
        drupal_set_message(t('Fundraiser is currently running in development mode. Remember to !link this feature on production websites.',
           array('!link' => $link)), 'warning');
      }
      // Throw a warning if no confirmation email has been configured.
      if (module_exists('email_wrappers') && !email_wrappers_load_settings($node->nid, NULL)) {
        drupal_set_message(t('No confirmation emails have been configured for this donation form. Users who submit a donation will not receive an email confirming their donation.'), 'warning');
      }

      // Allow other modules to throw up warnings on the donation form
      // when it is not configured correctly.
      $messages = module_invoke_all("fundraiser_donation_form_config_check", $node);

      if (!empty($messages)) {
        foreach ($messages as $message) {
          drupal_set_message($message, 'error');
        }
      }
    }

  }
  // NOTE: Glue display modules are responsible for using hook_node_view to
  // display themselves with the node.
  // Some, like webform based glue modules, already do the work of displaying on
  // a form themselves.
}
