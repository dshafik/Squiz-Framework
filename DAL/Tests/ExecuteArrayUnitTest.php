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
 * Unit Tests for the getArray() method of DAL.
 */
class ExecuteArrayUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Entries to use as test inserts into the database.
     *
     * @var $_testEntries The array of test entries.
     */
    private $_testEntries = array(
                             array(
                              'id'   => '1',
                              'name' => 'EntryA',
                             ),
                             array(
                              'id'   => '2',
                              'name' => 'EntryB',
                             ),
                             array(
                              'id'   => '3',
                              'name' => 'EntryC',
                             ),
                             array(
                              'id'   => '4',
                              'name' => 'EntryD',
                             ),
                             array(
                              'id'   => '5',
                              'name' => 'EntryE',
                             ),
                             array(
                              'id'   => '6',
                              'name' => 'EntryF',
                             ),
                            );


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(__FILE__).'/TestSystems/QueryTestSystem',
               );

    }//end getRequiredSystems()


    /**
     * This test messes around with the ovens, so we need to bake separately.
     *
     * @return boolean
     */
    public function bakeSeparately()
    {
        return TRUE;

    }//end bakeSeparately()


    /**
     * Test the array is executed correctly.
     *
     * @return void
     */
    public function testExecuteQueryArray()
    {
        // Install test entries.
        foreach ($this->_testEntries as $data) {
            Channels::createEventBasket();
            Channels::addToBasket('id', $data['id']);
            Channels::addToBasket('name', $data['name']);
            DAL::executeQuery('QueryTestSystem', 'insertTestNames');
            Channels::removeEventBasket();
        }

        $query    = DAL::getDALQuery('QueryTestSystem', 'getTestNames', 'assoc');
        $ret      = DAL::ExecuteDALQuery($query);
        $expected = array(
                     array(
                      'name' => 'EntryA',
                     ),
                     array(
                      'name' => 'EntryB',
                     ),
                     array(
                      'name' => 'EntryC',
                     ),
                     array(
                      'name' => 'EntryD',
                     ),
                     array(
                      'name' => 'EntryE',
                     ),
                     array(
                      'name' => 'EntryF',
                     ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testExecuteQueryArray()


}//end class
