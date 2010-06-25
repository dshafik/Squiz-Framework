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
 * Unit tests for the validateField method.
 *
 * validateField() method throws exception if there is an error in the query.
 *
 * @since 4.0.0
 */
class ValidateFieldUnitTest extends AbstractMySourceUnitTest
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
     * Tests field with no table attribute.
     *
     * Test that validateField throws exception when field does not have table
     * attribute.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoTableAttr()
    {
        $xml = '<fields><field column="col"></field></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $field  = $queryXml->getElementsByTagName('field')->item(0);
        $caught = FALSE;
        $expMsg = 'field tag must have table attr.';
        $msg    = '';

        try {
            DALQueryParser::validateField($field);
        } catch (DALParserException $e) {
            $msg    = $e->getMessage();
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoTableAttr()


    /**
     * Tests field with no column attribute.
     *
     * Test that validateField throws exception when field does not have table
     * column.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoColumnAttr()
    {
        $xml = '<fields><field table="col"></field></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $field  = $queryXml->getElementsByTagName('field')->item(0);
        $caught = FALSE;
        $expMsg = 'field tag must have column attr.';
        $msg    = '';

        try {
            DALQueryParser::validateField($field);
        } catch (DALParserException $e) {
            $msg    = $e->getMessage();
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testNoColumnAttr()


    /**
     * Test for invalid alias use.
     *
     * Test that validateField throws exception when field uses alias but its
     * not allowed.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAliasNotAllowed()
    {
        $xml = '<fields><field column="col" table="col">alias</field></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $field  = $queryXml->getElementsByTagName('field')->item(0);
        $caught = FALSE;
        $expMsg = 'alias for field is not allowed here.';
        $msg    = '';

        try {
            DALQueryParser::validateField($field);
        } catch (DALParserException $e) {
            $msg    = $e->getMessage();
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertEquals($expMsg, $msg);

    }//end testAliasNotAllowed()


    /**
     * Test for validateField() using a valid alias.
     *
     * Test that validateField does not throws exception when field uses alias
     * and it is allowed.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAliasAllowed()
    {
        $xml = '<fields><field column="col" table="col">alias</field></fields>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $field  = $queryXml->getElementsByTagName('field')->item(0);
        $caught = FALSE;

        try {
            DALQueryParser::validateField($field, TRUE);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testAliasAllowed()


}//end class

?>
