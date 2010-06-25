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
 * Tests for the parsing of Oracle Schemas.
 *
 * Unit test for testing that the sql produced for Oracle is correct when
 * parsing the schema.xml.
 *
 * @since 4.0.0
 */
class ParseSchemaOracleUnitTest extends ParseSchemaUnitTest
{


    /**
     * Test for Oracle schema parsing.
     *
     * Tests to verify that the SQL produced from parsing a test schema is what
     * is expected for the Oracle database.
     *
     * @since  4.0.0
     * @return void
     */
    public function testOracleSchemaParse()
    {
        if ($this->skipOnDbType('oracle') === TRUE) {
            return;
        }

        $result = $this->getSqlForTestSchema();

        PHPUnit_Framework_Assert::assertEquals($this->_getSql(), $result);

    }//end testOracleSchemaParse()


    /**
     * Returns the SQL expected for Oracle.
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
        $sql .= 'type_code NVARCHAR2(100),'."\n";
        $sql .= 'version NVARCHAR2(20),'."\n";
        $sql .= 'name NVARCHAR2(100),'."\n";
        $sql .= 'description NVARCHAR2(255),'."\n";
        $sql .= 'CONSTRAINT asset_type_pk PRIMARY KEY (type_code),'."\n";
        $sql .= 'CONSTRAINT asset_type_fk1 FOREIGN KEY (type_code) REFERENCES asset_type(type_code) ON DELETE CASCADE);'."\n";
        $sql .= 'CREATE INDEX asset_type_type_code ON asset_type (type_code);'."\n";
        $sql .= 'CREATE INDEX asset_type_lvl ON asset_type (version);'."\n";

        return $sql;

    }//end _getSql()


}//end class

?>
