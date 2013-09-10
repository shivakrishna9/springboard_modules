<?php

function springboard_services_key_save() {

}

function springboard_services_key_load() {

}

function springboard_services_key_delete() {

}

function springboard_services_get_donation_forms($nids = array(), $pagesize = FALSE, $page = FALSE) {
   $query = db_select('fundraiser', 'f');
   $query->join('node', 'n', 'f.nid = n.nid');
   $query->fields('f', array('nid'));
   $query->fields('n', array('title'));
   if ($pagesize) {
     $query->range($page, $pagesize);
   }
   $results = $query->execute();

   return $results->fetchAssoc();
}