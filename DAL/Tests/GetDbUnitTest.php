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
 * Unit tests for the getDb method.
 *
 * @since 4.0.0
 */
class GetDbUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Returns an array of systems that this test requires to run.
     *
     * @since  4.0.0
     * @return array(string)
     */
    public function getRequiredSystems()
    {
        return array();

    }//end getRequiredSystems()


    /**
     * Tests that getDdb connects to database if there is no connection alrady.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
