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
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the getDb method.
 *
 */
class GetDbUnitTest extends AbstractMySourceUnitTest
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
     * Tests that getDdb connects to database if there is no connection alrady.
     *
     * @return void
     */
    public function testNoConnection()
    {
        try {
            // Close the current database connection.
            DAL::dbClose();
            $db = DAL::getDb();
            PHPUnit_Framework_Assert::assertNotNull($db);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testNoConnection()


    /**
     * Tests that getDb returns the current PDO connection instead of a new one.
     *
     * @return void
     */
    public function testOpenConnection()
    {
        try {
            // Close the current database connection.
            $db  = DAL::getDb();
            $db2 = DAL::getDb();
            PHPUnit_Framework_Assert::assertEquals($db, $db2);
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testOpenConnection()


}//end class

?>
