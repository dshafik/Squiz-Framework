<?php
/**
 * Unit Tests for the ellipsisize method.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage LocalAsset
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for the ellipsisize method.
 *
 * @since 4.0.0
 */
class StringEllipsisizeUnitTest extends AbstractMySourceUnitTest
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
     * Test passing in invalid values for each parameter.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTypeVerification()
    {
        $expect = '';
        $result = String::ellipsisize(1, 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('', 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize(NULL, 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('Hello World', NULL);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('Hello World', 'Test');
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testTypeVerification()


    /**
     * Test minimum boundary cases of length argument.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMinimumLengthBoundary()
    {
        $expect = '';
        $result = String::ellipsisize('Hello World', -1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '...';
        $result = String::ellipsisize('Hello World', 0);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'H...';
        $result = String::ellipsisize('Hello World', 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testMinimumLengthBoundary()


    /**
     * Test General strings.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGeneralStrings()
    {
        $expect = 'Hel...';
        $result = String::ellipsisize('Hello World', 3);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hell...';
        $result = String::ellipsisize('Hello World', 4);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello...';
        $result = String::ellipsisize('Hello World', 5);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello...';
        $result = String::ellipsisize('Hello World', 6);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testGeneralStrings()


    /**
     * Test strings which are shorter than the length argument.
     *
     * @since  4.0.0
     * @return void
     */
    public function testStringEqualOrShorterThanLength()
    {
        // Length is 11.
        $expect = 'Hello World';

        $result = String::ellipsisize('Hello World', 11);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('Hello World', 12);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testStringEqualOrShorterThanLength()


    /**
     * Test strings which end with dot(s).
     *
     * @since  4.0.0
     * @return void
     */
    public function testStringWithDots()
    {
        $expect = 'Hel...';
        $result = String::ellipsisize('Hello World.', 3);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hel...';
        $result = String::ellipsisize('Hello World..', 3);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 11);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 12);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 13);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 14);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testStringWithDots()


}//end class

?>
