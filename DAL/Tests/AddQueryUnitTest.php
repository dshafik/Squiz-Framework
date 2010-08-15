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
 * Unit tests for the addQuery method.
 */
class AddQueryUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Base Query XML.
     *
     * @var string $_baseQuery Base query XML.
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
     * @return boolean
     */
    public function addBaseQuery()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());

        $doc = new DomDocument();
        $doc->loadXml(self::$_baseQuery);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        return DALBaker::addQuery('TestSys1', $queryNode);

    }//end addBaseQuery()


    /**
     * Tests that adding the base query results in only one present.
     *
     * @return void
     */
    public function testBaseQueryCount()
    {
        $this->addBaseQuery();
        $doc       = DALBaker::getQueryXml('TestSys1', 'testone');
        $queryList = $doc->getElementsByTagName('query');
        PHPUnit_Framework_Assert::assertEquals(1, $queryList->length);

    }//end testBaseQueryCount()


    /**
     * Tests that the system attribute is added to the base of the query.
     *
     * @return void
     */
    public function testBaseQuerySystemAttribute()
    {
        $this->addBaseQuery();
        $doc        = DALBaker::getQueryXml('TestSys1', 'testone');
        $queryList  = $doc->getElementsByTagName('query');
        $baseQuery  = $queryList->item(0);
        $systemName = $baseQuery->getAttribute('system');
        PHPUnit_Framework_Assert::assertEquals($systemName, 'TestSys1');

    }//end testBaseQuerySystemAttribute()


    /**
     * Tests the right number of placeholders are created for the base query.
     *
     * @return void
     */
    public function testBaseQueryPlaceholderCount()
    {
        $this->addBaseQuery();
        $doc             = DALBaker::getQueryXml('TestSys1', 'testone');
        $placeHolderList = $doc->getElementsByTagName('placeholder');
        PHPUnit_Framework_Assert::assertEquals(2, $placeHolderList->length);

    }//end testBaseQueryPlaceholderCount()


    /**
     * Tests the right number of bindings are created for the base query.
     *
     * @return void
     */
    public function testBaseQueryBindingCount()
    {
        $this->addBaseQuery();
        $doc         = DALBaker::getQueryXml('TestSys1', 'testone');
        $bindingList = $doc->getElementsByTagName('binding');
        PHPUnit_Framework_Assert::assertEquals(3, $bindingList->length);

    }//end testBaseQueryBindingCount()


    /**
     * Tests that the variable names for the bindings are correct.
     *
     * @return void
     */
    public function testQueryBindingNames()
    {
        $this->addBaseQuery();
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $bindingList  = $doc->getElementsByTagName('binding');
        $bindingNames = array();
        foreach ($bindingList as $binding) {
            $bindingNames[] = $binding->getAttribute('name');
        }

        $expected = array(
                     ':LinkTableParentid',
                     ':PermTableUserid',
                     ':accessLevel',
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
                     ':LinkTableParentid' => array (
                                              'table'  => 'link_table',
                                              'column' => 'parentid',
                                              'system' => 'TestSys1',
                                             ),
                     ':PermTableUserid'   => array (
                                              'table'  => 'perm_table',
                                              'column' => 'userid',
                                              'system' => 'TestSys1',
                                             ),
                     ':accessLevel'       => array (
                                              'table'  => 'perm_table',
                                              'column' => 'access',
                                              'system' => 'TestSys1',
                                             ),
                    );

        PHPUnit_Framework_Assert::assertEquals($bindingAttrs, $expected);

    }//end testQueryBindingAttributes()


    /**
     * Tests that placeholders have the correct names and values.
     *
     * @return void
     */
    public function testQueryPlaceHolderValues()
    {
        $this->addBaseQuery();
        $doc             = DALBaker::getQueryXml('TestSys1', 'testone');
        $placeHolderList = $doc->getElementsByTagName('placeholder');
        $placeHolders    = array();
        foreach ($placeHolderList as $placeHolder) {
            $varName                = $placeHolder->getAttribute('var_name');
            $varValue               = $placeHolder->getAttribute('value');
            $placeHolders[$varName] = $varValue;
        }

        $expected = array(
                     ':LinkTableParentid' => '123',
                     ':PermTableUserid'   => '10',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $placeHolders);

    }//end testQueryPlaceHolderValues()


    /**
     * Tests there are no subqueries when only the base is installed.
     *
     * @return void
     */
    public function testBaseQuerySubQueryCount()
    {
        $this->addBaseQuery();
        $doc     = DALBaker::getQueryXml('TestSys1', 'testone');
        $subNode = $doc->getElementsByTagName('sub-queries')->item(0);
        PHPUnit_Framework_Assert::assertEquals($subNode->childNodes->length, 0);

    }//end testBaseQuerySubQueryCount()


    /**
     * Tests there are no subqueries when only the base is installed.
     *
     * @return void
     */
    public function testBaseQueryFragmentCount()
    {
        $this->addBaseQuery();
        $doc      = DALBaker::getQueryXml('TestSys1', 'testone');
        $fragNode = $doc->getElementsByTagName('fragments')->item(0);
        PHPUnit_Framework_Assert::assertEquals($fragNode->childNodes->length, 0);

    }//end testBaseQueryFragmentCount()


    /**
     * Test that addQuery returns FALSE if there is a base query already.
     *
     * @return void
     */
    public function testMultipleBaseQueries()
    {
        $result = $this->addBaseQuery();
        PHPUnit_Framework_Assert::assertTrue($result);

        $doc = new DomDocument();
        $doc->loadXml(self::$_baseQuery);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        $result    = DALBaker::addQuery('TestSys1', $queryNode);

        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testMultipleBaseQueries()


}//end class

?>
