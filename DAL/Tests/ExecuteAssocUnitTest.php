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
 * Unit tests for the executeAssoc & getAssoc methods.
 *
 * @since 4.0.0
 */
class ExecuteAssocUnitTest extends AbstractMySourceUnitTest
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
     * Tests that executeAssoc returns an correct assoc array for an existent record.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExistentRecord()
    {
        try {
            $assetid = Asset::create('asset');

            $type = TestDALSys1::dalTestExecuteAssoc($assetid);

            $expected = array(
                         0 => array(
                               'typeid' => 'asset',
                              ),
                        );

            PHPUnit_Framework_Assert::assertEquals($expected, $type);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testExistentRecord()


    /**
     * Tests that executeAssoc returns empty array for a non existent record.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentRecord()
    {
        try {
            $assetid = 4235;
            $type    = TestDALSys1::dalTestExecuteAssoc($assetid);
            PHPUnit_Framework_Assert::assertTrue(empty($type));
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testNonExistentRecord()


}//end class

?>
