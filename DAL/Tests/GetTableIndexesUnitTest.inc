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
 * Unit tests for the getTableIndexes() method.
 *
 * getTableIndexes() converts XML table indexes to php array.
 */
class GetTableIndexesUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Schema xml for this unit test.
     *
     * @var string $_schema Schema xml.
     */
    private $_schema = '<schema system="asset">
                         <tables>
                          <table name="asset">
                           <columns></columns>
                           <indexes>
                               <index name="asset_type_code">
                                   <column>type_code</column>
                                   <column>assetid</column>
                               </index>
                               <index name="asset_type_code2">
                                   <column>type_code</column>
                               </index>
                           </indexes></table></tables></schema>';


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
     * Test that getTableIndexes returns empty array when no indexes defined.
     *
     * @return void
     */
    public function testNoIndexes()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);

        // Remove the indexes tag.
        $table   = $doc->getElementsByTagName('table')->item(0);
        $indexes = $doc->getElementsByTagName('indexes')->item(0);
        $table->removeChild($indexes);
        $result = DALSchemaParser::getTableIndexes($table);

        PHPUnit_Framework_Assert::assertTrue(empty($result));

    }//end testNoIndexes()


    /**
     * Test for getTableIndexes() with valid indexes.
     *
     * Test that getTableIndexes returns correct array result when there are
     * valid indexes defined.
     *
     * @return void
     */
    public function testValidatedIndexes()
    {
        $doc = new DomDocument();
        $doc->loadXML($this->_schema);

        // Remove the indexes tag.
        $table  = $doc->getElementsByTagName('table')->item(0);
        $result = DALSchemaParser::getTableIndexes($table);

        $expected = array(
                     0 => array(
                           'name'    => 'asset_type_code',
                           'COLUMNS' => array(
                                         'type_code',
                                         'assetid',
                                        ),
                          ),
                     1 => array(
                           'name'    => 'asset_type_code2',
                           'COLUMNS' => array(
                                         'type_code',
                                        ),
                          ),
                    );

        PHPUnit_Framework_Assert::assertEquals($result, $expected);

    }//end testValidatedIndexes()


}//end class

?>
