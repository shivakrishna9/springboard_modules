<?php
/**
 * @file
 * Node related hooks for the Fundraiser module.
 */

/**
 * Implements hook_node_access().
 */
function fundraiser_node_access($node, $op, $account) {
  // Without an idea who the owner is or type is we can't assess access.
  if (!empty($node->type) && !empty($node->uid) && fundraiser_is_donation_type($node->type)) {
    $is_author = $account->uid == $node->uid;

    if ($op == 'create') {
      return user_access('create donation form', $account) ? NODE_ACCESS_ALLOW : NODE_ACCESS_IGNORE;
    }
    if ($op == 'update') {
      return ((user_access('edit own donation form', $account) && $is_author) || user_access('edit any donation form', $account)) ? NODE_ACCESS_ALLOW : NODE_ACCESS_IGNORE;
    }
    if ($op == 'delete') {
      return ((user_access('delete own donation form', $account) && $is_author) || user_access('delete any donation form', $account)) ? NODE_ACCESS_ALLOW : NODE_ACCESS_IGNORE;
    }
  }

  return NODE_ACCESS_IGNORE;
}

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
    if (!fundraiser_is_donation_type($node->type)) {
      continue;
    }

    // Get the fundraiser information, including additional data presented from
    // other modules, such as fundraiser_webform and fundraiser_profile data.
    $fundraiser = _fundraiser_get_fundraiser_by_nid($node->nid);

    // Merge data from fundraiser with node.
    $fundraiser = (array) $fundraiser;
    foreach ($fundraiser as $key => $value) {
      $nodes[$node->nid]->$key = $value;
    }
  }
}

/**
 * Implements hook_node_presave().
 */
function fundraiser_node_presave($node) {
  // If this isn't a fundraiser type, or if the node object doesn't have the
  // amount_wrapper property, ignore it.
  if (!fundraiser_is_donation_type($node->type) || empty($node->amount_wrapper)) {
    return;
  }

  // Check the submission to remove the donation amounts marked and
  // (optionally) set the default.
  $node->default_amount = NULL;
  foreach ($node->amount_wrapper['donation_amounts'] as $index => $donation_amount) {
    // If the remove marker is set.
    if (!empty($donation_amount['remove']) && $donation_amount['remove'] == 1) {
      unset($node->amount_wrapper['donation_amounts'][$index]);
    }
    else {
      unset($node->amount_wrapper['donation_amounts'][$index]['remove']);
    }

    // Or if the values aren't set.
    if (empty($donation_amount['label']) && empty($donation_amount['amount'])) {
      unset($node->amount_wrapper['donation_amounts'][$index]);
    }
    else {
      unset($node->amount_wrapper['donation_amounts'][$index]['remove']);
    }

    // If the default amount box is checked.
    if (!empty($donation_amount['default_amount'])) {
      $node->default_amount = $donation_amount['amount'];
    }
  }

  // Set the donation amounts where we expect them to be for processing.
  // These are buried under amount_wrapper because we set 'TREE' on that in the
  // form.
  $node->donation_amounts = $node->amount_wrapper['donation_amounts'];
  $node->show_other_amount = $node->amount_wrapper['show_other_amount'];
  $node->minimum_donation_amount = variable_get('fundraiser_default_minimum', 10.00);
  if (is_numeric($node->amount_wrapper['minimum_donation_amount'])) {
    $node->amount_wrapper['minimum_donation_amount'];
  }

  unset($node->amount_wrapper);
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
    // Since the form is set to TREE at this point, we need to account for that in our addressing path.
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
        $link = l('turn off', 'admin/config/system/fundraiser');
        drupal_set_message(t('Fundraiser is currently running in development mode. Remember to ' .
          '!link this feature on production websites.',
           array('!link' => $link)), 'warning');
      }
      // Throw a warning if no confirmation email has been configured.
      if (module_exists('email_wrappers') && !email_wrappers_load_settings($node->nid, NULL)) {
        drupal_set_message(t('No confirmation emails have been configured for this donation form.' .
          ' Users who submit a donation will not receive an email confirming their donation.'), 'warning');
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
}

/**
 * Implements hook_form_BASE_FORM_ID_alter().
 */
function fundraiser_form_node_form_alter(&$form, &$form_state, $form_id) {
  // If this isn't a fundraiser type, ignore it.
  if (fundraiser_is_donation_type($form['#node']->type)) {
    _fundraiser_donation_settings_form($form, $form_state);
    // Other modules that need to add to the settings form should also implement
    // hook_form_BASE_FORM_ID_alter, or any of the other form alter hooks.
  }
}
