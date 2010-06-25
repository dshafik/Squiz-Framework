<?php
/**
 * Web Lib Tests.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Web/Web.inc';

/**
 * Unit tests for the getQueryStringParams() method of Web.
 *
 * @since 4.0.0
 */
class LibsWebGetQueryStringParamsUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test no url passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoURL()
    {
        $param = Web::getQueryStringParams();
        PHPUnit_Framework_Assert::assertEquals(array(), $param);

    }//end testNoURL()


    /**
     * Test no param passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoParam()
    {
        $url   = 'http://example.com';
        $param = Web::getQueryStringParams($url);
        PHPUnit_Framework_Assert::assertEquals(array(), $param);

    }//end testNoParam()


    /**
     * Test wrong param passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testWrongParam()
    {
        $url   = 'http://example.com?a=3';
        $param = Web::getQueryStringParams($url, 'b');
        PHPUnit_Framework_Assert::assertEquals('', $param);

    }//end testWrongParam()


    /**
     * Test correct param passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testCorrectParam()
    {
        $url   = 'http://example.com?a=3';
        $param = Web::getQueryStringParams($url, 'a');
        PHPUnit_Framework_Assert::assertEquals('3', $param);

    }//end testCorrectParam()


    /**
     * Test multi param passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testMultiParam()
    {
        $url      = 'http://example.com?a=-3&b=333&c=lol';
        $param    = Web::getQueryStringParams($url);
        $expected = array(
                     'a' => '-3',
                     'b' => '333',
                     'c' => 'lol',
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $param);

    }//end testMultiParam()


    /**
     * Test second param passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testSecondParam()
    {
        $url   = 'http://example.com?a=-3&b=333&c=lol';
        $param = Web::getQueryStringParams($url, 'c');
        PHPUnit_Framework_Assert::assertEquals('lol', $param);

    }//end testSecondParam()


    /**
     * Test invalid param passed in.
     *
     * @since  4.0.0
     * @return void
     */
    public function testInvalidParam()
    {
        $url   = 'http://example.com?a=-3&b=333&c=lol';
        $param = Web::getQueryStringParams($url, 'z');
        PHPUnit_Framework_Assert::assertEquals('', $param);

    }//end testInvalidParam()


}//end class

?>
