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
 * Unit tests for the parsing and conversion of math operations.
 */
class MathOperationConversionUnitTest extends AbstractMySourceUnitTest
{


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
     * Returns the converter class for given database type.
     *
     * @param string $dbType Type of the database.
     *
     * @return object
     */
    private function _getConverterClass($dbType)
    {
        $converterClass = 'DAL'.$dbType.'Converter';

        $path = dirname(dirname(__FILE__)).'/Converters/'.$converterClass.'.inc';
        require_once $path;
        // Here we will get the current DB type and use its converter class.
        $converter = eval("return new DAL$dbType".'Converter();');
        return $converter;

    }//end _getConverterClass()


    /**
     * Checks that expected results are same as the converted results.
     *
     * @param array $expected Expected results.
     * @param array $parsed   Parsed SQL array.
     *
     * @return void
     */
    private function _testQuery(array $expected, array $parsed)
    {
        $msg = 'Alter table returned different SQL string for database: ';
        foreach ($expected as $dbType => $result) {
            $converter = $this->_getConverterClass($dbType);
            $sql       = $converter->convertToSql($parsed['query']);
            $sql       = base64_encode($sql);
            PHPUnit_Framework_Assert::assertEquals($result, $sql, $msg.$dbType."\n");
        }

    }//end _testQuery()


