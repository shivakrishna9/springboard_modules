<?php

/**
 * @file springboard_advocacy.views.inc
 *
 * Define our advocacy views.
 */

/**
 * Implements hook_views_data().
 *
 * Create the views data arrays for each of the
 * fields returned by the Springboard API remote queries.
 *
 */
function springboard_advocacy_views_data() {
  $data = array();

  // Define our pseudo-DB table and assign the
  // custom query handler which queries the remote API server
  $data['advocacy']['table'] = array(
    'group' => t('Advocacy'),
    'base' => array(
      'title' => t('Advocacy'),
      'help' => t(''),
      'query class' => 'springboard_advocacy_views_plugin_query',
    ),
  );

  // Target ID returned by the API request.
  $data['advocacy']['id'] = array(
    'title' => t('Target ID'),
    'help' => t('Target ID'),
    'field' => array(
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // Salutation returned by the API request.
  $data['advocacy']['salutation'] = array(
    'title' => t('Salutation'),
    'help' => t('Target Salutation'),
    'field' => array(
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // First Name returned by the API request.
  $data['advocacy']['first_name'] = array(
    'title' => t('First Name'),
    'help' => t('Target First Name'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
     'filter' => array(
       'handler' => 'springboard_advocacy_views_handler_filter_string',
    ),
  );

  // Last Name returned by the API request.
  $data['advocacy']['last_name'] = array(
    'title' => t('Last Name'),
    'help' => t('Target Last Name'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
     'filter' => array(
       'handler' => 'springboard_advocacy_views_handler_filter_string',
    ),
  );

  // Email returned by the API request.
  $data['advocacy']['email'] = array(
    'title' => t('Email'),
    'help' => t('Target Email'),
    'field' => array(
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // Party returned by the API request.
  $data['advocacy']['party'] = array(
    'title' => t('Political Party'),
    'help' => t('Target Party'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // State returned by the API request.
  $data['advocacy']['state'] = array(
    'title' => t('State'),
    'help' => t('Target State'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  //Role returned by the API request.
  $data['advocacy']['role'] = array(
    'title' => t('Role'),
    'help' => t('Target Role'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // District returned by the API request.
  $data['advocacy']['district_name'] = array(
    'title' => t('District Name'),
    'help' => t('District Name'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // District code returned by the API request.
  $data['advocacy']['district_code'] = array(
    'title' => t('District Code'),
    'help' => t('District Code'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // Party returned by the API request.
  $data['advocacy']['party'] = array(
    'title' => t('Politcal Party'),
    'help' => t('Target Party'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // Organization returned by the API request.
  $data['advocacy']['organization'] = array(
    'title' => t('Organization'),
    'help' => t('Organization'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_string',
    ),
  );

  // Organization title returned by the API request.
  $data['advocacy']['title'] = array(
    'title' => t('Organization title'),
    'help' => t('Organization position or title'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );
  // Committee memberships returned by the API request.
  $data['advocacy']['committee_memberships'] = array(
    'title' => t('Committee Memberships'),
    'help' => t('Committee Memberships'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'springboard_advocacy_views_handler_field',
    ),
  );

  // Create/edit/delete/links
  $data['advocacy']['target_actions'] = array(
    'title' => t('Target Actions'),
    'help' => t('Action links for target displays'),
    'field' => array(
      'handler' => 'springboard_advocacy_views_handler_field_target_actions',
    ),
  );

  $data['advocacy']['group_actions'] = array(
    'title' => t('Group Actions'),
    'help' => t('Action links for group displays'),
    'field' => array(
      'handler' => 'springboard_advocacy_views_handler_field_group_actions',
    ),
  );

  // Combination search fields
  $data['advocacy']['combine'] = array(
    'title' => t('Search Multiple Target Fields by Keywords'),
     'help' => t('Combine multiple fields together and search targets by them.'),
     'filter' => array(
       'handler' => 'springboard_advocacy_views_handler_filter_combine',
     ),
  );

  // Search by class name (Target|Legislator).
  $data['advocacy']['search_class_name'] = array(
   'title' => t('Search By Type'),
    'help' => t('Limit results by Legislator or Custom Target. Please note: if this filter is not "exposed", it will return no results *unless* another filter is enabled and actively being searched.'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_type'
    ),
  );

  // Search by role.
  $data['advocacy']['search_role'] = array(
   'title' => t('Search By Branch'),
    'help' => t('Adds a filter to limit searches by Federal or State Branch'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_branch'
    ),
  );

  // Search by branch
  $data['advocacy']['search_level'] = array(
   'title' => t('Search By Level'),
    'help' => t('Adds a filter to limit searches by State or Federal level'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_level'

    ),
  );

  // Search by party
  $data['advocacy']['search_party'] = array(
   'title' => t('Search By Party'),
    'help' => t('Adds a filter to limit searches by Political Party'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_party'

    ),
  );

  // Search by state.
  $data['advocacy']['search_state'] = array(
   'title' => t('Search By State'),
    'help' => t('Adds a filter to limit searches by State'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_state'

    ),
  );

  // Search by district name
  $data['advocacy']['search_district_name'] = array(
    'title' => t('Search By District'),
    'help' => t('Adds a filter to limit searches by District'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_district'
    ),
  );

  // Search by gender
  $data['advocacy']['search_gender'] = array(
   'title' => t('Search By Gender'),
    'help' => t('Adds a filter to limit searches by Gender'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_gender'
    ),
  );

  // Search by social media presence
  $data['advocacy']['search_social'] = array(
    'title' => t('Search By Social Media Presence'),
    'help' => t('Adds a filter to limit searches by targets who have social media profiles'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_social'
    ),
  );

  // Search by committee name
  $data['advocacy']['search_committee'] = array(
    'title' => t('Search by Committee Name'),
    'help' => t('Committee name search'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_string',
    ),
  );

  // Search by committee chamber
  $data['advocacy']['search_committee_chamber'] = array(
    'title' => t('Search By Committee Chamber'),
    'help' => t('Adds a filter to limit searches by committee chamber'),
    'filter' => array(
      'handler' => 'springboard_advocacy_views_handler_filter_search_types',
      'options callback' => 'springboard_advocacy_target_committee_chamber'
    ),
  );

  return $data;
}

/**
 * Implements hook_views_plugins().
 *
 * Define our custom query, cache and pager plugins.
 */
function springboard_advocacy_views_plugins() {
  $plugins = array(
    // The query plugin replaces DB queries with API queries
    'query' => array(
      'springboard_advocacy_views_plugin_query' => array(
        'title' => t('Springboard Advocacy APIRequest'),
        'help' => t('Uses springboard_advocacyAPIRequest for querying Targets'),
        'handler' => 'springboard_advocacy_views_plugin_query',
        'path' => drupal_get_path('module', 'springboard_advocacy') . '/includes/views/plugins',
      ),
    ),
    // The pager is needed to fix a long-term views bug which causes
    // the form api to freak about illegal choices
    'pager' => array(
      'springboard_advocacy_views_plugin_pager' => array(
        'title' => t('Springboard Advocacy Pager Full'),
        'help' => t('For paging Targets'),
        'handler' => 'springboard_advocacy_views_plugin_pager',
        'path' => drupal_get_path('module', 'springboard_advocacy') . '/includes/views/plugins',
        'uses options' => TRUE,
      ),
    ),

    'cache' => array(
      'springboard_advocacy_views_plugin_cache_time' => array(
        'title' => t('Springboard Advocacy Timed Cache'),
        'help' => t('dgfdg'),
        'handler' => 'springboard_advocacy_views_plugin_cache_time',
        'path' => drupal_get_path('module', 'springboard_advocacy') . '/includes/views/plugins',
        'uses options' => TRUE,
      ),
    ),
  );

  return $plugins;
}

/**
 * Returns default options for views field configuration
 * Here in the main module because might be used by multiple
 * submodules.
 */
function springboard_advocacy_target_type() {
  $value_options = array(
    'Target' => t('Custom Target'),
    'Legislator' => t('Legislator'),
    'group' => t('Group')
  );
  return $value_options;
}

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_branch() {
  $value_options = array(
    'SR|SS' => t('Both Chambers, State'),
    'FR|FS' => t('Both Chambers, Federal'),
    'FS' => t('Federal Senate'),
    'FR' => t('Federal House'),
    'SR' => t('State House'),
    'SS' => t('State Senate'),
    'SS' => t('State Senate'),
    'PRES01|PRES03|GOVNR' => t('State and Federal Executives'),
    'PRES01|PRES03' => t('Federal Executives'),
    'GOVNR' => t('State Executives'),
  );
  return $value_options;
}

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_level() {
  $value_options = array(
    'federal' => t('Federal'),
    'state' => t('State'),
  );
  return $value_options;
}

/***** Helper functions defining value options for the various fields defined above ******/

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_party() {
  $value_options = array(
    'D' => t('Democrat'),
    'R' => t('Republican'),
    'I' => t('Independent'),
    'O' => t('Other'),
  );
  return $value_options;
}

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_committee_chamber() {
  $value_options = array(
    'FH' => t('Federal House'),
    'FS' => t('Federal Senate'),
    'FJ' => t('Federal Joint'),
    'SH' => t('State House'),
    'SS' => t('State Senate'),
    'SJ' => t('State Joint'),

  );
  return $value_options;
}

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_social() {
  $value_options = array(
    'facebook' => t('Has Facebook Profile'),
    'twitter' => t('Has Twitter Profile'),
  );
  return $value_options;
}

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_state() {
  $value_options = array(
    'AL' => 'Alabama',
    'AK' => 'Alaska',
    'AZ' => 'Arizona',
    'AR' => 'Arkansas',
    'CA' => 'California',
    'CO' => 'Colorado',
    'CT' => 'Connecticut',
    'DE' => 'Delaware',
    'DC' => 'District Of Columbia',
    'FL' => 'Florida',
    'GA' => 'Georgia',
    'HI' => 'Hawaii',
    'ID' => 'Idaho',
    'IL' => 'Illinois',
    'IN' => 'Indiana',
    'IA' => 'Iowa',
    'KS' => 'Kansas',
    'KY' => 'Kentucky',
    'LA' => 'Louisiana',
    'ME' => 'Maine',
    'MD' => 'Maryland',
    'MA' => 'Massachusetts',
    'MI' => 'Michigan',
    'MN' => 'Minnesota',
    'MS' => 'Mississippi',
    'MO' => 'Missouri',
    'MT' => 'Montana',
    'NE' => 'Nebraska',
    'NV' => 'Nevada',
    'NH' => 'New Hampshire',
    'NJ' => 'New Jersey',
    'NM' => 'New Mexico',
    'NY' => 'New York',
    'NC' => 'North Carolina',
    'ND' => 'North Dakota',
    'OH' => 'Ohio',
    'OK' => 'Oklahoma',
    'OR' => 'Oregon',
    'PA' => 'Pennsylvania',
    'RI' => 'Rhode Island',
    'SC' => 'South Carolina',
    'SD' => 'South Dakota',
    'TN' => 'Tennessee',
    'TX' => 'Texas',
    'UT' => 'Utah',
    'VT' => 'Vermont',
    'VA' => 'Virginia',
    'WA' => 'Washington',
    'WV' => 'West Virginia',
    'WI' => 'Wisconsin',
    'WY' => 'Wyoming',
  );
  return $value_options;
}

/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_district() {
  //values are populated via ajax call in form alter
  $value_options = array();
  return $value_options;
}


/**
 * Returns default options for views field configuration
 */
function springboard_advocacy_target_gender() {
  $value_options = array(
    'M' => 'Male',
    'F' => 'Female',
    'O' => 'Other'

  );
  return $value_options;
}

/********** Views Hooks *****************/

/**
 * Implements hook_views_pre_render
 *
 * Takes the target search views query and prepares it as string for
 * inclusion in the search form add-all link query string.
 *
 * @param $view
 */
function springboard_advocacy_views_pre_render(&$view) {
  if ($view->name == 'targets' && $view->current_display == 'block_3' && !empty($view->build_info['springboard_advocacy_search'])) {
    $search_terms = $view->build_info['springboard_advocacy_search'];
    $total_rows = $view->total_rows;
    $query = array();
    foreach ($search_terms as $field => $terms) {
      if (is_array($terms)) {
        $query[$field] = implode('|', $terms);
      }
      else {
        if($field != 'search_committee') {
          $terms =str_replace(' ', '|',   $terms);
        }
        $query[$field] = $terms;
      }
    }
    $view->attachment_before = theme('springboard_advocacy_views_attachment_before', array('view' => $view, 'total_rows' => $total_rows, 'query' => $query));
  }
}

/**
 *  Creates the "add-all link on message target search results
 * @param $vars
 * @return string
 */
function theme_springboard_advocacy_views_attachment_before($vars) {
  if (!empty($vars['query'])) {

    if (!empty($vars['view']->build_info['target_ids'])) {
      $ids_required =  array('fields', 'gender', 'district_name', 'gender', 'social', 'search_committee');
      $ids_required = array_intersect(array_keys($vars['query']), $ids_required);

      if(!empty($ids_required)) {
        $vars['query']['ids'] = implode('|', $vars['view']->build_info['target_ids']);
      }
    }
    if (!empty($vars['total_rows'])) {
      $plural = $vars['total_rows'] > 1 ? 'matches found' : 'match found';
      $output = '<div id ="advocacy-attachment-before-rows"><span class = "target-results">Search Results </span><span class = "target-count">' . $vars['total_rows'] . ' ' . $plural . '</span></div>';
      if (user_access('add target to action')) {
        $output .= '<div id ="advocacy-attachment-before-add-all">';
        $output .= l(t('Add All Targets'), "add-all", array('attributes' => array('id' => 'advo-add-all'), 'query' => $vars['query'], 'external' => TRUE));
        $output .= '</div>';
      }
      return $output;
    }
  }
}

/**
 * @todo remove this if fixed in future views release.
 * Override a bug in the original preprocess
 * See https://www.drupal.org/node/2315365 for a similar issue.
 */
function springboard_advocacy_preprocess_views_view_table(&$vars)
{
  $view = $vars['view'];
  $options  = $view->style_plugin->options;
  $handler  = $view->style_plugin;
  $fields = &$view->field;
  $columns = $handler->sanitize_columns($options['columns'], $fields);

  $active = !empty($handler->active) ? $handler->active : '';
  $order = !empty($handler->order) ? $handler->order : 'asc';

  $query = tablesort_get_query_parameters();

  // bug override
  // $query += $view->exposed_raw_input
  // broke sort link queries
  $exposed = $view->get_exposed_input();
  if (!empty($exposed)) {
    $query += $exposed;
  }

  foreach ($columns as $field => $column) {
    // render the header labels

    if ($field == $column && empty($fields[$field]->options['exclude'])) {
      $label = check_plain(!empty($fields[$field]) ? $fields[$field]->label() : '');
      if (!empty($options['info'][$field]['sortable']) || $fields[$field]->click_sortable()) {
        // $vars['header'][$field] = $label;
        $initial = !empty($options['info'][$field]['default_sort_order']) ? $options['info'][$field]['default_sort_order'] : 'asc';

        if ($active == $field) {
          $initial = ($order == 'asc') ? 'desc' : 'asc';
        }

        $title = t('sort by @s', array('@s' => $label));
        if ($active == $field) {
          $label .= theme('tablesort_indicator', array('style' => $initial));
        }

        $query['order'] = $field;
        $query['sort'] = $initial;
        $link_options = array(
          'html' => TRUE,
          'attributes' => array('title' => $title),
          'query' => $query,
        );

        $vars['header'][$field] = l($label, $_GET['q'], $link_options);
      }
    }
    if (isset($vars['rows'][0]['committee_memberships']) && $field == 'committee_memberships' && $label ==  '') {
      $label = t('Committees');
      $vars['header'][$field] = t($label);
    }
  }
}
