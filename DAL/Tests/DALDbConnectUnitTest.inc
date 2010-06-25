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
 * Unit tests for the dbConnect method.
 *
 * @since 4.0.0
 */
class DALDbConnectUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that dbConnect method throws exception when it cannot connect to db.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
