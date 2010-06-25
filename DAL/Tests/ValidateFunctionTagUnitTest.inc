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
 * Unit tests for validateFunctionTag method.
 *
 * @since 4.0.0
 */
class ValidateFunctionTagUnitTest extends AbstractMySourceUnitTest
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
     * Test that a function without a function attribute causes an error.
     *
     * Test that validateFunctionTag throws exception if tag does not have
     * function attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoFunctionAttribute()
    {
        $xml = '<fields><function><arg>test</arg></function></fields>';
        $doc = new DomDocument();
        $doc->loadXML($xml);
        $expected = 'function tag must have \'function\' attribute.';

        $function = $doc->getElementsByTagName('function')->item(0);
        try {
            DALQueryParser::validateFunctionTag($function);
        } catch (DALParserException $e) {
            $msg = $e->getMessage();
        }

        PHPUnit_Framework_Assert::assertEquals($expected, $msg);

    }//end testNoFunctionAttribute()


    /**
     * Test that a funciton without arguments is still valid.
     *
     * Test that validateFunctionTag does not throws exception if tag does not
     * have arg attributes.
     *
     * Function might not have any arguments.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoFunctionArgs()
    {
        $xml = '<fields><function function="CONCAT"></function></fields>';
        $doc = new DomDocument();
        $doc->loadXML($xml);

        $function = $doc->getElementsByTagName('function')->item(0);
        try {
            DALQueryParser::validateFunctionTag($function);
        } catch (DALParserException $e) {
            $msg = 'There should be no exception, but got: '.$e->getMessage();
            PHPUnit_Framework_Assert::fail($msg);
        }

    }//end testNoFunctionArgs()


    /**
     * Tests funtion with empty arguments.
     *
     * Test that validateFunctionTag does not throws exception if it has arg
     * attributes with no values.
     *
     * Function might have empty strings arguments.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoFunctionArgValue()
    {
        $xml = '<fields><function function="CONCAT"><arg></arg></function></fields>';
        $doc = new DomDocument();
        $doc->loadXML($xml);

        $function = $doc->getElementsByTagName('function')->item(0);
        try {
            DALQueryParser::validateFunctionTag($function);
        } catch (DALParserException $e) {
            $msg = 'There should be no exception, but got: '.$e->getMessage();
            PHPUnit_Framework_Assert::fail($msg);
        }

    }//end testNoFunctionArgValue()


}//end class

?>
