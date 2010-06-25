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
 * Unit tests for the getTableSequences method.
 *
 * getTableSequences method returns correct values.
 *
 * @since 4.0.0
 */
class GetTableSequencesUnitTest extends AbstractMySourceUnitTest
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
     * Test for getTableSequences() with no sequences.
     *
     * Test that getTableSequences returns empty array when table has no
     * sequences.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoSequences()
    {
        $xml = '<table name="test"></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array();
        $retVal   = DALSchemaParser::getTableSequences($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testNoSequences()


    /**
     * Test for getTableSequences() for valid sequences.
     *
     * Test that getTableSequences returns non empty array when table has
     * sequences.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSequences()
    {
        $xml = '<table name="test">
                <sequences>
                    <sequence name="assetid_seq" />
                    <sequence name="assetid_seq2" />
                </sequences></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array (
                     0 => 'assetid_seq',
                     1 => 'assetid_seq2',
                    );
        $retVal = DALSchemaParser::getTableSequences($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testSequences()


}//end class

?>
