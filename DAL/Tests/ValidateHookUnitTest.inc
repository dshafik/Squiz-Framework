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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the validateHook method.
 *
 * validateHook() method throws exception if hook does not have id attribute.
 *
 * @since 4.0.0
 */
class ValidateHookUnitTest extends AbstractMySourceUnitTest
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
     * Test that validateHook throws exception when hook does not have id attr.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoIdAttribute()
    {
        $xml = '<fields><hook /></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $hook   = $queryXml->getElementsByTagName('hook')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateHook($hook);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoIdAttribute()


    /**
     * Test that validateHook throws exception when hook does not have id attr.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidHook()
    {
        $xml = '<fields><hook id="valid" /></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $hook   = $queryXml->getElementsByTagName('hook')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateHook($hook);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidHook()


}//end class

?>
