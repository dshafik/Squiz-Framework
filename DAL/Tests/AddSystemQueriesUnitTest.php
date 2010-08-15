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
 * Unit Tests for the addSystemQueries() method of DALBaker.
 */
class AddSystemQueriesUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(__FILE__).'/TestSystems/MergeSys1',
               );

    }//end getRequiredSystems()


    /**
     * Adds the test system which is used for all tests.
     *
     * @return NULL
     */
    public function setupSystems()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        DALBaker::addSystemQueries('MergeSys1');

    }//end setupSystems()


    /**
     * Test addSystemQueries() produces the correct number of queries.
     *
     * @return void
     */
    public function testAddSystemQueriesCount()
    {
        self::setupSystems();
        $xmlFiles = FileSystem::listDirectory(DAL::getOvenPath(), array('.xml'));
        PHPUnit_Framework_Assert::assertEquals(4, count($xmlFiles));

    }//end testAddSystemQueriesCount()


    /**
     * Test addSystemQueries() produces the correct file names for XML files.
     *
     * @return void
     */
    public function testAddSystemQueriesFileNames()
    {
        self::setupSystems();
        $xmlFiles = FileSystem::listDirectory(DAL::getOvenPath(), array('.xml'), TRUE, FALSE);
        $expected = array(
                     'getLinks.xml',
                     'getLinksTwo.xml',
                     'getLinksThree.xml',
                     'getLinksFour.xml',
                    );

        PHPUnit_Framework_Assert::assertEquals($expected, $xmlFiles);

    }//end testAddSystemQueriesFileNames()


    /**
     * Test addSystemQueries() returns FALSE for non-existent system.
     *
     * @return void
     */
    public function testAddSystemQueriesNoSystem()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $ret = DALBaker::addSystemQueries('NonSystem');
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testAddSystemQueriesNoSystem()


}//end class
