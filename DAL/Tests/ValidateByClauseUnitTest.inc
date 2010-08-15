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
 * Unit tests for the validateByClause method.
 *
 * validateByClause() method throws exception if there is an error in the query.
 */
class ValidateByClauseUnitTest extends AbstractMySourceUnitTest
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
     * Test for validation of by clauses.
     *
     * Test that validateByClause does not throws exception when no group by tag
     * is given, it will ignore it.
     *
     * @return void
     */
    public function testInvalidByClauseType()
    {
        $xml = '<select><notvalidtype></notvalidtype></select>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $select = $queryXml->getElementsByTagName('notvalidtype')->item(0);
        $caught = FALSE;

        try {
            DALGroupByParser::validate($select);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testInvalidByClauseType()


    /**
     * Test for validation of by clauses.
     *
     * Test that validateByClause throws exception when no group by/order by
     * fields or hooks specified.
     *
     * @return void
     */
    public function testNoChildren()
    {
        $xml = '<select><group-by></group-by></select>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $select = $queryXml->getElementsByTagName('select')->item(0);
        $caught = FALSE;

        try {
            DALSelectParser::validate($select, 'group-by');
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testNoChildren()


    /**
     * Test for validation of by clauses.
     *
     * Test that validateByClause throws exception when invalid group/order by
     * children specified, there must be a field or a hook.
     *
     * @return void
     */
    public function testInvalidChildren()
    {
        $xml = '<select><group-by><invalid></invalid></group-by></select>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $select = $queryXml->getElementsByTagName('select')->item(0);
        $caught = FALSE;

        try {
            DALSelectParser::validate($select, 'group-by');
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testInvalidChildren()


    /**
     * Test for validation of by clauses.
     *
     * Test that validateByClause does not throws exception when valid
     * group/order by children specified.
     *
     * @return void
     */
    public function testValidChildren()
    {
        $xml = '<select><group-by><hook id="test" /></group-by></select>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $select = $queryXml->getElementsByTagName('select')->item(0);
        $caught = FALSE;

        try {
            DALGroupByParser::validate($select, 'group-by');
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidChildren()


    /**
     * Test for validation of by clauses.
     *
     * Test that validateByClause does not throws exception when valid
     * group/order by children specified.
     *
     * @return void
     */
    public function testGroupBy()
    {
        $xml = '<select><group-by><invalid></invalid></group-by></select>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $select = $queryXml->getElementsByTagName('select')->item(0);
        $caught = FALSE;

        try {
            DALSelectParser::validate($select);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testGroupBy()


    /**
     * Test for validation of by clauses.
     *
     * Test that validateByClause does not throws exception when valid
     * group/order by children specified.
     *
     * @return void
     */
    public function testOrderBy()
    {
        $xml = '<select><order-by><invalid></invalid></order-by></select>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $select = $queryXml->getElementsByTagName('select')->item(0);
        $caught = FALSE;

        try {
            DALSelectParser::validate($select);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testOrderBy()


}//end class

?>
