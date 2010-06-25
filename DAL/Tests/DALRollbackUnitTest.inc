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
 * Unit tests for the rollback method.
 *
 * @since 4.0.0
 */
class DALRollbackUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that no exceptions thrown when trying to rollback when no transactions started.
     *
     * @since  4.0.0
     * @return void
     */
    public function testRollBackOnNoTransaction()
    {
        try {
            DAL::rollback();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(-1, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

        // Make sure this test does not mess up the transaction count.
        DAL::beginTransaction();
        $count = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertEquals(0, $count);

    }//end testRollBackOnNoTransaction()


    /**
     * Tests that no exceptions thrown when rollback an open transactions.
     *
     * @since  4.0.0
     * @return void
     */
    public function testRollBackTransaction()
    {
        try {
            DAL::beginTransaction();
            DAL::rollback();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testRollBackTransaction()


}//end class

?>
