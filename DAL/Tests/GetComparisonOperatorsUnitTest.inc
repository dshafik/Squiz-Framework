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
 * Unit Tests for the checking of getComparisonOperators() in DALBaker.
 *
 * @since 4.0.0
 */
class GetComparisonOperatorsUnitTest extends AbstractMySourceUnitTest
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
     * Test the value returned for getComparisonOperators('equal').
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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