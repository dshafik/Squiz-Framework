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
 * Unit tests for the getTableColumns() method.
 *
 * getTableColumns() converts XML table columns to php array.
 *
 * @since 4.0.0
 */
class GetTableColumnsUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var    string $_schema Schema xml.
     * @since  4.0.0
     */
    private $_schema = '<schema system="asset">
                         <tables>
                          <table name="asset">
                           <columns>
                            <column type="ASSETID" allow-null="true">assetid</column>
                            <column type="TYPECODE" allow-null="true"></column>
                            <column type="VARCHAR" size="20" allow-null="true" default="test">version</column>
                           </columns>
                          </table>
                         </tables>
                        </schema>';


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
     * These tests require a fresh system for each test.
     *
     * @since  4.0.0
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Test that getTableColumns returns empty array when no columns defined.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoColumns()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);

        // Remove the indexes tag.
        $table   = $doc->getElementsByTagName('table')->item(0);
        $columns = $table->getElementsByTagName('columns')->item(0);
        $table->removeChild($columns);
        $result = DALSchemaParser::getTableColumns($table);

        PHPUnit_Framework_Assert::assertTrue(empty($result));

    }//end testNoColumns()


    /**
     * Test that getTableColumns returns correct array values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidatedColumns()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);

        // Remove the indexes tag.
        $table    = $doc->getElementsByTagName('table')->item(0);
        $result   = DALSchemaParser::getTableColumns($table);
        $expected = array(
                     0 => array (
                           'type'       => 'ASSETID',
                           'size'       => '',
                           'scale'      => '',
                           'name'       => 'assetid',
                           'allow-null' => 'true',
                           'default'    => '',
                          ),
                     1 => array (
                           'type'       => 'TYPECODE',
                           'size'       => '',
                           'scale'      => '',
                           'name'       => '',
                           'allow-null' => 'true',
                           'default'    => '',
                          ),
                     2 => array (
                           'type'       => 'VARCHAR',
                           'size'       => '20',
                           'scale'      => '',
                           'name'       => 'version',
                           'allow-null' => 'true',
                           'default'    => 'test',
                          ),
                    );

        PHPUnit_Framework_Assert::assertEquals($result, $expected);

    }//end testValidatedColumns()


}//end class

?>
