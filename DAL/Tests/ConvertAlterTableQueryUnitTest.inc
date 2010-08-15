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
 * Unit tests for ALTER TABLE query conversion.
 */
class ConvertAlterTableQueryUnitTest extends AbstractMySourceUnitTest
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
     * Checks that expected results are same as the converted results.
     *
     * @param array $expected Expected results.
     * @param array $parsed   Parsed SQL array.
     *
     * @return void
     */
    private function _testQuery(array $expected, array $parsed)
    {
        $msg       = 'Alter table returned different SQL string for database: ';
        $converter = DALBaker::getConverter();
        $dbType    = $converter->getName();
        $sql       = $converter->convertToSql($parsed['query']);
        $sql       = base64_encode($sql);

        PHPUnit_Framework_Assert::assertEquals($expected[$dbType], $sql, $msg.$dbType."\n");

    }//end _testQuery()


    /**
     * Test that add-column returns the correct array values.
     *
     * @return void
     */
    public function testConvertedAddColumnQuery()
    {
        $xml = '<query><alter table="table_name">
                    <add-column>
                        <columns>
                            <column type="VARCHAR" size="255" allow-null="false" default="">name</column>
                            <column type="VARCHAR" size="255" allow-null="false" default="">name2</column>
                        </columns>
                    </add-column>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter    = $queryXml->getElementsByTagName('query')->item(0);
        $parsed   = DALBaker::constructSql($alter);
        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgKG5hbWUgVkFSQ0hBUigyNTUpIE5PVCBOVUxMLCBuYW1lMiBWQVJDSEFSKDI1NSkgTk9UIE5VTEwp',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgKG5hbWUgVkFSQ0hBUigyNTUpIE5PVCBOVUxMLCBuYW1lMiBWQVJDSEFSKDI1NSkgTk9UIE5VTEwp',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgKG5hbWUgTlZBUkNIQVIyKDI1NSkgTk9UIE5VTEwsIG5hbWUyIE5WQVJDSEFSMigyNTUpIE5PVCBOVUxMKQ==',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgKG5hbWUgVkFSQ0hBUigyNTUpIE5PVCBOVUxMLCBuYW1lMiBWQVJDSEFSKDI1NSkgTk9UIE5VTEwp',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgKG5hbWUgTlZBUkNIQVIoMjU1KSBOT1QgTlVMTCwgbmFtZTIgTlZBUkNIQVIoMjU1KSBOT1QgTlVMTCk=',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedAddColumnQuery()


    /**
     * Test that modify-column returns the correct array values.
     *
     * @return void
     */
    public function testConvertedModifyColumnQuery()
    {
        $xml = '<query><alter table="table_name">
                    <modify-column>
                        <columns>
                            <column type="VARCHAR" size="255" allow-null="false" default="">name</column>
                        </columns>
                    </modify-column>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter  = $queryXml->getElementsByTagName('query')->item(0);
        $parsed = DALBaker::constructSql($alter);

        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpNT0RJRlkgKG5hbWUgVkFSQ0hBUigyNTUpIE5PVCBOVUxMKQ==',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpNT0RJRlkgKG5hbWUgVkFSQ0hBUigyNTUpIE5PVCBOVUxMKQ==',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpNT0RJRlkgKG5hbWUgTlZBUkNIQVIyKDI1NSkgTk9UIE5VTEwp',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpNT0RJRlkgKG5hbWUgVkFSQ0hBUigyNTUpIE5PVCBOVUxMKQ==',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpNT0RJRlkgKG5hbWUgTlZBUkNIQVIoMjU1KSBOT1QgTlVMTCk=',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedModifyColumnQuery()


    /**
     * Test that rename-column returns the correct array values.
     *
     * @return void
     */
    public function testConvertedRenameColumnQuery()
    {
        $xml = '<query><alter table="table_name">
                    <rename-column old="old_column_name">new_column_name</rename-column>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter  = $queryXml->getElementsByTagName('query')->item(0);
        $parsed = DALBaker::constructSql($alter);

        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgQ09MVU1OIG9sZF9jb2x1bW5fbmFtZSBUTyBuZXdfY29sdW1uX25hbWU=',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgQ09MVU1OIG9sZF9jb2x1bW5fbmFtZSBUTyBuZXdfY29sdW1uX25hbWU=',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgQ09MVU1OIG9sZF9jb2x1bW5fbmFtZSBUTyBuZXdfY29sdW1uX25hbWU=',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgQ09MVU1OIG9sZF9jb2x1bW5fbmFtZSBUTyBuZXdfY29sdW1uX25hbWU=',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgQ09MVU1OIG9sZF9jb2x1bW5fbmFtZSBUTyBuZXdfY29sdW1uX25hbWU=',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedRenameColumnQuery()


    /**
     * Test that drop-column returns the correct array values.
     *
     * @return void
     */
    public function testConvertedDropColumnQuery()
    {
        $xml = '<query><alter table="table_name">
                    <drop-column >column_name</drop-column>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter  = $queryXml->getElementsByTagName('query')->item(0);
        $parsed = DALBaker::constructSql($alter);

        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIENPTFVNTiBjb2x1bW5fbmFtZQ==',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIENPTFVNTiBjb2x1bW5fbmFtZQ==',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIENPTFVNTiBjb2x1bW5fbmFtZQ==',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIENPTFVNTiBjb2x1bW5fbmFtZQ==',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIENPTFVNTiBjb2x1bW5fbmFtZQ==',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedDropColumnQuery()


    /**
     * Test that add-constraint returns the correct array values.
     *
     * @return void
     */
    public function testConvertedAddConstraintQuery()
    {
        $xml = '<query><alter table="table_name">
                    <add-constraint>
                        <primary-key name="constraint_name">
                            <column>column_name</column>
                        </primary-key>
                    </add-constraint>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter  = $queryXml->getElementsByTagName('query')->item(0);
        $parsed = DALBaker::constructSql($alter);

        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgQ09OU1RSQUlOVCBjb25zdHJhaW50X25hbWUgUFJJTUFSWSBLRVkgKGNvbHVtbl9uYW1lKTsK',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgQ09OU1RSQUlOVCBjb25zdHJhaW50X25hbWUgUFJJTUFSWSBLRVkgKGNvbHVtbl9uYW1lKQ==',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgQ09OU1RSQUlOVCBjb25zdHJhaW50X25hbWUgUFJJTUFSWSBLRVkgKGNvbHVtbl9uYW1lKQ==',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgQ09OU1RSQUlOVCBjb25zdHJhaW50X25hbWUgUFJJTUFSWSBLRVkgKGNvbHVtbl9uYW1lKQ==',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpBREQgQ09OU1RSQUlOVCBjb25zdHJhaW50X25hbWUgUFJJTUFSWSBLRVkgKGNvbHVtbl9uYW1lKTs=',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedAddConstraintQuery()


    /**
     * Test that drop-constraint returns the correct array values.
     *
     * @return void
     */
    public function testConvertedDropConstraintQuery()
    {
        $xml = '<query><alter table="table_name">
                    <drop-constraint>constraint_name</drop-constraint>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter  = $queryXml->getElementsByTagName('query')->item(0);
        $parsed = DALBaker::constructSql($alter);

        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIGNvbnN0cmFpbnRfbmFtZQ==',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIGNvbnN0cmFpbnRfbmFtZQ==',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIGNvbnN0cmFpbnRfbmFtZQ==',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIGNvbnN0cmFpbnRfbmFtZQ==',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpEUk9QIGNvbnN0cmFpbnRfbmFtZQ==',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedDropConstraintQuery()


    /**
     * Test that rename-table returns the correct array values.
     *
     * @return void
     */
    public function testConvertedRenameTableQuery()
    {
        $xml = '<query><alter table="table_name">
                    <rename-table old="old_table_name">new_table_name</rename-table>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter  = $queryXml->getElementsByTagName('query')->item(0);
        $parsed = DALBaker::constructSql($alter);

        $expected = array(
                     'MySQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgb2xkX3RhYmxlX25hbWUgVE8gbmV3X3RhYmxlX25hbWU=',
                     'Postgres' => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgb2xkX3RhYmxlX25hbWUgVE8gbmV3X3RhYmxlX25hbWU=',
                     'Oracle'   => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgb2xkX3RhYmxlX25hbWUgVE8gbmV3X3RhYmxlX25hbWU=',
                     'DB2'      => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgb2xkX3RhYmxlX25hbWUgVE8gbmV3X3RhYmxlX25hbWU=',
                     'MSSQL'    => 'QUxURVIgVEFCTEUgdGFibGVfbmFtZQpSRU5BTUUgb2xkX3RhYmxlX25hbWUgVE8gbmV3X3RhYmxlX25hbWU=',
                    );

        $this->_testQuery($expected, $parsed);

    }//end testConvertedRenameTableQuery()


}//end class

?>
