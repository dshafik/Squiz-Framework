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
 * Unit tests for the loadQueriesXml() method.
 *
 * loadQueriesXml() returns the DomDocument object for the system's queries XML.
 *
 * @since 4.0.0
 */
class LoadQueriesXmlUnitTest extends AbstractMySourceUnitTest
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
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys2',
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys1',
               );

    }//end getRequiredSystems()


    /**
     * Test that loadQueriesXml returns NULL when system does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentSystem()
    {
        $result = DALBaker::loadQueriesXml('InvalidSystem');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSystem()


    /**
     * Test that loadQueriesXml returns NULL when quries XML does not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentQueriesFile()
    {
        $result = DALBaker::loadQueriesXml('TestSys2');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentQueriesFile()


    /**
     * Test that loadQueriesXml returns DomDocument when quries file exists.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidQueriesFile()
    {
        $result = DALBaker::loadQueriesXml('TestSys1');
        PHPUnit_Framework_Assert::assertTrue(is_a($result, 'DomDocument'));

    }//end testValidQueriesFile()


}//end class

?>
