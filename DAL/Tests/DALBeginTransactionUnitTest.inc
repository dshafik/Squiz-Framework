<?php
/**
 * Tests for functionality of the Database Abstraction Layer.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage DAL
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(__FILE__)).'/DAL.inc';
require_once dirname(dirname(__FILE__)).'/DALBaker.inc';

/**
 * Unit tests for the beginTransaction method.
 *
 * @since 4.0.0
 */
class DALBeginTransactionUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Check if DB is connected once transaction is started.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
