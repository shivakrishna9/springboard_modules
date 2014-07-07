<?php
/**
 * @file
 * Provides some starter content for Springboard Peer to Peer.
 */

/**
 * Form callback for creating starter content.
 */
function springboard_p2p_starter_form($form, $form_state) {
  $form = array();

  $form['create_content'] = array(
    '#type' => 'submit',
    '#value' => t('Create content'),
    '#description' => t('Creates starter content, including a landing page, categories, campaigns, and personal campaigns.'),
  );

  return $form;
}

/**
 * Form submit.
 */
function springboard_p2p_starter_form_submit($form, &$form_state) {
  $nid = springboard_p2p_create_starter_content();

  drupal_set_message('New P2P content has been created.');

  // Redirect to the newly created landing page.
  $form_state['redirect'] = 'node/' . $nid;
}

/**
 * Main function for creating all nessessary P2P content.
 *
 * P2P Categories
 * P2P Campaigns
 * P2P Personal Campaigns
 * P2P Landing Pages
 *
 * @return int
 *   Node id of the newly created landing page.
 *
 * @todo We should batch this.
 */
function springboard_p2p_create_starter_content() {
  $category_ids = springboard_p2p_create_starter_categories();
  $campaign_ids = springboard_p2p_create_starter_campaigns($category_ids);

  $personal_campaign_ids = springboard_p2p_create_starter_personal_campaigns($campaign_ids);

  return springboard_p2p_create_starter_campaign_landing($campaign_ids, $personal_campaign_ids);
}

/**
 * Creates a default campaign landing page node.
 */
function springboard_p2p_create_starter_campaign_landing($campaign_ids, $personal_campaign_ids) {
  $node = new stdClass();
  $node->type = 'p2p_campaign_landing';
  node_object_prepare($node);

  $node->title = '';
  $node->language = LANGUAGE_NONE;

  $node->field_p2p_side_block = array();

  $references = array(
    // Campaigns.
    'field_p2p_slide_show' => $campaign_ids,
    // Personal campaigns.
    'field_p2p_feature_pers_campaigns' => $personal_campaign_ids,
    // Campaigns.
    'field_p2p_referenced_campaigns' => $campaign_ids,
  );

  foreach ($references as $field_name => $nids) {
    $node->$field_name = springboard_p2p_create_starter_entity_reference_field($node->language, $nids);
  }

  $node->field_p2p_featured_title = array(
    $node->language => array(
      0 => array(
        'value' => 'Featured campaigns',
        'format' => NULL,
        'safe_value' => 'Featured campaigns',
      ),
    ),
  );

  $node->field_p2p_choose_featured_type = array(
    $node->language => array(
      0 => array(
        'value' => 'automated',
      ),
    ),
  );

  $node->field_p2p_grid_title = array(
    $node->language => array(
      0 => array(
        'value' => 'Featured causes',
        'format' => NULL,
        'safe_value' => 'Featured causes',
      ),
    ),
  );

  $node->field_p2p_featured_choose_blocks = array();

  node_save($node);

  return $node->nid;
}

/**
 * Create an entity reference field value.
 *
 * @param string $language
 *   The language to use for the field key.
 * @param array $nids
 *   And array of node IDs to reference.
 *
 * @return array
 *   An array ready fto be used as an entity reference field.
 */
function springboard_p2p_create_starter_entity_reference_field($language, $nids) {

  $items = array();
  foreach ($nids as $nid) {
    $items[] = array('target_id' => $nid);
  }

  return array(
    $language => $items,
  );

}

/**
 * Creates personal campaigns.
 *
 * This randomly assigns them to the passed in campaigns.
 *
 * @param array $campaign_ids
 *   The campaigns to assign the personal campaigns to.
 */
function springboard_p2p_create_starter_personal_campaigns($campaign_ids) {
  $personal_campaigns = springboard_p2p_starter_personal_campaigns();
  $personal_campaign_ids = array();

  $deadline = date('Y-m-d', strtotime('+1 month'));

  foreach ($personal_campaigns as $campaign => $personal_campaign) {
    foreach ($personal_campaign as $personal_campaign_title) {
      $node = new stdClass();
      $node->type = 'p2p_personal_campaign';
      node_object_prepare($node);
      $node->title = $personal_campaign_title;
      $node->language = LANGUAGE_NONE;
      $node->field_p2p_campaign[$node->language][0]['target_id'] = $campaign_ids[$campaign];
      $node->body[$node->language][0]['value'] = springboard_p2p_create_starter_greeking(150);
      $node->body[$node->language][0]['format'] = 'filtered_html';
      $node->field_p2p_personal_campaign_goal[$node->language][0]['value'] = rand(500, 1000);

      // The deadline field is required.
      $node->field_p2p_fundraising_deadline = array(
        $node->language => array(
          0 => array(
            'value' => $deadline,
            'timezone' => 'UTC',
            'timezone_db' => 'UTC',
            'date_type' => 'datetime',
          ),
        ),
      );

      $node->field_p2p_campaign_progress = array(
        $node->language => array(
          0 => array(
            'amount' => '0',
            'submissions' => '0',
          ),
        ),
      );

      node_save($node);
      $personal_campaign_ids[$personal_campaign_title] = $node->nid;
    }
  }

  return $personal_campaign_ids;
}

