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
 * Unit tests for the executeQueries method.
 *
 * @since 4.0.0
 */
class ExecuteQueriesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Query xml for this unit test.
     *
     * @var   string $_query
     * @since 4.0.0
     */
    private static $_query = '<query id="dalTestExecuteQuery">
        <insert>
            <fields table="asset">
                <field>assetid</field>
                <field>typeid</field>
            </fields>
            <values>
                <value column="assetid">:assetid</value>
                <value column="typeid">:typeid</value>
            </values>
        </insert>
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
     * Returns sql statement.
     *
     * @param integer $assetid Asset id to replace.
     * @param integer $typeid  Type id.
     *
     * @since  4.0.0
     * @return string
     */
    private static function _getInsertQuery($assetid, $typeid)
    {
        $query = str_replace(':assetid', $assetid, self::$_query);
        $query = str_replace(':typeid', '\''.$typeid.'\'', $query);

        $doc = new DomDocument;
        $doc->loadXml($query);
        $query = $doc->getElementsByTagName('query')->item(0);
        $sql   = DALBaker::convertToSql($query);

        return $sql;

    }//end _getInsertQuery()


    /**
     * Tests that executeQueries method can execute multiple queries.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExistentRecord()
    {
        try {
            DAL::beginTransaction();
            $assetidOne = 123;
            $typeid     = 'asset';
            $sql        = $this->_getInsertQuery($assetidOne, $typeid).';';

            $assetidTwo = 124;
            $typeid     = 'asset';
            $sql       .= $this->_getInsertQuery($assetidTwo, $typeid);

            $expected = 'asset';
            DAL::executeQueries($sql);

            $type = TestDALSys1::dalTestExecuteOne($assetidOne);
            PHPUnit_Framework_Assert::assertEquals($expected, $type);

            $type = TestDALSys1::dalTestExecuteOne($assetidTwo);
            PHPUnit_Framework_Assert::assertEquals($expected, $type);
            DAL::rollBack();

        } catch (PDOException $e) {
            DAL::rollBack();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        } catch (ChannelException $e) {
            DAL::rollBack();
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testExistentRecord()


}//end class

?>
