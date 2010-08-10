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
 * Unit tests for DROP TABLE query conversion.
 *
 */
class DropTableQueryUnitTest extends AbstractMySourceUnitTest
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
     * Tests that validateDropQuery() throws exception if no table attr found.
     *
     * @return void
     */
    public function testInvalidDropQuery()
    {
        $xml = '<query id="dropAssetTypeTable">
                    <drop />
                </query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $drop   = $queryXml->getElementsByTagName('query')->item(0);
        $caught = FALSE;
        try {
            DALBaker::constructSql($drop);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testInvalidDropQuery()


    /**
     * Tests that validateDropQuery() doesn't throw an exception if attr found.
     *
     * @return void
     */
    public function testValidDropQuery()
    {
        $xml = '<query id="dropTestTable">
                    <drop table="test" />
                </query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $drop   = $queryXml->getElementsByTagName('query')->item(0);
        $caught = FALSE;
        try {
            DALBaker::constructSql($drop);
        } catch (DALParserException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertFalse($caught);

    }//end testValidDropQuery()


    /**
     * Test that DROP TABLE is converted correctly for each db type.
     *
     * @return void
     */
    public function testConvertedDropTableQuery()
    {
        $xml = '<query id="dropAssetTypeTable">
                    <drop table="asset_type" />
                </query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $drop     = $queryXml->getElementsByTagName('query')->item(0);
        $parsed   = DALBaker::constructSql($drop);
        $expected = array(
                     'Mysql'    => 'RFJPUCBUQUJMRSBJRiBFWElTVFMgYXNzZXRfdHlwZQ==',
                     'Postgres' => 'RFJPUCBUQUJMRSBhc3NldF90eXBl',
                     'Oracle'   => 'RFJPUCBUQUJMRSBhc3NldF90eXBl',
                     'Db2'      => 'RFJPUCBUQUJMRSBhc3NldF90eXBl',
                     'Mssql'    => 'RFJPUCBUQUJMRSBhc3NldF90eXBl',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedDropTableQuery()


}//end class

?>
