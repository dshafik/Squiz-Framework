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
 * Unit Tests for the creation of oven directories.
 *
 * @since 4.0.0
 */
class OvenDirectoryCreationUnitTest extends AbstractMySourceUnitTest
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
     * Test the return value of the createSystemOvenPath() method.
     *
     * Test is performed for a non-existent directory.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCreateSystemDirectoryNotExistsReturn()
    {
        $ret = DALBaker::createSystemOvenPath('TestSystem');
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testCreateSystemDirectoryNotExistsReturn()


    /**
     * Test the return value of the createSystemOvenPath() method.
     *
     * Test is performed for a directory that exists.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCreateSystemDirectoryExistsReturn()
    {
        DALBaker::createSystemOvenPath('TestSystem');
        $ret = DALBaker::createSystemOvenPath('TestSystem');

        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testCreateSystemDirectoryExistsReturn()


    /**
     * Test the path of a newly created system exists.
     *
     * Test is performed for a directory that exists.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetOvenPath()
    {
        DALBaker::createSystemOvenPath('TestSystem');
        $path = DAL::getOvenPath('TestSystem');
        $ret  = file_exists($path);

        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testGetOvenPath()


    /**
     * Test the path of a system doesn't exist.
     *
     * Test is performed for a directory that exists.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetOvenPathEmptyDir()
    {
        $path = DAL::getOvenPath('TestSystemThatDoesNotExist');
        $ret  = file_exists($path);

        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testGetOvenPathEmptyDir()


}//end class

?>
