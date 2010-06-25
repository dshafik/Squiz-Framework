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
 * Unit tests for the validateValuesTag and validateValueTag method.
 *
 * These methods throws exception if they do not have required children etc.
 *
 * @since 4.0.0
 */
class ValidateValueTagUnitTest extends AbstractMySourceUnitTest
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
     * Test validateValuesTag throws exception when it has no value children.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoValuesTags()
    {
        $xml = '<insert><values></values></insert>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $values = $queryXml->getElementsByTagName('values')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateField($values);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoValuesTags()


    /**
     * Test that validateValueTag throws exception when it has no column attr.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoValueTags()
    {
        $xml = '<insert><values><value></value></values></insert>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $value  = $queryXml->getElementsByTagName('value')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateField($value);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoValueTags()


    /**
     * Test valid values tag.
     *
     * Test that validateValuesTag does not throw exception when it has value
     * children and they are valid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidValuesTags()
    {
        $xml = '<insert><values><value column="ts">a</value></values></insert>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $values = $queryXml->getElementsByTagName('values')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateValuesTag($values);
        } catch (DALParserException $e) {
            var_dump($e->getMessage());
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidValuesTags()


}//end class

?>
