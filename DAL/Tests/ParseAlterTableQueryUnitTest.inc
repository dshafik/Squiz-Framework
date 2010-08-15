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
 * Unit tests for the parsing and validation of ALTER TABLE queries.
 */
class ParseAlterTableQueryUnitTest extends AbstractMySourceUnitTest
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
     * Test that add-column returns the correct array values.
     *
     * @return void
     */
    public function testParsedAddColumnQuery()
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
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array(
                     'ALTER' => array(
                                 'table'      => 'table_name',
                                 'ADD-COLUMN' => array(
                                                  'COLUMNS' => array(
                                                                0 => array(
                                                                      'type'       => 'VARCHAR',
                                                                      'size'       => '255',
                                                                      'scale'      => '',
                                                                      'allow-null' => 'false',
                                                                      'default'    => '',
                                                                      'name'       => 'name',
                                                                     ),
                                                                1 => array (
                                                                      'type'       => 'VARCHAR',
                                                                      'size'       => '255',
                                                                      'scale'      => '',
                                                                      'allow-null' => 'false',
                                                                      'default'    => '',
                                                                      'name'       => 'name2',
                                                                     ),
                                                               ),
                                                 ),
                                ),
                    );
        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedAddColumnQuery()


    /**
     * Test that modify-column returns the correct array values.
     *
     * @return void
     */
    public function testParsedModifyColumnQuery()
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
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array(
                     'ALTER' => array(
                                 'table'         => 'table_name',
                                 'MODIFY-COLUMN' => array(
                                                     'COLUMNS' => array(
                                                                   0 => array(
                                                                         'type'       => 'VARCHAR',
                                                                         'size'       => '255',
                                                                         'scale'      => '',
                                                                         'allow-null' => 'false',
                                                                         'default'    => '',
                                                                         'name'       => 'name',
                                                                        ),
                                                                  ),
                                                    ),
                                ),
                    );

        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedModifyColumnQuery()


    /**
     * Test that rename-column returns the correct array values.
     *
     * @return void
     */
    public function testParsedRenameColumnQuery()
    {
        $xml = '<query><alter table="table_name">
                    <rename-column old="old_column_name">new_column_name</rename-column>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array (
                     'ALTER' => array(
                                 'table'         => 'table_name',
                                 'RENAME-COLUMN' => array(
                                                     'OLD' => 'old_column_name',
                                                     'NEW' => 'new_column_name',
                                                    ),
                                ),
                    );

        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedRenameColumnQuery()


    /**
     * Test that drop-column returns the correct array values.
     *
     * @return void
     */
    public function testParsedDropColumnQuery()
    {
        $xml = '<query><alter table="table_name">
                    <drop-column >column_name</drop-column>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array(
                     'ALTER' => array(
                                 'table'       => 'table_name',
                                 'DROP-COLUMN' => 'column_name',
                                ),
                    );

        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedDropColumnQuery()


    /**
     * Test that add-constraint returns the correct array values.
     *
     * @return void
     */
    public function testParsedAddConstraintQuery()
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
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array(
                     'ALTER' => array(
                                 'table'          => 'table_name',
                                 'ADD-CONSTRAINT' => array(
                                                      'PRIMARY-KEYS' => array(
                                                                         0 => array(
                                                                               'name'    => 'constraint_name',
                                                                               'COLUMNS' => array(
                                                                                             0 => 'column_name',
                                                                                            ),
                                                                              ),
                                                                        ),
                                                     ),
                                ),
                    );

        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedAddConstraintQuery()


    /**
     * Test that drop-constraint returns the correct array values.
     *
     * @return void
     */
    public function testParsedDropConstraintQuery()
    {
        $xml = '<query><alter table="table_name">
                    <drop-constraint>constraint_name</drop-constraint>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array(
                     'ALTER' => array(
                                 'table'           => 'table_name',
                                 'DROP-CONSTRAINT' => 'constraint_name',
                                ),
                    );

        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedDropConstraintQuery()


    /**
     * Test that rename-table returns the correct array values.
     *
     * @return void
     */
    public function testParsedRenameTableQuery()
    {
        $xml = '<query><alter table="table_name">
                    <rename-table old="old_table_name">new_table_name</rename-table>
                </alter></query>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $alter = $queryXml->getElementsByTagName('query')->item(0);

        $expected = array (
                     'ALTER' => array(
                                 'table'        => 'table_name',
                                 'RENAME-TABLE' => array(
                                                    'OLD' => 'old_table_name',
                                                    'NEW' => 'new_table_name',
                                                   ),
                                ),
                    );

        $retVal = DALBaker::constructSql($alter);
        $retVal = $retVal['query'];

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testParsedRenameTableQuery()


}//end class

?>
