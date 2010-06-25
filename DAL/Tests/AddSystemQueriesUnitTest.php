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
 * Unit Tests for the addSystemQueries() method of DALBaker.
 *
 * @since 4.0.0
 */
class AddSystemQueriesUnitTest extends AbstractMySourceUnitTest
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
                dirname(__FILE__).'/TestSystems/MergeSys1',
               );

    }//end getRequiredSystems()


    /**
     * Adds the test system which is used for all tests.
     *
     * @since  4.0.0
     * @return NULL
     */
    public function setupSystems()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        DALBaker::addSystemQueries('MergeSys1');

    }//end setupSystems()


    /**
     * Test addSystemQueries() produces the correct number of queries.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddSystemQueriesCount()
    {
        self::setupSystems();
        $xmlFiles = FileSystem::listDirectory(DAL::getOvenPath(), array('.xml'));
        PHPUnit_Framework_Assert::assertEquals(4, count($xmlFiles));

    }//end testAddSystemQueriesCount()


    /**
     * Test addSystemQueries() produces the correct file names for XML files.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddSystemQueriesFileNames()
    {
        self::setupSystems();
        $xmlFiles = FileSystem::listDirectory(DAL::getOvenPath(), array('.xml'), TRUE, FALSE);
        $expected = array(
                     'getLinks.xml',
                     'getLinksTwo.xml',
                     'getLinksThree.xml',
                     'getLinksFour.xml',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $xmlFiles);

    }//end testAddSystemQueriesFileNames()


    /**
     * Test addSystemQueries() returns FALSE for non-existent system.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddSystemQueriesNoSystem()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $ret = DALBaker::addSystemQueries('NonSystem');
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testAddSystemQueriesNoSystem()


}//end class
