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
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the commit method.
 *
 * @since 4.0.0
 */
class DALCommitUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that no exceptions thrown when trying to commit when no transactions started.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCommitOnNoTransaction()
    {
        try {
            DAL::commit();
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(-1, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

        // Make sure this test does not mess up the transaction count.
        DAL::beginTransaction();
        $count = DAL::getTransactionCount();
        PHPUnit_Framework_Assert::assertEquals(0, $count);

    }//end testCommitOnNoTransaction()


    /**
     * Tests that no exceptions thrown when commiting an open transactions.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCommitTransaction()
    {
        try {
            DAL::beginTransaction();
            DAL::commit();

            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (ChannelException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testCommitTransaction()


}//end class

?>
