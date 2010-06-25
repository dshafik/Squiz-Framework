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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the validSchemaColumnType() method.
 *
 * validSchemaColumnType() determines if the given type is a valid column type.
 *
 * @since 4.0.0
 */
class ValidSchemaColumnTypeUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var   string $_schema Schema xml.
     * @since 4.0.0
     */
    private $_schema = '<schema system="asset">
                         <udts>
                          <udt datatype="INTEGER">ASSETID</udt>
                          <udt-import system="asset_type">TYPECODE</udt-import>
                         </udts>
                         <tables>
                          <table name="asset">
                           <columns></columns></table></tables></schema>';


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test that getQuerySource returns null if system does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testLowercaseColumnType()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);
        $schema = $doc->getElementsByTagName('schema')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($schema, 'varchar');

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testLowercaseColumnType()


    /**
     * Test a column with a base (non-UDT) type specified.
     *
     * Test that validSchemaColumnType returns TRUE for a type that was defined
     * in DALBaker::$_columnDataTypes array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testBaseColumnType()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);
        $schema = $doc->getElementsByTagName('schema')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($schema, 'VARCHAR');

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testBaseColumnType()


    /**
     * Tests a column that has a non-existent type.
     *
     * Test that validSchemaColumnType returns FALSE for a type that is not
     * defined anywhere in the schema or not a base type.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidColumnType()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);
        $schema = $doc->getElementsByTagName('schema')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($schema, 'NOTVALID');

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testInvalidColumnType()


    /**
     * Test a column with a type that is a local UDT.
     *
     * Test that validSchemaColumnType returns TRUE for a type that was defined
     * as a local UDT (User Defined Type).
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidLocalUdtColumnType()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);
        $schema = $doc->getElementsByTagName('schema')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($schema, 'ASSETID');

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testValidLocalUdtColumnType()


    /**
     * Test column type that is a foreign UDT.
     *
     * Test that validSchemaColumnType returns TRUE for a type that was defined
     * as a foreign UDT (User Defined Type).
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidForeignUdtColumnType()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);
        $schema = $doc->getElementsByTagName('schema')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($schema, 'TYPECODE');

        PHPUnit_Framework_Assert::assertTrue($result);

    }//end testValidForeignUdtColumnType()


    /**
     * Test that validSchemaColumnType returns FALSE for an empty type.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyColumnType()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);
        $schema = $doc->getElementsByTagName('schema')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($schema, '');

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testEmptyColumnType()


    /**
     * Test that validSchemaColumnType returns FALSE for schema with no UDTs.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyUdts()
    {
        $xml = '<schema system="asset"><tables><table name="test"></table>
                </tables></schema>';

        $doc = new DomDocument();
        $doc->loadXML($xml);
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::validSchemaColumnType($table, 'ASSETID');

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testEmptyUdts()


}//end class

?>
