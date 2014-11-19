<?php
/**
 * @file
 * The Views controller class for the Fundraiser Sustainer record entity.
 */

class FundraiserSustainerRecordViewsController extends EntityDefaultViewsController {

  /**
   * Defines the result for hook_views_data().
   *
   * For the Fundraiser Sustainer record entity. This allows filtering on
   * gateway response using "is null" and "is not null".
   *
   * This also adds a join on fundraiser_donation, which I copied from the
   * original Springboard Views implementation of hook_views_data for
   * the sustainers table.
   *
   * @return array
   *   Data for Views.
   */
  public function views_data() {
    $data = parent::views_data();
    $table = $this->info['base table'];

    // Adds support for filtering on null values.
    $data[$table]['gateway_resp']['filter']['allow empty'] = TRUE;

    // Adds a join against fundraiser_donation.
    $data[$table]['table']['join']['fundraiser_donation'] = array(
      'left_field' => 'did',
      'field' => 'did',
    );

    return $data;
  }
}
