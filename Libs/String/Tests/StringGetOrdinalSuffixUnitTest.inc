<?php
/**
 * Unit Tests for the getOrGetOrdinalSuffix function.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the getOrdinalSuffix function.
 *
 * @since 4.0.0
 */
class StringGetOrdinalSuffixUnitTest extends AbstractMySourceUnitTest
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
     * Test getOrdinalSuffix.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetOrdinalSuffix()
    {
        $expected = 'st';
        $result   = String::getOrdinalSuffix(1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'rd';
        $result   = String::getOrdinalSuffix(3);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(4);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(19);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(20);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'st';
        $result   = String::getOrdinalSuffix(21);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'nd';
        $result   = String::getOrdinalSuffix(32);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'rd';
        $result   = String::getOrdinalSuffix(43);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(54);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testGetOrdinalSuffix()


    /**
     * Test invalid values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::getOrdinalSuffix('Number');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::getOrdinalSuffix('Number 123');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::getOrdinalSuffix(NULL);
        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testWithInvalidValues()


}//end class

?>
