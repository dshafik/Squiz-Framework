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
 * Unit Tests for the getBakedQueryFileName and loadBakedQuery methods.
 */
class BakedQueryFileHandlingUnitTest extends AbstractMySourceUnitTest
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
     * Test getBakedQueryFileName() returns the path of a newly generated file.
     *
     * @return void
     */
    public function testGetBakedQueryFileName()
    {
        DALBaker::generateQueryXml('TestSystem', 'TestQuery');
        $fileName = DALBaker::getBakedQueryFileName('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertTrue(file_exists($fileName));

    }//end testGetBakedQueryFileName()


    /**
     * Test getBakedQueryFileName() returns a non-existent file path.
     *
     * This is tested after the Oven directory is cleared.
     *
     * @return void
     */
    public function testGetBakedQueryFileNameNoFile()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $fileName = DALBaker::getBakedQueryFileName('abc', 'def');
        PHPUnit_Framework_Assert::assertFalse(file_exists($fileName));

    }//end testGetBakedQueryFileNameNoFile()


    /**
     * Test loadBakedQuery() returns a DomDocument on the generated file.
     *
     * @return void
     */
    public function testLoadBakedQuery()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        DALBaker::generateQueryXml('TestSystem', 'TestQuery');
        $doc   = DALBaker::loadBakedQuery('TestSystem', 'TestQuery');
        $class = get_class($doc);
        PHPUnit_Framework_Assert::assertEquals('DOMDocument', get_class($doc));

    }//end testLoadBakedQuery()


    /**
     * Test loadBakedQuery() returns NULL for a non-existent file.
     *
     * @return void
     */
    public function testLoadBakedQueryNull()
    {
        FileSystem::clearDirectory(DAL::getOvenPath());
        $doc = DALBaker::loadBakedQuery('TestSystem', 'TestQuery');
        PHPUnit_Framework_Assert::assertNull($doc);

    }//end testLoadBakedQueryNull()


}//end class
