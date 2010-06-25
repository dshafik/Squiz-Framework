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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit Tests for the getArray() method of DAL.
 *
 * @since 4.0.0
 */
class PrepareGetArrayUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Base Query XML.
     *
     * @var   string $_baseQuery Base query XML.
     * @since 4.0.0
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
     * @since 4.0.0
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
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * These tests need to be baked separately.
     *
     * @since  4.0.0
     * @return boolean.
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Adds a base query used by the tests.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
