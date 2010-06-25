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
 * Unit Tests for the getSeperator() methods.
 *
 * @since 4.0.0
 */
class GetSeperatorUnitTest extends AbstractMySourceUnitTest
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
     * Tests the getSeperator() method for 'union-all'.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetSeperatorUnionAll()
    {
        $expected = '\nUNION-ALL\n';
        $ret      = DALBaker::getSeperator('union-all');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetSeperatorUnionAll()


    /**
     * Tests the getSeperator() method for 'and'.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetSeperatorAnd()
    {
        $expected = '\nAND\n';
        $ret      = DALBaker::getSeperator('and');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetSeperatorAnd()


    /**
     * Tests the getSeperator() method for 'or'.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetSeperatorOr()
    {
        $expected = '\nOR\n';
        $ret      = DALBaker::getSeperator('or');
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetSeperatorOr()


}//end class