/**
 * Creates peer to peer campaigns.
 *
 * @param array $category_ids
 *   The categories to assign the campaigns to.
 */
function springboard_p2p_create_starter_campaigns($category_ids) {
  $campaigns = springboard_p2p_starter_campaigns();
  $campaign_ids = array();

  $campaign_thumbnail = springboard_p2p_create_starter_file('/images/starter/campaign_thumbnail.png');
  $campaign_landing_page_slider = springboard_p2p_create_starter_file('/images/starter/campaign_landing_page_slider.png');
  $campaign_banner = springboard_p2p_create_starter_file('/images/starter/personal_campaign_banner.png');

  foreach ($campaigns as $category => $campaign) {
    foreach ($campaign as $this_campaign) {
      $node = new stdClass();
      $node->type = 'p2p_campaign';
      node_object_prepare($node);
      $node->title = $this_campaign;
      $node->language = LANGUAGE_NONE;
      $node->field_p2p_category[$node->language][0]['target_id'] = $category_ids[$category];
      $node->body[$node->language][0]['value'] = springboard_p2p_create_starter_greeking(200);
      $node->body[$node->language][0]['format'] = 'filtered_html';
      $node->field_p2p_allow_donor_comments[$node->language][0]['value'] = 1;
      $node->field_p2p_show_donor_comments[$node->language][0]['value'] = 1;
      $node->field_p2p_show_donor_scroll[$node->language][0]['value'] = 1;

      $node->field_p2p_campaigns_approval[$node->language][0]['value'] = 0;
      $node->field_p2p_org_intro[$node->language][0]['value'] = springboard_p2p_create_starter_greeking(100);
      $node->field_p2p_personal_intro[$node->language][0]['value'] = springboard_p2p_create_starter_greeking(50);

      // @todo Figure out how to deal with the video embed field.
      $node->field_p2p_video_embed = array(
        $node->language => array(
          0 => array(
            'video_url' => 'http://www.youtube.com/watch?v=OVICu9-FfNU',
          ),
        ),
      );

      // Image fields.
      $node->field_p2p_campaign_thumbnail[$node->language][0] = (array) $campaign_thumbnail;
      $node->field_p2p_campaign_slider[$node->language][0] = (array) $campaign_landing_page_slider;
      $node->field_p2p_campaign_banner[$node->language][0] = (array) $campaign_banner;

      // Create a goal.
      $next_id = db_query("SELECT MAX(goal_set_id) FROM {springboard_p2p_fields_campaign_goals}")->fetchField();
      $goal_set_id = ++$next_id;
      $goal = array(
        'goal_set_id' => $goal_set_id,
        'type' => 'donation_form',
        'enabled' => 1,
        'nid' => 2,
        'campaign_id' => '',
        'goal_type' => 'amount',
        'goal_value' => rand(500, 1000),
      );
      drupal_write_record('springboard_p2p_fields_campaign_goals', $goal);

      $node->field_p2p_campaign_goals[$node->language][0]['campaign_goals'] = array(
        'goal_set_id' => $goal_set_id,
        'form_types' => array(
          'fundraiser' => array(
            'donation' => array(
              'enabled' => $goal['enabled'],
              'nid' => $goal['nid'],
              'campaign_id' => '',
              'goal_type' => $goal['goal_type'],
              'goal_value' => $goal['goal_value'],
            ),
          ),
        ),
      );

      node_save($node);
      $campaign_ids[$this_campaign] = $node->nid;
    }
  }

  return $campaign_ids;
}

/**
 * Creates peer to peer categories.
 */
function springboard_p2p_create_starter_categories() {
  $categories = springboard_p2p_starter_categories();
  $category_ids = array();
  foreach ($categories as $category) {
    $node = new stdClass();
    $node->type = 'p2p_category';
    node_object_prepare($node);
    $node->title = $category;
    $node->language = LANGUAGE_NONE;
    $node->body[$node->language][0]['value'] = springboard_p2p_create_starter_greeking(200);
    $node->body[$node->language][0]['format'] = 'filtered_html';
    node_save($node);
    $category_ids[$category] = $node->nid;
  }
  return $category_ids;
}


/**
 * Provides default category names.
 */
function springboard_p2p_starter_categories() {
  return array(
    'Natural Disaster',
    'Humanitarian Aid',
    'Runs and Walks',
    'Human Rights',
    'Animal Rights',
    'Endangered Species',
  );
}

/**
 * Provides default peer to peer campaign names.
 */
