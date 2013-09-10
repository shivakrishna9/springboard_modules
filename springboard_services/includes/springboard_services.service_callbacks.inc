<?php

/**
 * @file service implementation callbacks
 */

function springboard_services_donation_form_index($page, $pagesize) {
    return springboard_services_get_donation_forms(array(), $pagesize, $page);
}
