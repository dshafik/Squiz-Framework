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
 * Unit tests for the validateField method.
 *
 * validateField() method throws exception if there is an error in the query.
 *
 */
class ValidateFieldUnitTest extends AbstractMySourceUnitTest
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
     * Tests field with no table attribute.
     *
     * Test that validateField throws exception when field does not have table
     * attribute.
     *
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
