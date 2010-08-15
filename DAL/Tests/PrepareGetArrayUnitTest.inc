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
 * Unit Tests for the getArray() method of DAL.
 */
class PrepareGetArrayUnitTest extends AbstractMySourceUnitTest
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
                                        </from>
                                        <where>
                                            <equal table="link_table" column="parentid">123</equal>
                                            <equal table="perm_table" column="userid">10</equal>
                                            <equal table="perm_table" column="access">:accessLevel</equal>
                                        </where>
                                      </select>
                                  </primary>
                                  </query>';

    /**
     * Fragment for use in the tests.
     *
     * @var   string $_queryFragment The XML source of a test query.
     */
    private static $_queryFragment = '<query id="testoneFragment" hookid="TestSys1.testone">
                                     <primary>
                                     <select>
                                        <from>
                                            <table>asset_table</table>
                                        </from>
                                        <where>
                                            <equal table="asset_table" column="type_code">page</equal>
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
        return array();

    }//end getRequiredSystems()


    /**
     * These tests need to be baked separately.
     *
     * @return boolean.
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Adds a base query used by the tests.
     *
     * @return boolean
     */
    public function addBaseQuery()
    {
        $doc = new DomDocument();
        $doc->loadXml(self::$_baseQuery);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        return DALBaker::addQuery('TestSys1', $queryNode);

    }//end addBaseQuery()


    /**
     * Adds a fragment used for the tests.
     *
     * @return void
     */
    public function addQueryFragment()
    {
        $doc = new DomDocument();
        $doc->loadXml(self::$_queryFragment);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addQueryFragment('TestSys1', $queryNode);

    }//end addQueryFragment()


    /**
     * Tests the bindings returned are correct.
     *
     * @return void
     */
    public function testBindingStructure()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        Channels::createEventBasket();
        Channels::addToBasket('accessLevel', 'abc');
        $query = DAL::getDALQuery('TestSys1', 'testone', 'getAssoc');
        Channels::removeEventBasket();
        $bindings = $query->getBindings();
        $expected = array(
                     0 => array(
                           'name' => ':bind_1',
                           'value' => 'abc',
                           'type'  => PDO::PARAM_STR,
                          ),
                    );
        Channels::removeEventBasket();

        PHPUnit_Framework_Assert::assertEquals($expected, $bindings);

    }//end testBindingStructure()


    /**
     * Tests the SQL returned in the array.
     *
     * @return void
     */
    public function testGetQueryArraySqlContent()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        Channels::createEventBasket();
        Channels::addToBasket('accessLevel', 'abc');
        $query = DAL::getDALQuery('TestSys1', 'testone', 'getAssoc');
        Channels::removeEventBasket();
        $sql      = preg_replace('/\s+/i', ' ', $query->getSql());
        $expected = 'SELECT link_table.childid as id, asset_table.type_code as type FROM link_table, perm_table WHERE ( link_table.parentid = (123) AND perm_table.userid = (10) AND perm_table.access = (:bind_1) ) ';

        PHPUnit_Framework_Assert::assertEquals($expected, $sql);

    }//end testGetQueryArraySqlContent()


    /**
     * Tests the method in the returned array is stored correctly.
     *
     * @return void
     */
    public function testGetQueryArrayMethod()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        Channels::createEventBasket();
        Channels::addToBasket('accessLevel', 'abc');
        $query = DAL::getDALQuery('TestSys1', 'testone', 'getAssoc');
        Channels::removeEventBasket();
        $method   = $query->getExecMethod();
        $expected = 'getAssoc';

        PHPUnit_Framework_Assert::assertEquals($expected, $method);

    }//end testGetQueryArrayMethod()


}//end class

?>
