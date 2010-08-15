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
require_once dirname(dirname(__FILE__)).'/Parsers/DALQueryParser.inc';

/**
 * Unit tests for the getTableColumnTypes() method.
 *
 * getTableColumnTypes() gets the column types of a specific table in a system.
 */
class GetTableColumnTypesUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys2',
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys1',
               );

    }//end getRequiredSystems()


    /**
     * Test that getTableColumnTypes return NULL when system does not exist.
     *
     * @return void
     */
    public function testNonExistentSystem()
    {
        $result = DALBaker::getTableColumnTypes('InvalidSystem', 'table');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSystem()


    /**
     * Test that getTableColumnTypes return NULL when table does not exist.
     *
     * @return void
     */
    public function testNonExistentTable()
    {
        $result = DALBaker::getTableColumnTypes('TestSys1', 'invalidTable');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentTable()


    /**
     * Test that getTableColumnTypes return correct array values.
     *
     * @return void
     */
    public function testArrayOfColumns()
    {
        $result   = DALBaker::getTableColumnTypes('TestSys1', 'table1');
        $expected = array(
                     'assetid'               => 'INTEGER',
                     'type_code'             => 'VARCHAR',
                     'version'               => 'VARCHAR',
                     'name'                  => 'VARCHAR',
                     'status'                => 'SMALLINT',
                     'status_changed_userid' => 'INTEGER',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testArrayOfColumns()


    /**
     * Test that getTableColumnTypes with column specified return correct value.
     *
     * @return void
     */
    public function testSingleColumn()
    {
        $result   = DALBaker::getTableColumnTypes('TestSys1', 'table1', 'version');
        $expected = 'VARCHAR';

        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testSingleColumn()


    /**
     * Test getTableColumnTypes for a non-existent column.
     *
     * Test that getTableColumnTypes with non existent column specified return
     * NULL.
     *
     * @return void
     */
    public function testNonExistentSingleColumn()
    {
        $result = DALBaker::getTableColumnTypes('TestSys1', 'table1', 'asdfdf');
        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSingleColumn()


}//end class

?>
