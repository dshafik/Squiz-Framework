<?php
/**
 * Web Lib Tests.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file license.txt. If not, see
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *
 * @package    Framework
 * @subpackage Libs
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Libs/Web/Web.inc';

/**
 * Unit tests for the getQueryStringParams() method of Web.
 *
 */
class LibsWebGetQueryStringParamsUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test no url passed in.
     *
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
