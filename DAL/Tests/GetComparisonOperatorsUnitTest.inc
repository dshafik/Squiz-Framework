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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit Tests for the checking of getComparisonOperators() in DALBaker.
 *
 */
class GetComparisonOperatorsUnitTest extends AbstractMySourceUnitTest
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
     * Test the value returned for getComparisonOperators('equal').
     *
     * @return void
     */
    public function testGetComparisonEqual()
    {
        $expected = '=';
        $ret      = DALBaker::getComparisonOperators('equal');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonEqual()


    /**
     * Test the value returned for getComparisonOperators('not-equal').
     *
     * @return void
     */
    public function testGetComparisonNotEqual()
    {
        $expected = '!=';
        $ret      = DALBaker::getComparisonOperators('not-equal');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonNotEqual()


    /**
     * Test the value returned for getComparisonOperators('equal-or-greater').
     *
     * @return void
     */
    public function testGetComparisonEqualOrGreater()
    {
        $expected = '>=';
        $ret      = DALBaker::getComparisonOperators('equal-or-greater');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonEqualOrGreater()


    /**
     * Test the value returned for getComparisonOperators('equal-or-less').
     *
     * @return void
     */
    public function testGetComparisonEqualOrLess()
    {
        $expected = '<=';
        $ret      = DALBaker::getComparisonOperators('equal-or-less');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonEqualOrLess()


    /**
     * Test the value returned for getComparisonOperators('greater').
     *
     * @return void
     */
    public function testGetComparisonGreater()
    {
        $expected = '>';
        $ret      = DALBaker::getComparisonOperators('greater');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonGreater()


    /**
     * Test the value returned for getComparisonOperators('less').
     *
     * @return void
     */
    public function testGetComparisonLess()
    {
        $expected = '<';
        $ret      = DALBaker::getComparisonOperators('less');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonLess()


    /**
     * Test the value returned for getComparisonOperators('like').
     *
     * @return void
     */
    public function testGetComparisonLike()
    {
        $expected = 'LIKE';
        $ret      = DALBaker::getComparisonOperators('like');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonLike()


    /**
     * Test the value returned for getComparisonOperators('is-null').
     *
     * @return void
     */
    public function testGetComparisonIsNull()
    {
        $expected = 'IS NULL';
        $ret      = DALBaker::getComparisonOperators('is-null');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonIsNull()


    /**
     * Test the value returned for getComparisonOperators('not-null').
     *
     * @return void
     */
    public function testGetComparisonNotNull()
    {
        $expected = 'NOT NULL';
        $ret      = DALBaker::getComparisonOperators('not-null');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonNotNull()


    /**
     * Test the value returned is empty for an invalid comparison.
     *
     * @return void
     */
    public function testGetComparisonInvalid()
    {
        $expected = '';
        $ret      = DALBaker::getComparisonOperators('invalid_operator');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonInvalid()


    /**
     * Test the value returned for a NULL argument.
     *
     * @return void
     */
    public function testGetComparisonNullArgument()
    {
        $expected = array(
                     'equal'            => '=',
                     'not-equal'        => '!=',
                     'equal-or-greater' => '>=',
                     'equal-or-less'    => '<=',
                     'greater'          => '>',
                     'less'             => '<',
                     'like'             => 'LIKE',
                     'is-null'          => 'IS NULL',
                     'not-null'         => 'NOT NULL',
                    );

        $ret = DALBaker::getComparisonOperators(NULL);
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetComparisonNullArgument()


}//end class