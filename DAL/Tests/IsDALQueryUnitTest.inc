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

require_once dirname(dirname(__FILE__)).'/DALBaker.inc';
require_once dirname(dirname(__FILE__)).'/Parsers/DALSchemaParser.inc';

/**
 * Unit tests for the isDALQuery() method.
 *
 * @since 4.0.0
 */
class IsDALQueryUnitTest extends AbstractMySourceUnitTest
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
     * Tests checking a valid query will return TRUE.
     *
     * @since  4.0.0
     * @return void
     */
    public function testIsDALQueryTrue()
    {
        $testArray = array(
                      'query' => array(),
                     );
        $query = new Query('', $testArray);

        $ret = DAL::isDALQuery($query);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testIsDALQueryTrue()


    /**
     * Tests that an empty array will throw an exception.
     *
     * @since  4.0.0
     * @return void
     */
    public function testIsDALQueryEmptyQueryArray()
    {
        $caught = FALSE;
        try {
            $query = new Query();
        } catch (DALException $e) {
            $caught = TRUE;
        }

        PHPUnit_Framework_Assert::assertTrue($caught);

    }//end testIsDALQueryEmptyQueryArray()


    /**
     * Tests a query that is close but does not contain a DAL_QUERY element.
     *
     * @since  4.0.0
     * @return void
     */
    public function testIsDALQueryFalse()
    {
        $testArray = array(
                      array(
                       'sql'      => '',
                       'bindings' => array(),
                      ),
                     );

        $ret = DAL::isDALQuery($testArray);
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testIsDALQueryFalse()


    /**
     * Test a query that is a normal array returns FALSE.
     *
     * @since  4.0.0
     * @return void
     */
    public function testIsDALQueryRandom()
    {
        $testArray = array(
                      '1',
                      '2',
                      '3',
                      '4',
                     );

        $ret = DAL::isDALQuery($testArray);
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testIsDALQueryRandom()


}//end class

?>
