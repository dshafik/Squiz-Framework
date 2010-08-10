<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage DAL
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the executeQueries method.
 *
 */
class ExecuteQueriesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Query xml for this unit test.
     *
     * @var   string $_query
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
