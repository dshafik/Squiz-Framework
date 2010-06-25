<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'DAL/Tests/ParseSchemaUnitTest.php';

/**
 * MySQL schema tests.
 *
 * Unit test for testing that the SQL produced for MySQL is correct when
 * parsing the schema.xml.
 *
 * @since 4.0.0
 */
class ParseSchemaMySQLUnitTest extends ParseSchemaUnitTest
{


    /**
     * Test SQL for MySQL schema.
     *
     * Tests to verify that the SQL produced from parsing a test schema is what
     * is expected for the MySQL database.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMySqlSchemaParse()
    {
        if ($this->skipOnDbType('mysql') === TRUE) {
            return;
        }

        $result = $this->getSqlForTestSchema();
        PHPUnit_Framework_Assert::assertEquals($this->_getSql(), $result);

    }//end testMySqlSchemaParse()


    /**
     * Returns the SQL expected for MySQL.
     *
     * We have to do this in a method because member vars don't allow concatination
     * of strings! arrg.
     *
     * @since  4.0.0
     * @return string
     */
    private function _getSql()
    {
        $sql  = 'CREATE TABLE asset_type ('."\n";
        $sql .= 'type_code VARCHAR(100),'."\n";
        $sql .= 'version VARCHAR(20),'."\n";
        $sql .= 'name VARCHAR(100),'."\n";
        $sql .= 'description VARCHAR(255));'."\n";
        $sql .= 'ALTER TABLE asset_type'."\n";
        $sql .= 'ADD CONSTRAINT asset_type_pk PRIMARY KEY (type_code);'."\n";
        $sql .= 'ALTER TABLE asset_type'."\n";
        $sql .= 'ADD CONSTRAINT asset_type_fk1 FOREIGN KEY (type_code) REFERENCES asset_type(type_code) ON DELETE CASCADE;'."\n\n";
        $sql .= 'CREATE INDEX asset_type_type_code ON asset_type (type_code);'."\n";
        $sql .= 'CREATE INDEX asset_type_lvl ON asset_type (version);'."\n";

        return $sql;

    }//end _getSql()


}//end class

?>
