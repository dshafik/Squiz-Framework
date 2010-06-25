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
 * Unit tests for the addQueryFragment method.
 *
 * @since 4.0.0
 */
class AddQueryFragmentUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Base query for testing the addQueryFragment() method.
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
     * Fragment for use in the tests.
     *
     * @var   string $_queryFragment The XML source of a test query.
     * @since 4.0.0
     */
    private static $_queryFragment = '<query id="testoneFragment" hookid="TestSys1.testone">
                                     <select>
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
    public function addQueryFragment()
    {
        $doc = new DomDocument();
        $doc->loadXml(self::$_queryFragment);
        $queryNode = $doc->getElementsByTagName('query')->item(0);
        DALBaker::addQueryFragment('TestSys1', $queryNode);

    }//end addQueryFragment()


    /**
     * Returns the added fragment node for use in the tests.
     *
     * @since  4.0.0
     * @return DomElement
     */
    public function getFragmentNode()
    {
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $fragmentNode = $doc->getElementsByTagName('fragments')->item(0);
        $fragment     = $fragmentNode->getElementsByTagName('query')->item(0);
        return $fragment;

    }//end getFragmentNode()


    /**
     * Tests that after being added the test fragment is the only one present.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddFragmentCount()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $doc          = DALBaker::getQueryXml('TestSys1', 'testone');
        $fragmentNode = $doc->getElementsByTagName('fragments')->item(0);
        $fragments    = $fragmentNode->getElementsByTagName('query');
        PHPUnit_Framework_Assert::assertEquals(1, $fragments->length);

    }//end testAddFragmentCount()


    /**
     * Adds a base query used by the tests.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddFragmentSystemAttribute()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $fragment   = $this->getFragmentNode();
        $systemName = $fragment->getAttribute('system');
        PHPUnit_Framework_Assert::assertEquals('TestSys1', $systemName);

    }//end testAddFragmentSystemAttribute()


    /**
     * Tests the right number of placeholders are created for the base query.
     *
     * @since  4.0.0
     * @return void
     */
    public function testQueryPlaceholderCount()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $doc             = DALBaker::getQueryXml('TestSys1', 'testone');
        $placeHolderList = $doc->getElementsByTagName('placeholder');
        PHPUnit_Framework_Assert::assertEquals(3, $placeHolderList->length);

    }//end testQueryPlaceholderCount()


    /**
     * Tests the right number of bindings are created for the base query.
     *
     * @since  4.0.0
     * @return void
     */
    public function testQueryBindingCount()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $doc         = DALBaker::getQueryXml('TestSys1', 'testone');
        $bindingList = $doc->getElementsByTagName('binding');
        PHPUnit_Framework_Assert::assertEquals(4, $bindingList->length);

    }//end testQueryBindingCount()


    /**
     * Tests that the variable names for the bindings are correct.
     *
     * @since  4.0.0
     * @return void
     */
    public function testQueryBindingNames()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
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
     * @since  4.0.0
     * @return void
     */
    public function testQueryBindingAttributes()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
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
     * @since  4.0.0
     * @return void
     */
    public function testFragmentBindingNames()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $fragment     = $this->getFragmentNode();
        $bindingList  = $fragment->getElementsByTagName('binding');
        $bindindNames = array();
        foreach ($bindingList as $binding) {
            $bindingNames[] = $binding->getAttribute('name');
        }

        $expected = array(
                     ':AssetTableTypeCode',
                    );

        PHPUnit_Framework_Assert::assertEquals($bindingNames, $expected);

    }//end testFragmentBindingNames()


    /**
     * Tests the right number of bindings are created for the fragment.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFragmentBindingCount()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $fragment    = $this->getFragmentNode();
        $bindingList = $fragment->getElementsByTagName('binding');
        PHPUnit_Framework_Assert::assertEquals(1, $bindingList->length);

    }//end testFragmentBindingCount()


    /**
     * Tests the right number of place holders are created for the fragment.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFragmentPlaceHolderCount()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $fragment        = $this->getFragmentNode();
        $placeHolderList = $fragment->getElementsByTagName('placeholder');
        PHPUnit_Framework_Assert::assertEquals($placeHolderList->length, 1);

    }//end testFragmentPlaceHolderCount()


    /**
     * Tests that placeholders have the correct names and values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testQueryPlaceHolderValues()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $doc             = DALBaker::getQueryXml('TestSys1', 'testone');
        $placeHolderList = $doc->getElementsByTagName('placeholder');
        $placeHolders    = array();
        foreach ($placeHolderList as $placeHolder) {
            $varName                = $placeHolder->getAttribute('var_name');
            $varValue               = $placeHolder->getAttribute('value');
            $placeHolders[$varName] = $varValue;
        }

        $expected = array(
                     ':LinkTableParentid'  => '123',
                     ':PermTableUserid'    => '10',
                     ':AssetTableTypeCode' => 'page',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $placeHolders);

    }//end testQueryPlaceHolderValues()


    /**
     * Tests that placeholders have the correct names and values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFragmentPlaceHolderValues()
    {
        $this->addBaseQuery();
        $this->addQueryFragment();
        $fragment        = $this->getFragmentNode();
        $placeHolderList = $fragment->getElementsByTagName('placeholder');
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

    }//end testFragmentPlaceHolderValues()


}//end class

?>
