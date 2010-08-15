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
 * Unit tests for the dbConnect method.
 */
class DALDbConnectUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that dbConnect method throws exception when it cannot connect to db.
     *
     * @return void
     */
    public function testInvalidConnection()
    {
        $exception = FALSE;
        try {
            $dsn = array('DSN' => 'sdfsfsdfsdff');
            $con = DAL::dbConnect($dsn);
        } catch (PDOException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        } catch (DALException $e) {
            $expected = 'DBError!:invalid data source name';
            PHPUnit_Framework_Assert::assertEquals($expected, $e->getMessage());
            $exception = TRUE;
        }

        $count = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertEquals(0, $count);
        PHPUnit_Framework_Assert::assertTrue($exception);

    }//end testInvalidConnection()


    /**
     * Tests that dbConnect method throws exception when it cannot connect to db.
     *
     * @return void
     */
    public function testValidConnection()
    {
        try {
            // Just use the default dsn.
            $con = DAL::dbConnect();
            PHPUnit_Framework_Assert::assertNotNull($con);
            PHPUnit_Framework_Assert::assertTrue(DAL::dbConnected());
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (PDOException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        } catch (DALException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage().$e->getTraceAsString());
        }

    }//end testValidConnection()


}//end class

?>
