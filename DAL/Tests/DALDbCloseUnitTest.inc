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
 * Unit tests for the dbClose method.
 *
 * @since 4.0.0
 */
class DALDbCloseUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that dbClose method closes the database connection.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCloseConnection()
    {
        try {
            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);

            DAL::dbClose();
            PHPUnit_Framework_Assert::assertFalse(DAL::dbConnected());

            $count = DAL::getTransactionCount();
            PHPUnit_Framework_Assert::assertEquals(0, $count);
        } catch (DALException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testCloseConnection()


}//end class

?>
