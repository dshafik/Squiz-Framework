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
 * Unit tests for the bakeQueryMethod() method.
 *
 * @since 4.0.0
 */
class BakeQueryMethodUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Base query for testing the addSubQuery() method.
     *
     * @var   string $_baseQuery The XML source of a test query.
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
     * @since 4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * Returns the added method code for the query.
     *
     * @since  4.0.0
     * @return DomElement
     */
    public function getMethodCode()
    {
        $methodCode = DALBaker::bakeQueryMethod('TestSys1', 'testone');
        return $methodCode;

    }//end getMethodCode()


    /**
     * Tests that the method signature is correct in the baked method.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMethodSignature()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $code     = $this->getMethodCode();
        $ret      = strpos($code, 'public static function prepareTestoneQuery(array $data)');
        $expected = 0;
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testMethodSignature()


    /**
     * Tests that sub query code has the right amount of bindings.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMethodBindCount()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $code    = $this->getMethodCode();
        $matches = array();
        preg_match_all('/\$query-\>bind/i', $code, $matches);
        $ret      = count(current($matches));
        $expected = 4;
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testMethodBindCount()


    /**
     * Tests the assignment syntax for $LinkTableParentid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMethodLinkTableParentidAssignment()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $code    = $this->getMethodCode();
        $matches = array();
        $pos     = strpos($code, '$LinkTableParentid = \'123\';');
        $ret     = ($pos === FALSE) ? FALSE : TRUE;
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testMethodLinkTableParentidAssignment()


    /**
     * Tests the assignment syntax for $PermTableUserid.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMethodPermTableUseridAssignment()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $code    = $this->getMethodCode();
        $matches = array();
        $pos     = strpos($code, '$PermTableUserid = \'10\';');
        $ret     = ($pos === FALSE) ? FALSE : TRUE;
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testMethodPermTableUseridAssignment()


    /**
     * Tests the assignment syntax for $AssetTableTypeCode.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMethodAssetTableTypeCodeAssignment()
    {
        $this->addBaseQuery();
        $this->addSubQuery();
        $code    = $this->getMethodCode();
        $matches = array();
        $pos     = strpos($code, '$AssetTableTypeCode = \'page\';');
        $ret     = ($pos === FALSE) ? FALSE : TRUE;
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testMethodAssetTableTypeCodeAssignment()


}//end class

?>
