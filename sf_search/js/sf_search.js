// global variable to store all sf_search related items
var sfSearch = {};
var contactTable;
sfSearch.contacts = []; // initialize empty contacts array

Drupal.behaviors.SfSearchSetup = function(context) {
  $('#edit-sf-search-search-form-submit').click(function() {
    var query = $('#edit-sf-search-query').val();
    var nid = $('#edit-nid').val();
    var fid = $('#edit-fid').val();
    if (query != '') {
      // hide any current search results
      $('#sf-search-panel').slideUp('fast');
      if ($('#edit-sf-search-type-lookup').is(':checked')) { 
        contactLookup(query, nid, fid);
      }
      else if ($('#edit-sf-search-type-search').is(':checked')) { 
        contactSearch(query, nid, fid);
      }
    }
    return false;
  });

  /* throbbers */
  $("#edit-sf-search-query").bind("ajaxStart", function(){
    $(this).addClass('sf-search-searching');
  });

  $("#edit-sf-search-query").bind("ajaxStop", function(){
    $(this).removeClass('sf-search-searching');
  });
};

/**
 * Extract the contact when it's id is clicked.
 */
$("#sf-contacts tbody tr").live('click', function() {
  var contact = $('#sf-contacts').dataTable().fnGetData(this);
  $('#sf-search-panel').slideUp('fast');
  populateContactInfo(contact);
});

/**
 * Close the contact when no selection is clicked.
 */
$("#sf-contacts_close").live('click', function() {
  $('#sf-search-panel').slideUp('fast');
  $('#sf-search-no-results').html('<em>No donor selected.</em>').show().fadeOut(4000);
});

/**
 * Use the display columns setting to build a data structure that data tables
 * can use to display the contacts.
 */
function getColumnDefinitions() {
  var columns = [];
  for (var propName in Drupal.settings.sf_search.display_columns) {
    var column = {};
    column.sTitle = Drupal.settings.sf_search.display_columns[propName];
    column.mDataProp = propName;
    columns.push(column);
  }
  return columns;
}

/**
 * Displays the results of a search query.
 */
function displayResults(data) {
  if (data.length == 1) {
    // single contact, go ahead and populate form
    populateContactInfo(data[0]);
  }
  else if (data.length > 1) {
    // save data for later
    sfSearch.contacts = data;
    // get a data tables friendly version of the column data
    columns = getColumnDefinitions();
    // create a data table on the fly filled with the returned contacts
    contactTable = $('#sf-contacts').dataTable( {
      "aaData": sfSearch.contacts,
      "aoColumns": columns,
      "bFilter": false,
      "bDestroy": true
    });
    // Add close button to search panel at top.
    $('#sf-search-panel').prepend('<div id="sf-contacts_close"><span id="sf-close_text">Close Search</span></div>');    
    $('#sf-search-panel').slideDown('slow');
  }
  else {
    $('#sf-search-no-results').html('<em>No contacts found</em>').show().fadeOut(4000);
  }
}

/**
 * Performs a contact lookup by donor id.
 */
function contactLookup(donor_id, nid, fid) {
  var url = '/js/salesforce-search/lookup/' + nid + '/' + donor_id;
  $.getJSON(url, function(data) {
    displayResults(data);
  });
}

/**
 * Performs a full-text contact search.
 */
function contactSearch(query, nid, fid) {
  var url = '/js/salesforce-search/find/' + nid + '/' + query;
  $.getJSON(url, function(data) {
    displayResults(data);
  });
}

/**
 * Takes a contact object and uses a client-side fieldmap to populate a form.
 */
function populateContactInfo(contact) {
  var fieldMap = Drupal.settings.sf_search.field_map;
  for (var field in fieldMap) {
    var inputId = '#' + fieldMap[field];
    // there has to be a better solution for these state/country conversions
    if (field == 'MailingState') {
      contact[field] = Drupal.settings.sf_search.zones[contact[field]];
    }   
    if (field == 'MailingCountry') {
      contact[field] = Drupal.settings.sf_search.countries[contact[field]];
    }
    if (typeof contact[field] != "undefined") {
      $(inputId).val(contact[field]).effect('highlight', {}, 2000);
    }
  }
  $('#sf-search-no-results').html('<em>Donor information has been copied to the from below.</em>').show().fadeOut(4000);
}