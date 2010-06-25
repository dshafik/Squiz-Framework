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
 * Unit tests for DROP TABLE query conversion.
 *
 * @since 4.0.0
 */
class DropTableQueryUnitTest extends AbstractMySourceUnitTest
{


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
     * Returns the converter class for given database type.
     *
     * @param string $dbType Type of the database.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
