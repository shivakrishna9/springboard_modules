<?php

class SpringboardP2pFieldsFactory {

  static public function getField($type) {
    switch ($type) {
      case 'springboard_p2p_fields_campaign_goals':
        return new SpringboardP2pCampaignGoalsField();

      case 'springboard_p2p_fields_personal_campaign_progress':
        return new SpringboardP2pPersonalCampaignProgressField();

    }
  }

}
