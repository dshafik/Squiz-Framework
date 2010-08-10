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
 * Unit tests for the executeSql method.
 *
 */
class ExecuteSqlUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Query xml for this unit test.
     *
     * @var   string $_query
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
     * Tests that executeSql returns a correct value for an existent record.
     *
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
