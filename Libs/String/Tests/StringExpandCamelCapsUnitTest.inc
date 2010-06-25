<?php
/**
 * Unit Tests for the expandCamelCaps function.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit tests for the expandCamelCaps function.
 *
 * @since 4.0.0
 */
class StringExpandCamelCapsUnitTest extends AbstractMySourceUnitTest
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
     * Test expandCamelCaps.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExpandCamelCaps()
    {
        $expected = 'Hello world';
        $result   = String::expandCamelCaps('helloWorld');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello world from home';
        $result   = String::expandCamelCaps('helloWorldFromHome');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello';
        $result   = String::expandCamelCaps('hello');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello 123';
        $result   = String::expandCamelCaps('hello123');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello 123 world';
        $result   = String::expandCamelCaps('hello123World');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello AWorld';
        $result   = String::expandCamelCaps('helloAWorld');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello world';
        $result   = String::expandCamelCaps('helloWorld  ');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Get HTML';
        $result   = String::expandCamelCaps('getHTML');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testExpandCamelCaps()


    /**
     * Test invalid values.
     *
     * @since  4.0.0
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::expandCamelCaps(NULL);
        PHPUnit_Framework_Assert::assertEquals('', $result);

        $result = String::expandCamelCaps('');
        PHPUnit_Framework_Assert::assertEquals('', $result);

    }//end testWithInvalidValues()


}//end class

?>
