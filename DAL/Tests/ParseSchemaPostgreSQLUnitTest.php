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
 * Tests for the parsing of Postgres schemas.
 *
 * Unit test for testing that the sql produced for PostgreSQL is correct when
 * parsing the schema.xml.
 *
 * @since 4.0.0
 */
class ParseSchemaPostgreSQLUnitTest extends ParseSchemaUnitTest
{


    /**
     * Test for Postgres schema parsing.
     *
     * Tests to verify that the SQL produced from parsing a test schema is what
     * is expected for the PostgreSQL database.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPostgreSQLSchemaParse()
    {
        if ($this->skipOnDbType('pgsql') === TRUE) {
            return;
        }

        $result = $this->getSqlForTestSchema();
        PHPUnit_Framework_Assert::assertEquals($this->_getSql(), $result);

    }//end testPostgreSQLSchemaParse()


    /**
     * Returns the SQL expected for PostgreSQL.
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
        $sql .= 'description VARCHAR(255),'."\n";
        $sql .= 'CONSTRAINT asset_type_pk PRIMARY KEY (type_code),'."\n";
        $sql .= 'CONSTRAINT asset_type_fk1 FOREIGN KEY (type_code) REFERENCES asset_type(type_code) ON DELETE CASCADE);'."\n";
        $sql .= 'CREATE INDEX asset_type_type_code ON asset_type (type_code);'."\n";
        $sql .= 'CREATE INDEX asset_type_lvl ON asset_type (version);'."\n";

        return $sql;

    }//end _getSql()


}//end class

?>
