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
 * Unit tests for the validateTableColumns method.
 *
 * validateTableColumns method throws exception if there are any errors.
 *
 */
class ValidateTableColumnsUnitTest extends AbstractMySourceUnitTest
{


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
     * Test validateTableColumns() throws exception when table has no columns.
     *
     * @return void
     */
    public function testNoColumns()
    {
        $xml = '<table name="test"></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has no columns.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableColumns($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoColumns()


    /**
     * Test validateTableColumns() for invalid table.
     *
     * Test that validateTableColumns throws exception when table has column
     * with no type attribute.
     *
     * @return void
     */
    public function testNoTypeAttribute()
    {
        $xml = '<table name="test"><columns>
                  <column size="255" allow-null="false" default="">name</column>
                  <column type="TYPECODE" allow-null="false">type_code</column>
                </columns></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table column at #1 does not have "type" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableColumns($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoTypeAttribute()


    /**
     * Test validateTableColumns() for invalid table.
     *
     * Test that validateTableColumns throws exception when table has column
     * with no size attribute.
     *
     * @return void
     */
    public function testNoSizeAttribute()
    {
        $xml = '<table name="test"><columns>
                  <column type="VARCHAR" allow-null="false" default="">name</column>
                  <column type="TYPECODE" allow-null="false">type_code</column>
                </columns></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table column at #1 does not have "size" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableColumns($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoSizeAttribute()


    /**
     * Test validateTableColumns() for invalid table.
     *
     * Test that validateTableColumns throws exception when table has column
     * that is of type NUMERIC and has no scale attribute.
     *
     * @return void
     */
    public function testNoScaleAttribute()
    {
        $xml = '<table name="test"><columns>
                  <column type="NUMERIC" size="20" allow-null="false" default="">name</column>
                </columns></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table column at #1 is type NUMERIC and it does not have "scale" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableColumns($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoScaleAttribute()


    /**
     * Test for validateTableColumns() for invalid table.
     *
     * Test that validateTableColumns throws exception when table has column
     * that is of type NUMERIC and has no scale attribute.
     *
     * @return void
     */
    public function testNoName()
    {
        $xml = '<table name="test"><columns>
                  <column type="NUMERIC" size="20" scale="2" allow-null="false" default=""></column>
                </columns></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table column at #1 does not have a column name.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableColumns($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoName()


}//end class

?>
