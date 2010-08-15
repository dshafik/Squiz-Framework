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
 * Unit tests for the getTableColumns() method.
 *
 * getTableColumns() converts XML table columns to php array.
 */
class GetTableColumnsUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var string $_schema Schema xml.
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
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * These tests require a fresh system for each test.
     *
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Test that getTableColumns returns empty array when no columns defined.
     *
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
