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
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the executeQuery method.
 *
 * @since 4.0.0
 */
class ExecuteQueryUnitTest extends AbstractMySourceUnitTest
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
                'AssetType',
                'Asset',
                'LocalAsset',
                'Attribute',
                'AttributeDataSource',
                'Lookup',
                'Linking',
                'AdjacencyList',
                'MaterialisedPathLinking',
                'RecursiveLinking',
                'Design',
                'Keyword',
                'Frontend',
                'DataSource',
                'FileDataSource',
                'Project',
                'RelationshipDataSource',
                dirname(__FILE__).'/TestSystems/TestDALSys1/',
               );

    }//end getRequiredSystems()


    /**
     * This test messes around with the ovens, so we need to bake separately.
     *
     * @since  4.0.0
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Tests that executeAll returns number of rows for an existent record.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExistentRecord()
    {
        try {
            $assetid  = Asset::create('asset');
            $expected = 1;
            DAL::beginTransaction();
            $rows = TestDALSys1::dalTestExecuteQuery('asset', 'name', 'VALUE 1');
            DAL::rollback();
            PHPUnit_Framework_Assert::assertEquals($expected, $rows);
        } catch (ChannelException $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testExistentRecord()


    /**
     * Tests that executeQuery returns 0 for a non existent record.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentRecord()
    {
        try {
            $expected = 0;
            DAL::beginTransaction();
            $rows = TestDALSys1::dalTestExecuteQuery('asset', 'firstname', 'VALUE 1');
            DAL::rollback();
            PHPUnit_Framework_Assert::assertEquals($expected, $rows);
        } catch (ChannelException $e) {
            DAL::rollback();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testNonExistentRecord()


}//end class

?>
