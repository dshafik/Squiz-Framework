<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage DAL
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the validSchemaColumnType() method.
 *
 * validSchemaColumnType() determines if the given type is a valid column type.
 *
 */
class ValidSchemaColumnTypeUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var   string $_schema Schema xml.
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
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Test that getQuerySource returns null if system does not exist.
     *
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
