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

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';

/**
 * Unit tests for the getQuerySource() method.
 *
 * getQuerySource() returns query in its XML format.
 */
class GetQuerySourceUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array(
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys1',
                dirname(dirname(dirname(__FILE__))).'/Channels/Tests/TestSystems/TestSys2',
               );

    }//end getRequiredSystems()


    /**
     * Test that getQuerySource returns null if system does not exist.
     *
     * @return void
     */
    public function testNonExistentSystem()
    {
        $result = DALBaker::getQuerySource('Tasdfsdfsdf', 'getLinks');

        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentSystem()


    /**
     * Test that getQuerySource returns null if query does not exist.
     *
     * @return void
     */
    public function testNonExistentQuery()
    {
        $result = DALBaker::getQuerySource('TestSys1', 'sdfsdfsdf');

        PHPUnit_Framework_Assert::assertNull($result);

    }//end testNonExistentQuery()


    /**
     * Test that getQuerySource returns DomElement if query does exist.
     *
     * @return void
     */
    public function testExistentQuery()
    {
        $result = DALBaker::getQuerySource('TestSys1', 'getLinks');

        PHPUnit_Framework_Assert::assertNotNull($result);

    }//end testExistentQuery()


}//end class

?>
