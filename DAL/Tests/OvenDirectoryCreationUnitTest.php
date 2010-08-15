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
 * Unit Tests for the creation of oven directories.
 */
class OvenDirectoryCreationUnitTest extends AbstractMySourceUnitTest
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
     * Test the return value of the createSystemOvenPath() method.
     *
     * Test is performed for a non-existent directory.
     *
     * @return void
     */
    public function testCreateSystemDirectoryNotExistsReturn()
    {
        $ret = DALBaker::createSystemOvenPath('TestSystem');
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testCreateSystemDirectoryNotExistsReturn()


    /**
     * Test the return value of the createSystemOvenPath() method.
     *
     * Test is performed for a directory that exists.
     *
     * @return void
     */
    public function testCreateSystemDirectoryExistsReturn()
    {
        DALBaker::createSystemOvenPath('TestSystem');
        $ret = DALBaker::createSystemOvenPath('TestSystem');

        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testCreateSystemDirectoryExistsReturn()


    /**
     * Test the path of a newly created system exists.
     *
     * Test is performed for a directory that exists.
     *
     * @return void
     */
    public function testGetOvenPath()
    {
        DALBaker::createSystemOvenPath('TestSystem');
        $path = DAL::getOvenPath('TestSystem');
        $ret  = file_exists($path);

        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testGetOvenPath()


    /**
     * Test the path of a system doesn't exist.
     *
     * Test is performed for a directory that exists.
     *
     * @return void
     */
    public function testGetOvenPathEmptyDir()
    {
        $path = DAL::getOvenPath('TestSystemThatDoesNotExist');
        $ret  = file_exists($path);

        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testGetOvenPathEmptyDir()


}//end class

?>
