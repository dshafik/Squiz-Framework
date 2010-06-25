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
 * Unit tests for the validateConstraints() method.
 *
 * validateConstraints() returns array of constraints from a parent node.
 *
 * @since 4.0.0
 */
class ValidateConstraintsUnitTest extends AbstractMySourceUnitTest
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
     * Test validateConstraints for valid arguments.
     *
     * Test that validateConstraints does not throw exception for valid
     * constraints.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidConstraints()
    {
        $xml = '<query id="test">
                 <test>
                    <primary-key name="asset_type_pk">
                        <column>type_code</column>
                    </primary-key>
                    <foreign-key foreign-table="asset_type" name="asset_type_fk1" on-delete="CASCADE">
                        <column references="type_code">parent_type</column>
                    </foreign-key>
                    <unique name="unique">
                        <column>col2</column>
                    </unique>
                </test></query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $parent = $doc->getElementsByTagName('test')->item(0);
            $result = DALSchemaParser::validateConstraints($parent);
        } catch (DALParserException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

        PHPUnit_Framework_Assert::assertFalse($exception);

    }//end testValidConstraints()


    /**
     * Test that validateConstraints throws exception for invalid constraints.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInValidConstraints()
    {
        $xml = '<query id="test">
                 <test>
                    <primary-key>
                        <column>type_code</column>
                    </primary-key>
                </test></query>';

        $exception = FALSE;
        try {
            $doc = new DomDocument();
            $doc->loadXML($xml);
            $parent = $doc->getElementsByTagName('test')->item(0);
            $result = DALSchemaParser::validateConstraints($parent);
        } catch (DALParserException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testInValidConstraints()


}//end class

?>
