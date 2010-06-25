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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';

/**
 * Unit tests for the getQuerySource() method.
 *
 * getQuerySource() returns query in its XML format.
 *
 * @since 4.0.0
 */
class GetQuerySourceUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys1',
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys2',
               );

    }//end getRequiredSystems()


    /**
     * Test that getQuerySource returns null if system does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentSystem()
    {
        $result = DALBaker::getQuerySource('Tasdfsdfsdf', 'getLinks');

        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSystem()


    /**
     * Test that getQuerySource returns null if query does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentQuery()
    {
        $result = DALBaker::getQuerySource('TestSys1', 'sdfsdfsdf');

        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentQuery()


    /**
     * Test that getQuerySource returns DomElement if query does exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExistentQuery()
    {
        $result = DALBaker::getQuerySource('TestSys1', 'getLinks');

        PHPUnit_Framework_Assert::assertNotNull($result);

    }//end testExistentQuery()


}//end class

?>
