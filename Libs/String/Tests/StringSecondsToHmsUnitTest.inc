<?php
/**
 * Unit Tests for the secondsToHms function.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the secondsToHms function.
 *
 * @since 4.0.0
 */
class StringSecondsToHmsUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Sets up this unit test.
     *
     * This bakes out a working system that contains the systems that this
     * unit test requires to be able to run.
     *
     * @since  4.0.0
     * @return void
     */
    protected function setUp()
    {
        parent::setUp();
        include_once 'Libs/String/String.inc';

    }//end setUp()


    /**
     * Test secondsToHms.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSecondsToHms()
    {
        $expected = '0:00';
        $result   = String::secondsToHms(0);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '0:01';
        $result   = String::secondsToHms(1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '0:59';
        $result   = String::secondsToHms(59);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00';
        $result   = String::secondsToHms(60);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:01';
        $result   = String::secondsToHms(61);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '59:59';
        $result   = String::secondsToHms(3599);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00:00';
        $result   = String::secondsToHms(3600);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00:01';
        $result   = String::secondsToHms(3601);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00:59';
        $result   = String::secondsToHms(3659);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:01:00';
        $result   = String::secondsToHms(3660);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:59:59';
        $result   = String::secondsToHms(7199);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testSecondsToHms()


    /**
     * Test invalid values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::secondsToHms(NULL);
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

        $result = String::secondsToHms('');
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

        $result = String::secondsToHms(-1);
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

        $result = String::secondsToHms('A');
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

    }//end testWithInvalidValues()


}//end class

?>
