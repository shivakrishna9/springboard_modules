<?php

/**
 * A simple base class that contains things common to all
 * FdMigrateExample classes.
 */
abstract class FdMigrateMigration extends Migration {
  public function __construct($arguments = array()) {
    // Always call the parent constructor first for basic setup
    parent::__construct($arguments);
  }

  /**
   * Provide the names of the incoming CSV file columns.
   */
  function csvcolumns() {
    $columns[0] = array('SKU', 'SKU');
    $columns[1] = array('Title', 'Title');
    $columns[2] = array('Category', 'Category');
    $columns[3] = array('Price', 'Price');


    return $columns;
  }
}

/**
 * Import Products into the "product" product type.
 */
class FdMigrateProductMigration extends FdMigrateMigration {
  public function __construct($arguments = array()) {
    parent::__construct($arguments);
    $this->description = t('Import products from CSV file (with no header).');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        // The 'SKU' field in the CSV file is the key.
        'SKU' => array('type' => 'varchar',
                       'length' => 24,
                       'not null' => TRUE,
                       'description' => 'SKU',
                      ),
      ),


      MigrateDestinationEntityAPI::getKeySchema('commerce_product', 'fundraiser_designation')

    );
    // Create a MigrateSource object, which manages retrieving the input data.
    $csv_file = drupal_get_path('module', 'fd_migrate') . '/fd_migrate.csv';
    $this->source = new MigrateSourceCSV($csv_file, $this->csvcolumns(), array('header_rows' => 1));

    // Again here we're hardwiring the product type to 'product'.
    $this->destination = new MigrateDestinationEntityAPI('commerce_product', 'fundraiser_designation');

    $this->addFieldMapping('sku', 'SKU');
    $this->addFieldMapping('title', 'Title');
    $this->addFieldMapping('field_fd_designation_categories', 'Category');
    $this->addFieldMapping('field_fd_designation_categories:create_term')
     ->defaultValue(TRUE);
    $this->addFieldMapping('commerce_price', 'Price')->defaultValue('1');
    $this->addFieldMapping('commerce_price:currency_code')->defaultValue('USD');


  }
}

