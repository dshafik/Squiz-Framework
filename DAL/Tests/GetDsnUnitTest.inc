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
 * Unit tests for the getDsn method.
 *
 * @since 4.0.0
 */
class GetDsnUnitTest extends AbstractMySourceUnitTest
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
     * Tests that getDsn method returns non empty array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testValidDsn()
    {
        try {
            $dsn = DAL::getDsn();
            PHPUnit_Framework_Assert::assertFalse(empty($dsn));
        } catch (DALException $e) {
            PHPUnit_Framework_Assert::fail($e->getMessage());
        }

    }//end testValidDsn()


}//end class

?>
