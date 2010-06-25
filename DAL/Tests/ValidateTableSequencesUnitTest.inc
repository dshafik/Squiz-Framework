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
 * Unit tests for the validateTableSequences method.
 *
 * validateTableSequences method throws exception if there are any errors.
 *
 * @since 4.0.0
 */
class ValidateTableSequencesUnitTest extends AbstractMySourceUnitTest
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
     * Tests empty sequences declaration.
     *
     * Test that validateTableSequences throws exception when it has a
     * sequences with no sequence tags.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoSequnceTags()
    {
        $xml = '<table name="test"><sequences></sequences></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has sequences tag with no sequence tags.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableSequences($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoSequnceTags()


    /**
     * Test sequence with missing name attribute.
     *
     * Test that validateTableSequences throws exception when it has a
     * sequence tag with no name attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoSequenceName()
    {
        $xml = '<table name="test"><sequences><sequence></sequence></sequences></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table  = $queryXml->getElementsByTagName('table')->item(0);
        $expMsg = 'test\'s table has sequence with missing "name" attribute.';
        $msg    = '';

        try {
            DALSchemaParser::validateTableSequences($table);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoSequenceName()


}//end class

?>
