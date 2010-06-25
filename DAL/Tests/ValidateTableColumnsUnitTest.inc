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
 * Unit tests for the validateTableColumns method.
 *
 * validateTableColumns method throws exception if there are any errors.
 *
 * @since 4.0.0
 */
class ValidateTableColumnsUnitTest extends AbstractMySourceUnitTest
{


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
     * Test validateTableColumns() throws exception when table has no columns.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
