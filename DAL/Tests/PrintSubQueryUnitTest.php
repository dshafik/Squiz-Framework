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
 * Unit tests for the printSubQueries() method.
 *
 */
class PrintSubQueryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Base query for testing the addSubQuery() method.
     *
     * @var   string $_baseQuery The XML source of a test query.
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
     * @var   string $_subQuery The XML source of a test query.
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
    public function getSubQueryCode()
    {
        $hookCode = DALBaker::printSubQueries('TestSys1', 'testone');
        return $hookCode;

    }//end getSubQueryCode()


    /**
     * Tests that after evaluating the sub query code the hook array is set.
     *
     * @return void
     */
    public function testHookArrayIsset()
    {
        PHPUnit_Framework_Assert::assertTrue(TRUE);
//        $this->addBaseQuery();
//        $this->addSubQuery();
//        $query = new Query('', array(), '');
//        $code = $this->getSubQueryCode();
//        $sql  = '';
//        var_dump($code);
//        eval($code);
//        $set = isset($Hook1Array);
//        PHPUnit_Framework_Assert::assertTrue($set);

    }//end testHookArrayIsset()


    /**
     * Tests that after evaluating the hook array contains the right count.
     *
     * @return void
     */
    public function testHookArrayCount()
    {
        PHPUnit_Framework_Assert::assertTrue(TRUE);
//        $this->addBaseQuery();
//        $this->addSubQuery();
//        $query = new Query('', array(), '');
//        $code = $this->getSubQueryCode();
//        $sql  = '';
//        eval($code);
//        $ret      = count($Hook1Array);
//        $expected = 1;
//        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testHookArrayCount()


    /**
     * Tests that after evaluating the hook array contains the right count.
     *
     * @return void
     */
    public function testHookArrayContents()
    {
        PHPUnit_Framework_Assert::assertTrue(TRUE);
//        $this->addBaseQuery();
//        $this->addSubQuery();
//        $query = new Query('', array(), '');
//        $code = $this->getSubQueryCode();
//        $sql  = '';
//        eval($code);
//        $ret      = current($Hook1Array);
//        $ret      = preg_replace('/\s+/', ' ', $ret);
//        $expected = ' ( SELECT FROM asset_table WHERE asset_table.assetid = link_table.childid AND ( asset_table.type_code = :AssetTableTypeCode ) )';
//        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testHookArrayContents()


    /**
     * Tests that the hook placeholder is replaced.
     *
     * @return void
     */
    public function testHookContentsReplace()
    {
        PHPUnit_Framework_Assert::assertTrue(TRUE);
//        $this->addBaseQuery();
//        $this->addSubQuery();
//        $query = new Query('', array(), '');
//        $code = $this->getSubQueryCode();
//        $sql  = 'HOOKID:hook1';
//        eval($code);
//        $ret = strpos($sql, 'HOOKID:hook1');
//        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testHookContentsReplace()


    /**
     * Tests that the hook placeholder is replaced with an empty string.
     *
     * This is when there os no sub queries present for this hook.
     *
     * @return void
     */
    public function testHookContentsNotReplaced()
    {
        PHPUnit_Framework_Assert::assertTrue(TRUE);
//        $this->addBaseQuery();
//        $query = new Query('', array(), '');
//        $code     = $this->getSubQueryCode();
//        $sql      = 'HOOKID:hook1';
//        $expected = '';
//        eval($code);
//        PHPUnit_Framework_Assert::assertEquals($expected, $sql);

    }//end testHookContentsNotReplaced()


}//end class

?>
