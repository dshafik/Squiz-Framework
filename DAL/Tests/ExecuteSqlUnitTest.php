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
 * Unit tests for the executeSql method.
 *
 * @since 4.0.0
 */
class ExecuteSqlUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Query xml for this unit test.
     *
     * @var   string $_query
     * @since 4.0.0
     */
    private static $_query = '<query id="dalTestExecuteQuery">
        <update>
            <fields table="attribute">
                <field>initial_val</field>
            </fields>
            <values>
                <value column="initial_val">\'newValue\'</value>
            </values>
            <where>
                <equal table="attribute" column="attributeid">:attributeid</equal>
                <equal table="attribute" column="asset_type">\'asset\'</equal>
            </where>
        </update>
    </query>';


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
     * Tests that executeSql returns a correct value for an existent record.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExistentRecord()
    {
        try {
            DAL::beginTransaction();
            $assetid = Asset::create('asset');
            $query   = str_replace(':attributeid', '\'name\'', self::$_query);

            $expected = 1;

            $doc = new DomDocument;
            $doc->loadXml($query);
            $query = $doc->getElementsByTagName('query')->item(0);

            $sql = DALBaker::convertToSql($query);

            $rows = TestDALSys1::dalTestExecuteSql($sql);
            DAL::rollBack();

            PHPUnit_Framework_Assert::assertEquals($expected, $rows);
        } catch (ChannelException $e) {
            DAL::rollBack();
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testExistentRecord()


    /**
     * Tests that executeSql returns correct value for a non existent record.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNonExistentRecord()
    {
        try {
            DAL::beginTransaction();

            $query = str_replace(':attributeid', '\'sarsasdsd\'', self::$_query);

            $doc = new DomDocument;
            $doc->loadXml($query);
            $query = $doc->getElementsByTagName('query')->item(0);

            $sql = DALBaker::convertToSql($query);

            $rows = TestDALSys1::dalTestExecuteSql($sql);
            DAL::rollBack();
            $expected = 0;
            PHPUnit_Framework_Assert::assertEquals($expected, $rows);
        } catch (ChannelException $e) {
            DAL::rollBack();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testNonExistentRecord()


    /**
     * Tests that executeSql throws PDO exception for a bad query.
     *
     * @since  4.0.0
     * @return void
     */
    public function testBadQuery()
    {
        $exception = FALSE;
        try {
            $sql  = 'INSDFTF sfsfsfsfd sdf';
            $rows = TestDALSys1::dalTestExecuteSql($sql);
        } catch (PDOException $e) {
            $exception = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testBadQuery()


}//end class

?>
