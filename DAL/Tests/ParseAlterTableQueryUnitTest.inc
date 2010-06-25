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
 * Unit tests for the parsing and validation of ALTER TABLE queries.
 *
 * @since 4.0.0
 */
class ParseAlterTableQueryUnitTest extends AbstractMySourceUnitTest
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
     * Test that add-column returns the correct array values.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
