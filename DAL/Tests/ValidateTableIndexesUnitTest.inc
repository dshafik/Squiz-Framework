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
 * Unit tests for the validateTableIndexes method.
 *
 * validateTableIndexes method throws exception if there are any errors.
 *
 * @since 4.0.0
 */
class ValidateTableIndexesUnitTest extends AbstractMySourceUnitTest
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
     * Tests validateValuesTag() with no name.
     *
     * Test that validateValuesTag throws exception when it has an index with no
     * name attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoIndexName()
    {
        $xml = '<table name="test"><indexes><index></index></indexes></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has index with missing "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableIndexes($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoIndexName()


    /**
     * Tests validateValuesTag() with no columns.
     *
     * Test that validateValuesTag throws exception when it has index with no
     * columns.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoIndexColumns()
    {
        $xml = '<table name="test"><indexes>
                <index name="idx1"></index></indexes></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has index with no columns.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableIndexes($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoIndexColumns()


    /**
     * Tests validateValuesTag() with an undefined columns.
     *
     * Test that validateValuesTag throws exception when it has index that uses
     * undefined column.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidColumn()
    {
        $xml = '<table name="test"><indexes>
                <index name="idx1">
                    <column>col1</column>
                </index></indexes></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table does not have column "col1" but it was used in its index.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableIndexes($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testInvalidColumn()


}//end class

?>