function springboard_p2p_starter_campaigns() {
  return array(
    'Endangered Species' => array(
      'Black Rhino',
      'Cross River Gorilla',
      'Leatherback Turtle',
      'Mountain Gorilla',
      'African Wild Dog',
      'Asian Elephant',
      'Bengal Tiger',
      'Black Spider Monkey',
      'Blue Whale',
    ),
  );
}

/**
 * Provides default peer to peer personal campaigns.
 */
function springboard_p2p_starter_personal_campaigns() {
  return array(
    'Black Rhino' => array(
      'Help End Rhino Poaching',
      'Just Say No to Rhino Rodeos',
    ),
    'Mountain Gorilla' => array(
      'Campaign to Protect Gorilla Habitat',
    ),
    'Blue Whale' => array(
      'Tell Tuna Fishers to Take a Swim',
      'No to Nets',
      'Help Rid the Oceans of Plastic Bottles',
    ),
  );
}

/**
 * Uploads image files.
 */
function springboard_p2p_create_starter_file($image_path) {
  $path = drupal_get_path('module', 'springboard_p2p');
  $filepath = drupal_realpath($path . $image_path);
  // Create managed File object and associate with Image field.
  $file = (object) array(
    'uid' => 1,
    'uri' => $filepath,
    'filemime' => file_get_mimetype($filepath),
    'status' => 1,
  );

  // We save the file to the root of the files directory.
  $file = file_copy($file, 'public://');
  return $file;
}

/**
 * Stolen from devel generate :).
 */
function springboard_p2p_create_starter_greeking($word_count, $title = FALSE) {
  $dictionary = array("abbas", "abdo", "abico", "abigo", "abluo", "accumsan",
    "acsi", "ad", "adipiscing", "aliquam", "aliquip", "amet", "antehabeo",
    "appellatio", "aptent", "at", "augue", "autem", "bene", "blandit",
    "brevitas", "caecus", "camur", "capto", "causa", "cogo", "comis",
    "commodo", "commoveo", "consectetuer", "consequat", "conventio", "cui",
    "damnum", "decet", "defui", "diam", "dignissim", "distineo", "dolor",
    "dolore", "dolus", "duis", "ea", "eligo", "elit", "enim", "erat",
    "eros", "esca", "esse", "et", "eu", "euismod", "eum", "ex", "exerci",
    "exputo", "facilisi", "facilisis", "fere", "feugiat", "gemino",
    "genitus", "gilvus", "gravis", "haero", "hendrerit", "hos", "huic",
    "humo", "iaceo", "ibidem", "ideo", "ille", "illum", "immitto",
    "importunus", "imputo", "in", "incassum", "inhibeo", "interdico",
    "iriure", "iusto", "iustum", "jugis", "jumentum", "jus", "laoreet",
    "lenis", "letalis", "lobortis", "loquor", "lucidus", "luctus", "ludus",
    "luptatum", "macto", "magna", "mauris", "melior", "metuo", "meus",
    "minim", "modo", "molior", "mos", "natu", "neo", "neque", "nibh",
    "nimis", "nisl", "nobis", "nostrud", "nulla", "nunc", "nutus", "obruo",
    "occuro", "odio", "olim", "oppeto", "os", "pagus", "pala", "paratus",
    "patria", "paulatim", "pecus", "persto", "pertineo", "plaga", "pneum",
    "populus", "praemitto", "praesent", "premo", "probo", "proprius",
    "quadrum", "quae", "qui", "quia", "quibus", "quidem", "quidne", "quis",
    "ratis", "refero", "refoveo", "roto", "rusticus", "saepius",
    "sagaciter", "saluto", "scisco", "secundum", "sed", "si", "similis",
    "singularis", "sino", "sit", "sudo", "suscipere", "suscipit", "tamen",
    "tation", "te", "tego", "tincidunt", "torqueo", "tum", "turpis",
    "typicus", "ulciscor", "ullamcorper", "usitas", "ut", "utinam",
    "utrum", "uxor", "valde", "valetudo", "validus", "vel", "velit",
    "veniam", "venio", "vereor", "vero", "verto", "vicis", "vindico",
    "virtus", "voco", "volutpat", "vulpes", "vulputate", "wisi", "ymo",
    "zelus");
  $dictionary_flipped = array_flip($dictionary);

  $greeking = '';

  if (!$title) {
    $words_remaining = $word_count;
    while ($words_remaining > 0) {
      $sentence_length = mt_rand(3, 10);
      $words = array_rand($dictionary_flipped, $sentence_length);
      $sentence = implode(' ', $words);
      $greeking .= ucfirst($sentence) . '. ';
      $words_remaining -= $sentence_length;
    }
  }
  else {
    // Use slightly different method for titles.
    $words = array_rand($dictionary_flipped, $word_count);
    $words = is_array($words) ? implode(' ', $words) : $words;
    $greeking = ucwords($words);
  }

  // Work around possible php garbage collection bug. Without an unset(), this
  // function gets very expensive over many calls (php 5.2.11).
  unset($dictionary, $dictionary_flipped);
  return trim($greeking);
}
