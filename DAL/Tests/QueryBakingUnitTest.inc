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
 * Unit Tests for the merging of queries.
 *
 */
class QueryBakingUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Base Query XML.
     *
     * @var   string $_baseQuery Base query XML.
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
     * Test the class of a merged file exists.
     *
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
