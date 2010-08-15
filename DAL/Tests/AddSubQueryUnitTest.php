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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the addQueryFragment method.
 */
class AddSubQueryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Base query for testing the addSubQuery() method.
     *
     * @var string $_baseQuery The XML source of a test query.
     */
    private static $_baseQuery = '<query id="testone">
                                  <primary>
                                  <select>
                                    <fields>
                                        <field table="link_table" column="childid">id</field>
                                        <field table="asset_table" column="type_code">type</field>
                                    </fields>
                                    <from>
                                        <table>link_table</table>
                                        <table>perm_table</table>
                                        <table>asset_table</table>
                                    </from>
                                    <where>
                                        <equal table="link_table" column="parentid"><value>123</value></equal>
                                        <equal table="perm_table" column="userid"><value>10</value></equal>
                                        <equal table="perm_table" column="access">:accessLevel</equal>
                                        <and>
                                            <hook id="hook1" />
                                        </and>
                                    </where>
                                    <joins>
                                        <join>
                                            <field table="asset_table" column="assetid" />
                                            <field table="link_table" column="childid" />
                                        </join>
                                        <join>
                                            <field table="perm_table" column="assetid" />
                                            <field table="link_table" column="childid" />
                                        </join>
                                    </joins>
                                  </select>
                                  </primary>
                                  </query>';

    /**
     * Test sub-query for testing the addSubQuery() method.
     *
     * @var string $_subQuery The XML source of a test query.
     */
    private static $_subQuery = '<query id="testoneFragment" hookid="TestSys1.testone.hook1">
                                 <primary>
                                 <select>
                                    <fields />
                                    <from>
                                        <table>asset_table</table>
                                    </from>
                                    <where>
                                        <equal table="asset_table" column="type_code"><value>page</value></equal>
                                    </where>
                                    <joins>
                                        <join>
                                            <field table="asset_table" column="assetid" />
                                            <field table="link_table" column="childid" />
                                        </join>
                                    </joins>
                                  </select>
                                  </primary>
                                  </query>';


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys1',
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys2',
               );

    }//end getRequiredSystems()


    /**
     * Adds a base query used by the tests.
     *
     * @return void
     */
    public function addBaseQuery()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());

        $doc = new DomDocument();
        $doc->loadXml(self::$_baseQuery);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addQuery('TestSys1', $queryNode);

    }//end addBaseQuery()


    /**
     * Adds a fragment used for the tests.
     *
     * @return void
     */
    public function addSubQuery()
    {
        $doc = new DomDocument();
        $doc->loadXml(self::$_subQuery);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addSubQuery('TestSys1', $queryNode);

    }//end addSubQuery()


    /**
     * Returns the added sub-query node for use in the tests.
     *
     * @return DomElement
     */
    public function getSubQueryNode()
    {
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $subQueryNode = $doc->getElementsByTagName('sub-queries')->item(0);
        $subQuery     = $subQueryNode->getElementsByTagName('query')->item(0);
        return $subQuery;

    }//end getSubQueryNode()


    /**
     * Tests that after being added the test fragment is the only one present.
     *
     * @return void
     */
    public function testAddSubQueryCount()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $subQueryNode = $doc->getElementsByTagName('sub-queries')->item(0);
        $subQueries   = $subQueryNode->getElementsByTagName('query');
        PHPUnit_Framework_Assert::assertEquals(1, $subQueries->length);

    }//end testAddSubQueryCount()


    /**
     * Adds a base query used by the tests.
     *
     * @return void
     */
    public function testAddSubQuerySystemAttribute()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $subQuery   = $this->getSubQueryNode();
        $systemName = $subQuery->getAttribute('system');
        PHPUnit_Framework_Assert::assertEquals('TestSys1', $systemName);

    }//end testAddSubQuerySystemAttribute()


    /**
     * Tests the right number of bindings are created for the base query.
     *
     * @return void
     */
    public function testQueryBindingCount()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $doc         = DALBaker::getQueryXml('TestSys1', 'testone');
        $bindingList = $doc->getElementsByTagName('binding');
        PHPUnit_Framework_Assert::assertEquals(4, $bindingList->length);

    }//end testQueryBindingCount()


    /**
     * Tests that the variable names for the bindings are correct.
     *
     * @return void
     */
    public function testQueryBindingNames()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $bindingList  = $doc->getElementsByTagName('binding');
        $bindindNames = array();
        foreach ($bindingList as $binding) {
            $bindingNames[] = $binding->getAttribute('name');
        }

        $expected = array(
                     ':LinkTableParentid',
                     ':PermTableUserid',
                     ':accessLevel',
                     ':AssetTableTypeCode',
                    );

        PHPUnit_Framework_Assert::assertEquals($bindingNames, $expected);

    }//end testQueryBindingNames()


    /**
     * Tests that the attributes for the bindings are correct.
     *
     * @return void
     */
    public function testQueryBindingAttributes()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $bindingList  = $doc->getElementsByTagName('binding');
        $bindingAttrs = array();
        foreach ($bindingList as $binding) {
            $bindingName   = $binding->getAttribute('name');
            $bindingTable  = $binding->getAttribute('table');
            $bindingColumn = $binding->getAttribute('column');
            $bindingSystem = $binding->getAttribute('system');

            $bindingAttrs[$bindingName] = array(
                                           'table'  => $bindingTable,
                                           'column' => $bindingColumn,
                                           'system' => $bindingSystem,
                                          );
        }

        $expected = array(
                     ':LinkTableParentid'  => array (
                                               'table'  => 'link_table',
                                               'column' => 'parentid',
                                               'system' => 'TestSys1',
                                              ),
                     ':PermTableUserid'    => array (
                                               'table'  => 'perm_table',
                                               'column' => 'userid',
                                               'system' => 'TestSys1',
                                              ),
                     ':accessLevel'        => array (
                                               'table'  => 'perm_table',
                                               'column' => 'access',
                                               'system' => 'TestSys1',
                                              ),
                     ':AssetTableTypeCode' => array (
                                               'table'  => 'asset_table',
                                               'column' => 'type_code',
                                               'system' => 'TestSys1',
                                              ),
                    );

        PHPUnit_Framework_Assert::assertEquals($bindingAttrs, $expected);

    }//end testQueryBindingAttributes()


    /**
     * Tests that the variable names for the bindings are correct.
     *
     * @return void
     */
    public function testSubQueryBindingNames()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $subQuery     = $this->getSubQueryNode();
        $bindingList  = $subQuery->getElementsByTagName('binding');
        $bindindNames = array();
        foreach ($bindingList as $binding) {
            $bindingNames[] = $binding->getAttribute('name');
        }

        $expected = array(
                     ':AssetTableTypeCode',
                    );

        PHPUnit_Framework_Assert::assertEquals($bindingNames, $expected);

    }//end testSubQueryBindingNames()


    /**
     * Tests the right number of bindings are created for the fragment.
     *
     * @return void
     */
    public function testSubQueryBindingCount()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $subQuery    = $this->getSubQueryNode();
        $bindingList = $subQuery->getElementsByTagName('binding');
        PHPUnit_Framework_Assert::assertEquals(1, $bindingList->length);

    }//end testSubQueryBindingCount()


    /**
     * Tests that placeholders have the correct names and values.
     *
     * @return void
     */
    public function testSubQueryPlaceHolderValues()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $subQuery        = $this->getsubQueryNode();
        $placeHolderList = $subQuery->getElementsByTagName('placeholder');
        $placeHolders    = array();
        foreach ($placeHolderList as $placeHolder) {
            $varName                = $placeHolder->getAttribute('var_name');
            $varValue               = $placeHolder->getAttribute('value');
            $placeHolders[$varName] = $varValue;
        }

        $expected = array(
                     ':AssetTableTypeCode' => 'page',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $placeHolders);

    }//end testSubQueryPlaceHolderValues()


    /**
     * Tests the right number of place holders are created for the fragment.
     *
     * @return void
     */
    public function testSubQueryPlaceHolderCount()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $subQuery        = $this->getSubQueryNode();
        $placeHolderList = $subQuery->getElementsByTagName('placeholder');
        PHPUnit_Framework_Assert::assertEquals($placeHolderList->length, 1);

    }//end testSubQueryPlaceHolderCount()


    /**
     * Tests the right number of placeholders are created for the whole query.
     *
     * @return void
     */
    public function testQueryPlaceholderCount()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $doc             = DALBaker::getQueryXml('TestSys1', 'testone');
        $placeHolderList = $doc->getElementsByTagName('placeholder');
        PHPUnit_Framework_Assert::assertEquals(3, $placeHolderList->length);

    }//end testQueryPlaceholderCount()


}//end class

?>
