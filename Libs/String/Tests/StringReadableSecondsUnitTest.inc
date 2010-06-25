<?php
/**
 * Unit Tests for the readableSeconds function.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the readableSeconds function.
 *
 * @since 4.0.0
 */
class StringReadableSecondsUnitTest extends AbstractMySourceUnitTest
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
     * Test the singular and plural versions of all time units.
     *
     * @since  4.0.0
     * @return void
     */
    public function testPlurals()
    {
        $expected = '1 second';
        $result   = String::readableSeconds(1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 seconds';
        $result   = String::readableSeconds(2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 minute';
        $result   = String::readableSeconds(61);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 minutes';
        $result   = String::readableSeconds(120);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 hour';
        $result   = String::readableSeconds(3601);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 hours';
        $result   = String::readableSeconds(7200);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 day';
        $result   = String::readableSeconds(86401);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 days';
        $result   = String::readableSeconds(86400 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 week';
        $result   = String::readableSeconds(604801);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 weeks';
        $result   = String::readableSeconds(604800 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 month';
        $result   = String::readableSeconds(2419201);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 months';
        $result   = String::readableSeconds(2419200 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 year';
        $result   = String::readableSeconds(31536001);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 years';
        $result   = String::readableSeconds(31536000 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testPlurals()


    /**
     * Test boundary cases where strings roll over.
     *
     * @since  4.0.0
     * @return void
     */
    public function testBoundaries()
    {
        $expected = '0 seconds';
        $result   = String::readableSeconds(0);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '60 seconds';
        $result   = String::readableSeconds(60);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '60 minutes';
        $result   = String::readableSeconds(3600);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '24 hours';
        $result   = String::readableSeconds(86400);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '7 days';
        $result   = String::readableSeconds(604800);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '4 weeks';
        $result   = String::readableSeconds(2419200);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '13 months';
        $result   = String::readableSeconds(31536000);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testBoundaries()


    /**
     * Test negative second values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNegativeValues()
    {
        $expected = '-1 seconds';
        $result   = String::readableSeconds(-1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '-61 seconds';
        $result   = String::readableSeconds(-61);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '-6000 seconds';
        $result   = String::readableSeconds(-6000);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testNegativeValues()


}//end class

?>
