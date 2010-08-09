<?php
/**
 * XML Lib Tests.
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

require_once dirname(dirname(__FILE__)).'/XML.inc';

/**
 * Unit tests for the getFlatValueListFromHTMLDom() Method of XML.
 *
 */
class LibsXMLGetFlatValueListFromHTMLDomUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Basic HTML test.
     *
     * @return void
     */
    public function testHtml()
    {
        $html = '<p>hello world</p>';
        $list = XML::getFlatValueListFromHTMLDom($html);

        PHPUnit_Framework_Assert::assertTrue(count($list) === 1);
        PHPUnit_Framework_Assert::assertTrue($list[0]['tag'] === 'p');
        PHPUnit_Framework_Assert::assertTrue($list[0]['attr'] === '');
        PHPUnit_Framework_Assert::assertTrue($list[0]['value'] === 'hello world');
        PHPUnit_Framework_Assert::assertTrue($list[0]['length'] === 11);

        $html = '<p><img src="somewhere" />hello</div></p>';
        $list = XML::getFlatValueListFromHTMLDom($html);
        PHPUnit_Framework_Assert::assertTrue(count($list) === 2);
        PHPUnit_Framework_Assert::assertTrue($list[0]['tag'] === 'img');
        PHPUnit_Framework_Assert::assertTrue($list[0]['attr'] === 'src');
        PHPUnit_Framework_Assert::assertTrue($list[0]['value'] === 'somewhere');
        PHPUnit_Framework_Assert::assertTrue($list[0]['length'] === 9);

        $html = '<p><a href="somewhere">hello</a></p>';
        $list = XML::getFlatValueListFromHTMLDom($html);
        PHPUnit_Framework_Assert::assertTrue(count($list) === 2);
        PHPUnit_Framework_Assert::assertTrue($list[0]['tag'] === 'a');
        PHPUnit_Framework_Assert::assertTrue($list[0]['attr'] === 'href');
        PHPUnit_Framework_Assert::assertTrue($list[0]['value'] === 'somewhere');
        PHPUnit_Framework_Assert::assertTrue($list[0]['length'] === 9);
        PHPUnit_Framework_Assert::assertTrue($list[1]['tag'] === 'a');
        PHPUnit_Framework_Assert::assertTrue($list[1]['attr'] === '');
        PHPUnit_Framework_Assert::assertTrue($list[1]['value'] === 'hello');
        PHPUnit_Framework_Assert::assertTrue($list[1]['length'] === 5);

    }//end testHtml()


}//end class

?>
