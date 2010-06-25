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
 * Unit Tests for the merging of queries.
 *
 * @since 4.0.0
 */
class QueryBakingUnitTest extends AbstractMySourceUnitTest
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
     * Test the class of a merged file exists.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMergedQueryClass()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        DAL::requireQueryClass('TestSys1');
        PHPUnit_Framework_Assert::assertTrue(class_exists('TestSys1Queries'));

    }//end testMergedQueryClass()


    /**
     * Test there is only one method in the merged file.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMergedQueryMethodsCount()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        DAL::requireQueryClass('TestSys1');
        $queryClass = new ReflectionClass('TestSys1Queries');
        $methods    = $queryClass->getMethods();
        PHPUnit_Framework_Assert::assertEquals(1, count($methods));

    }//end testMergedQueryMethodsCount()


    /**
     * Test the methods of a merged file are named correctly.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMergedQueryMethodName()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        DAL::requireQueryClass('TestSys1');
        $queryClass = new ReflectionClass('TestSys1Queries');
        $methods    = $queryClass->getMethods();
        $methodName = $methods[0]->name;
        $expected   = 'prepareTestoneQuery';
        PHPUnit_Framework_Assert::assertEquals($methodName, $expected);

    }//end testMergedQueryMethodName()


    /**
     * Test the parameter of the baked method has only 2 arguments.
     *
     * The first is the $params array, and the second is a flag for returning
     * an array, or binding the query.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMergedQueryMethodArgumentCount()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        DAL::requireQueryClass('TestSys1');
        $queryClass = new ReflectionClass('TestSys1Queries');
        $methods    = $queryClass->getMethods();
        $method     = $methods[0];
        $parameters = $method->getParameters();
        PHPUnit_Framework_Assert::assertEquals(2, count($parameters));

    }//end testMergedQueryMethodArgumentCount()


    /**
     * Test the parameter of the baked method is named correctly.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMergedQueryMethodArgumentName()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        DAL::requireQueryClass('TestSys1');
        $queryClass = new ReflectionClass('TestSys1Queries');
        $methods    = $queryClass->getMethods();
        $method     = $methods[0];
        $parameters = $method->getParameters();
        $paramName  = $parameters[0]->name;
        $expected   = 'data';
        PHPUnit_Framework_Assert::assertEquals($expected, $paramName);

    }//end testMergedQueryMethodArgumentName()


    /**
     * Test the parameter of the baked method has a type-hint.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMergedQueryMethodArgumentTypeHint()
    {
        $this->addbaseQuery();
        $this->addQueryFragment();
        DALBaker::bakeQueriesFile('TestSys1');
        $classFile = 'TestSys1Queries.inc';
        $path      = DAL::getOvenPath('TestSys1').'/'.$classFile;
        $contents  = file_get_contents($path);
        $typeHints = array();
        preg_match('/\(([a-zA-Z]*)\s+\$data,/i', $contents, $typeHints);
        $typeHint = $typeHints[1];

        PHPUnit_Framework_Assert::assertEquals('array', $typeHint);

    }//end testMergedQueryMethodArgumentTypeHint()


}//end class

?>
