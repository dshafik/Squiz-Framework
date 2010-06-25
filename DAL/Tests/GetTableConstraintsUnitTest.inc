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
 * Unit tests for the getTableConstraints method.
 *
 * getTableConstraints method returns correct values.
 *
 * @since 4.0.0
 */
class GetTableConstraintsUnitTest extends AbstractMySourceUnitTest
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
     * Test getTableConstraints returns array() when table has no constraints.
     *
     * Note: table must have PK, validateConstrains() must be called
     * before calling getTableConstraints().
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoConstraints()
    {
        $xml = '<table name="test"></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array();
        $retVal   = DALSchemaParser::getTableConstraints($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testNoConstraints()


    /**
     * Tests getTableConstraints for table with primary key constraint.
     *
     * Test that getTableConstraints returns correct array elements when table
     * has primary key constraint.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPrimaryKeyConstraints()
    {
        $xml = '<table name="test"><constraints>
                    <primary-key name="asset_type_pk">
                        <column>type_code</column>
                    </primary-key>
               </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array(
                     'PRIMARY-KEYS' => array(
                                        0 => array(
                                              'name'    => 'asset_type_pk',
                                              'COLUMNS' => array(
                                                            0 => 'type_code',
                                                           ),
                                             ),
                                       ),
                    );

        $retVal = DALSchemaParser::getTableConstraints($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testPrimaryKeyConstraints()


    /**
     * Tests getTableConstraints() for table with a foreign key constraint.
     *
     * Test that getTableConstraints returns correct array elements when table
     * has foreign key constraint.
     *
     * @since  4.0.0
     * @return void
     */
    public function testForeignKeyConstraints()
    {
        $xml = '<table name="test"><constraints>
                    <foreign-key ref-table="asset_type" name="asset_type_fk1" on-delete="CASCADE">
                        <column references="type_code">parent_type</column>
                    </foreign-key>
               </constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array(
                     'FOREIGN-KEYS' => array(
                                        0 => array(
                                              'name'      => 'asset_type_fk1',
                                              'table'     => '',
                                              'on-delete' => 'CASCADE',
                                              'COLUMNS'   => array (
                                                              0 => array(
                                                                    'name'       => 'parent_type',
                                                                    'references' => 'type_code',
                                                                   ),
                                                             ),
                                             ),
                                       ),
                    );

        $retVal = DALSchemaParser::getTableConstraints($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testForeignKeyConstraints()


    /**
     * Tests getTableConstraints() for a table with a unique constraint.
     *
     * Test that getTableConstraints returns correct array elements when table
     * has unqiue constraint.
     *
     * @since  4.0.0
     * @return void
     */
    public function testUniqueConstraints()
    {
        $xml = '<table name="test"><constraints>
                    <unique name="unique_constraint">
                      <column>assetid</column>
                    </unique></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array(
                     'UNIQUES' => array(
                                   0 => array(
                                         'name'    => 'unique_constraint',
                                         'COLUMNS' => array(
                                                       0 => 'assetid',
                                                      ),
                                        ),
                                  ),
                    );

        $retVal = DALSchemaParser::getTableConstraints($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testUniqueConstraints()


    /**
     * Tests getTableConstraints() for a table with all constraints.
     *
     * Test that getTableConstraints returns correct array elements when table
     * has all constraints.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAllConstraints()
    {
        $xml = '<table name="test"><constraints>
                    <primary-key name="asset_type_pk">
                        <column>type_code</column>
                    </primary-key>
                    <foreign-key ref-table="asset_type" name="asset_type_fk1" on-delete="CASCADE">
                        <column references="type_code">parent_type</column>
                    </foreign-key>
                    <unique name="unique_constraint">
                        <column>assetid</column>
                    </unique></constraints></table>';

        $queryXml = new DOMDocument();
        $queryXml->loadXML($xml);
        $table    = $queryXml->getElementsByTagName('table')->item(0);
        $expected = array(
                     'PRIMARY-KEYS' => array(
                                        0 => array(
                                              'name'    => 'asset_type_pk',
                                              'COLUMNS' => array(
                                                            0 => 'type_code',
                                                           ),
                                             ),
                                       ),
                     'FOREIGN-KEYS' => array(
                                        0 => array(
                                              'name'      => 'asset_type_fk1',
                                              'table'     => '',
                                              'on-delete' => 'CASCADE',
                                              'COLUMNS'   => array (
                                                              0 => array(
                                                                    'name'       => 'parent_type',
                                                                    'references' => 'type_code',
                                                                   ),
                                                             ),
                                             ),
                                       ),
                     'UNIQUES'      => array(
                                        0 => array(
                                              'name'    => 'unique_constraint',
                                              'COLUMNS' => array(
                                                            0 => 'assetid',
                                                           ),
                                             ),
                                       ),
                    );

        $retVal = DALSchemaParser::getTableConstraints($table);

        PHPUnit_Framework_Assert::assertEquals($expected, $retVal);

    }//end testAllConstraints()


}//end class

?>
