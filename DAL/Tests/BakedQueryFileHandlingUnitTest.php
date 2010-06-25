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
 * Unit Tests for the getBakedQueryFileName and loadBakedQuery methods.
 *
 * @since 4.0.0
 */
class BakedQueryFileHandlingUnitTest extends AbstractMySourceUnitTest
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
     * Test getBakedQueryFileName() returns the path of a newly generated file.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetBakedQueryFileName()
    {
        DALBaker::generateQueryXml('TestSystem', 'TestQuery');
        $fileName = DALBaker::getBakedQueryFileName('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertTrue(file_exists($fileName));

    }//end testGetBakedQueryFileName()


    /**
     * Test getBakedQueryFileName() returns a non-existent file path.
     *
     * This is tested after the Oven directory is cleared.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetBakedQueryFileNameNoFile()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $fileName = DALBaker::getBakedQueryFileName('abc', 'def');
        PHPUnit_Framework_Assert::assertFalse(file_exists($fileName));

    }//end testGetBakedQueryFileNameNoFile()


    /**
     * Test loadBakedQuery() returns a DomDocument on the generated file.
     *
     * @since  4.0.0
     * @return void
     */
    public function testLoadBakedQuery()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        DALBaker::generateQueryXml('TestSystem', 'TestQuery');
        $doc   = DALBaker::loadBakedQuery('TestSystem', 'TestQuery');
        $class = get_class($doc);
        PHPUnit_Framework_Assert::assertEquals('DOMDocument', get_class($doc));

    }//end testLoadBakedQuery()


    /**
     * Test loadBakedQuery() returns NULL for a non-existent file.
     *
     * @since  4.0.0
     * @return void
     */
    public function testLoadBakedQueryNull()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $doc = DALBaker::loadBakedQuery('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertNull($doc);

    }//end testLoadBakedQueryNull()


}//end class
