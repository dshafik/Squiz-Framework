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
 * Unit tests for the beginTransaction method.
 *
 */
class DALBeginTransactionUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Check if DB is connected once transaction is started.
     *
     * @return void
     */
    public function testDBInitAfterBegin()
    {
        try {
            DAL::beginTransaction();
            $connected = DAL::dbConnected();
            PHPUnit_Framework_Assert::assertEquals(TRUE, $connected);
            DAL::rollback();

            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testDBInitAfterBegin()


    /**
     * Tests that no exceptions thrown when trying to start two transactions.
     *
     * @return void
     */
    public function testMultipleBeginTransactions()
    {
        try {
            DAL::beginTransaction();
            DAL::beginTransaction();
            DAL::rollback();
            DAL::rollback();

            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testMultipleBeginTransactions()


    /**
     * Tests transaction count.
     *
     * @return void
     */
    public function testTransactionCount()
    {
        try {
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);

            DAL::beginTransaction();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(1, $count);

            DAL::beginTransaction();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(2, $count);

            DAL::commit();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(1, $count);

            DAL::beginTransaction();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(2, $count);

            DAL::commit();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(1, $count);

            DAL::rollback();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testTransactionCount()


}//end class

?>