    /**
     * Test math operations in SELECT queries.
     *
     * Test that math operations are parsed and converted correctly when they
     * are in SELECT query.
     *
     * @return void
     */
    public function testMathOpInSelectFields()
    {
        $xml = '<query>
                    <select>
                        <from>
                             <table>table</table>
                        </from>
                            <fields>
                                <field table="table" column="assetid" />
                                <math-op>
                                    <subtract>
                                        <value>10</value>
                                        <value>5</value>
                                        <add>
                                            <value>20</value>
                                            <field table="table" column="b" />
                                        </add>
                                        <field table="table" column="b" />
                                    </subtract>
                                </math-op>
                            </fields>
                    </select>
                </query>';
        try {
            $queryXml = new DOMDocument();
            $queryXml->loadXML($xml);
            $select   = $queryXml->getElementsByTagName('query')->item(0);
            $expected = 'a:2:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:2:{i:0;a:2:{s:5:"table";s:5:"table";s:6:"column";s:7:"assetid";}i:1;a:1:{s:7:"MATH-OP";a:1:{i:0;a:1:{s:8:"SUBTRACT";a:4:{i:0;s:2:"10";i:1;s:1:"5";i:2;a:1:{s:3:"ADD";a:2:{i:0;s:2:"20";i:1;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}i:3;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}}}}}s:4:"FROM";a:1:{i:0;s:5:"table";}}';
            $parsed   = DALBaker::constructSql($select);

            PHPUnit_Framework_Assert::assertEquals($expected, serialize($parsed['query']));

            $expected = array(
                         'Mysql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkLCAoMTAgLSA1IC0gKDIwICsgdGFibGUuYikgLSB0YWJsZS5iKQpGUk9NCiAgICB0YWJsZQo=',
                         'Postgres' => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkLCAoMTAgLSA1IC0gKDIwICsgdGFibGUuYikgLSB0YWJsZS5iKQpGUk9NCiAgICB0YWJsZQo=',
                         'Oracle'   => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkLCAoMTAgLSA1IC0gKDIwICsgdGFibGUuYikgLSB0YWJsZS5iKQpGUk9NCiAgICB0YWJsZQo=',
                         'Db2'      => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkLCAoMTAgLSA1IC0gKDIwICsgdGFibGUuYikgLSB0YWJsZS5iKQpGUk9NCiAgICB0YWJsZQo=',
                         'Mssql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkLCAoMTAgLSA1IC0gKDIwICsgdGFibGUuYikgLSB0YWJsZS5iKQpGUk9NCiAgICB0YWJsZQo=',
                        );

            $this->_testQuery($expected, $parsed);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testMathOpInSelectFields()


    /**
     * Test math operations in WHERE conditions.
     *
     * Test that math operations are parsed and converted correctly for each db
     * type when they are in WHERE conditions.
     *
     * @return void
     */
    public function testMathOpInWhereConditions()
    {
        $xml = '<query>
                    <select>
                        <fields>
                            <field table="table" column="assetid" />
                        </fields>
                        <from>
                             <table>table</table>
                        </from>
                        <where>
                            <equal table="test" column="a">
                                <math-op>
                                    <subtract>
                                        <value>10</value>
                                        <value>5</value>
                                        <add>
                                            <value>20</value>
                                            <multiply>
                                                <value>2</value>
                                                <value>8</value>
                                            </multiply>
                                            <field table="table" column="b" />
                                        </add>
                                        <field table="table" column="b" />
                                    </subtract>
                                </math-op>
                            </equal>
                        </where>
                    </select>
                </query>';

        try {
            $queryXml = new DOMDocument();
            $queryXml->loadXML($xml);
            $select   = $queryXml->getElementsByTagName('query')->item(0);
            $expected = 'a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:1:{i:0;a:2:{s:5:"table";s:5:"table";s:6:"column";s:7:"assetid";}}}s:4:"FROM";a:1:{i:0;s:5:"table";}s:5:"WHERE";a:1:{s:3:"AND";a:1:{i:0;a:3:{s:7:"compare";a:2:{s:5:"table";s:4:"test";s:6:"column";s:1:"a";}s:4:"type";s:5:"equal";s:2:"to";a:1:{s:7:"MATH-OP";a:1:{i:0;a:1:{s:8:"SUBTRACT";a:4:{i:0;s:2:"10";i:1;s:1:"5";i:2;a:1:{s:3:"ADD";a:3:{i:0;s:2:"20";i:1;a:1:{s:8:"MULTIPLY";a:2:{i:0;s:1:"2";i:1;s:1:"8";}}i:2;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}i:3;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}}}}}}}';
            $parsed   = DALBaker::constructSql($select);

            PHPUnit_Framework_Assert::assertEquals($expected, serialize($parsed['query']));

            $expected = array(
                         'Mysql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIHRlc3QuYSA9ICgoMTAgLSA1IC0gKDIwICsgKDIgKiA4KSArIHRhYmxlLmIpIC0gdGFibGUuYikpCikK',
                         'Postgres' => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIHRlc3QuYSA9ICgoMTAgLSA1IC0gKDIwICsgKDIgKiA4KSArIHRhYmxlLmIpIC0gdGFibGUuYikpCikK',
                         'Oracle'   => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIHRlc3QuYSA9ICgoMTAgLSA1IC0gKDIwICsgKDIgKiA4KSArIHRhYmxlLmIpIC0gdGFibGUuYikpCikK',
                         'Db2'      => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIHRlc3QuYSA9ICgoMTAgLSA1IC0gKDIwICsgKDIgKiA4KSArIHRhYmxlLmIpIC0gdGFibGUuYikpCikK',
                         'Mssql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIHRlc3QuYSA9ICgoMTAgLSA1IC0gKDIwICsgKDIgKiA4KSArIHRhYmxlLmIpIC0gdGFibGUuYikpCikK',
                        );
            $this->_testQuery($expected, $parsed);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testMathOpInWhereConditions()


    /**
     * Test math operations in IN clauses.
     *
     * Test that math operations are parsed and converted correctly for each db
     * type when they are part of a IN condition.
     *
     * @return void
     */
    public function testMathOpInInCondition()
    {
        $xml = '<query>
                    <select>
                        <fields>
                            <field table="table" column="assetid" />
                        </fields>
                        <from>
                             <table>table</table>
                        </from>
                        <where>
                            <in table="asset" column="asset_type">
                                 <value>page</value>
                                 <value>user</value>
                                 <math-op>
                                    <multiply>
                                        <subtract>
                                            <value>10</value>
                                            <value>5</value>
                                            <field table="table" column="b" />
                                        </subtract>
                                        <add>
                                            <value>:numAssets</value>
                                            <value>:numParents</value>
                                        </add>
                                    </multiply>
                                </math-op>
                             </in>
                        </where>
                    </select>
                </query>';
        try {
            $queryXml = new DOMDocument();
            $queryXml->loadXML($xml);
            $select   = $queryXml->getElementsByTagName('query')->item(0);
            $expected = 'a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:1:{i:0;a:2:{s:5:"table";s:5:"table";s:6:"column";s:7:"assetid";}}}s:4:"FROM";a:1:{i:0;s:5:"table";}s:5:"WHERE";a:1:{s:3:"AND";a:1:{i:0;a:1:{s:2:"IN";a:3:{s:5:"table";s:5:"asset";s:6:"column";s:10:"asset_type";s:10:"CONDITIONS";a:3:{i:0;s:4:"page";i:1;s:4:"user";i:2;a:1:{s:7:"MATH-OP";a:1:{i:0;a:1:{s:8:"MULTIPLY";a:2:{i:0;a:1:{s:8:"SUBTRACT";a:3:{i:0;s:2:"10";i:1;s:1:"5";i:2;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}i:1;a:1:{s:3:"ADD";a:2:{i:0;s:10:":numAssets";i:1;s:11:":numParents";}}}}}}}}}}}}';
            $parsed   = DALBaker::constructSql($select);

            PHPUnit_Framework_Assert::assertEquals($expected, serialize($parsed['query']));

            $expected = array(
                         'Mysql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4ocGFnZSwgdXNlciwgKCgxMCAtIDUgLSB0YWJsZS5iKSAqICg6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Postgres' => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4ocGFnZSwgdXNlciwgKCgxMCAtIDUgLSB0YWJsZS5iKSAqICg6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Oracle'   => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4ocGFnZSwgdXNlciwgKCgxMCAtIDUgLSB0YWJsZS5iKSAqICg6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Db2'      => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4ocGFnZSwgdXNlciwgKCgxMCAtIDUgLSB0YWJsZS5iKSAqICg6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Mssql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4ocGFnZSwgdXNlciwgKCgxMCAtIDUgLSB0YWJsZS5iKSAqICg6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                        );

            $this->_testQuery($expected, $parsed);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testMathOpInInCondition()


    /**
     * Test math operations in a variety of structures.
     *
     * Test that math operations are parsed and converted correctly for each db
     * type when math-op has different value types like, function, value, field,
     * and hook.
     *
     * @return void
     */
    public function testMathOpWithDifferentValueTypes()
    {
        $xml = '<query>
                    <select>
                        <fields>
                            <field table="table" column="assetid" />
                        </fields>
                        <from>
                             <table>table</table>
                        </from>
                        <where>
                            <in table="asset" column="asset_type">
                                 <math-op>
                                    <multiply>
                                        <subtract>
                                            <value>5</value>
                                            <divide>
                                                <function function="cast">
                                                    <arg>23</arg>
                                                    <arg>
                                                        <field table="asset_attribute_int" column="value" />
                                                    </arg>
                                                </function>
                                                <field table="table" column="b" />
                                            </divide>
                                        </subtract>
                                        <add>
                                            <hook id="hookid1" />
                                            <value>:numAssets</value>
                                            <value>:numParents</value>
                                        </add>
                                    </multiply>
                                </math-op>
                             </in>
                        </where>
                    </select>
                </query>';
        try {
            $queryXml = new DOMDocument();
            $queryXml->loadXML($xml);
            $select   = $queryXml->getElementsByTagName('query')->item(0);
            $expected = 'a:3:{s:6:"SELECT";a:1:{s:6:"FIELDS";a:1:{i:0;a:2:{s:5:"table";s:5:"table";s:6:"column";s:7:"assetid";}}}s:4:"FROM";a:1:{i:0;s:5:"table";}s:5:"WHERE";a:1:{s:3:"AND";a:1:{i:0;a:1:{s:2:"IN";a:3:{s:5:"table";s:5:"asset";s:6:"column";s:10:"asset_type";s:10:"CONDITIONS";a:1:{i:0;a:1:{s:7:"MATH-OP";a:1:{i:0;a:1:{s:8:"MULTIPLY";a:2:{i:0;a:1:{s:8:"SUBTRACT";a:2:{i:0;s:1:"5";i:1;a:1:{s:6:"DIVIDE";a:2:{i:0;a:1:{s:8:"FUNCTION";a:2:{s:8:"function";s:4:"cast";s:4:"ARGS";a:2:{i:0;s:2:"23";i:1;a:2:{s:5:"table";s:19:"asset_attribute_int";s:6:"column";s:5:"value";}}}}i:1;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}}}i:1;a:1:{s:3:"ADD";a:3:{i:0;s:14:"HOOKID:hookid1";i:1;s:10:":numAssets";i:2;s:11:":numParents";}}}}}}}}}}}}';
            $parsed   = DALBaker::constructSql($select);

            PHPUnit_Framework_Assert::assertEquals($expected, serialize($parsed['query']));

            $expected = array(
                         'Mysql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4oKCg1IC0gKENBU1QoMjMgQVMgYXNzZXRfYXR0cmlidXRlX2ludC52YWx1ZSkgLyB0YWJsZS5iKSkgKiAoSE9PS0lEOmhvb2tpZDEgKyA6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Postgres' => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4oKCg1IC0gKENBU1QoMjMgQVMgYXNzZXRfYXR0cmlidXRlX2ludC52YWx1ZSkgLyB0YWJsZS5iKSkgKiAoSE9PS0lEOmhvb2tpZDEgKyA6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Oracle'   => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4oKCg1IC0gKENBU1QoMjMgQVMgYXNzZXRfYXR0cmlidXRlX2ludC52YWx1ZSkgLyB0YWJsZS5iKSkgKiAoSE9PS0lEOmhvb2tpZDEgKyA6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Db2'      => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4oKCg1IC0gKENBU1QoMjMgQVMgYXNzZXRfYXR0cmlidXRlX2ludC52YWx1ZSkgLyB0YWJsZS5iKSkgKiAoSE9PS0lEOmhvb2tpZDEgKyA6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                         'Mssql'    => 'U0VMRUNUCiAgICB0YWJsZS5hc3NldGlkCkZST00KICAgIHRhYmxlCldIRVJFCigKICAgIGFzc2V0LmFzc2V0X3R5cGUgSU4oKCg1IC0gKENBU1QoMjMgQVMgYXNzZXRfYXR0cmlidXRlX2ludC52YWx1ZSkgLyB0YWJsZS5iKSkgKiAoSE9PS0lEOmhvb2tpZDEgKyA6bnVtQXNzZXRzICsgOm51bVBhcmVudHMpKSkKKQo=',
                        );

            $this->_testQuery($expected, $parsed);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testMathOpWithDifferentValueTypes()


    /**
     * Test Math Operations in Update Queries.
     *
     * Test that math operations are parsed and converted correctly when they
     * are in UPDATE query.
     *
     * @return void
     */
    public function testMathOpInUpdateQuery()
    {
        $xml = '<query>
                    <update>
                        <fields table="link_table">
                            <field>parentid</field>
                            <field>childid</field>
                        </fields>
                        <values>
                            <value column="parentid">
                                <math-op>
                                    <subtract>
                                        <value>5</value>
                                        <add>
                                            <value>:numAssets</value>
                                            <value>:numParents</value>
                                        </add>
                                        <field table="table" column="b" />
                                    </subtract>
                                </math-op>
                            </value>
                            <value column="childid">234</value>
                        </values>
                    </update>
                </query>';
        try {
            $queryXml = new DOMDocument();
            $queryXml->loadXML($xml);
            $select   = $queryXml->getElementsByTagName('query')->item(0);
            $expected = 'a:1:{s:6:"UPDATE";a:3:{s:5:"table";s:10:"link_table";s:6:"FIELDS";a:2:{i:0;s:8:"parentid";i:1;s:7:"childid";}s:6:"VALUES";a:2:{s:8:"parentid";a:1:{s:7:"MATH-OP";a:1:{i:0;a:1:{s:8:"SUBTRACT";a:3:{i:0;s:1:"5";i:1;a:1:{s:3:"ADD";a:2:{i:0;s:10:":numAssets";i:1;s:11:":numParents";}}i:2;a:2:{s:5:"table";s:5:"table";s:6:"column";s:1:"b";}}}}}s:7:"childid";s:3:"234";}}}';
            $parsed   = DALBaker::constructSql($select);

            PHPUnit_Framework_Assert::assertEquals($expected, serialize($parsed['query']));

            $expected = array(
                         'Mysql'    => 'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9ICg1IC0gKDpudW1Bc3NldHMgKyA6bnVtUGFyZW50cykgLSB0YWJsZS5iKSwKICAgIGNoaWxkaWQgPSAyMzQ=',
                         'Postgres' => 'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9ICg1IC0gKDpudW1Bc3NldHMgKyA6bnVtUGFyZW50cykgLSB0YWJsZS5iKSwKICAgIGNoaWxkaWQgPSAyMzQ=',
                         'Oracle'   => 'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9ICg1IC0gKDpudW1Bc3NldHMgKyA6bnVtUGFyZW50cykgLSB0YWJsZS5iKSwKICAgIGNoaWxkaWQgPSAyMzQ=',
                         'Db2'      => 'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9ICg1IC0gKDpudW1Bc3NldHMgKyA6bnVtUGFyZW50cykgLSB0YWJsZS5iKSwKICAgIGNoaWxkaWQgPSAyMzQ=',
                         'Mssql'    => 'VVBEQVRFIGxpbmtfdGFibGUKU0VUCiAgICBwYXJlbnRpZCA9ICg1IC0gKDpudW1Bc3NldHMgKyA6bnVtUGFyZW50cykgLSB0YWJsZS5iKSwKICAgIGNoaWxkaWQgPSAyMzQ=',
                        );

            $this->_testQuery($expected, $parsed);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testMathOpInUpdateQuery()


}//end class

?>
