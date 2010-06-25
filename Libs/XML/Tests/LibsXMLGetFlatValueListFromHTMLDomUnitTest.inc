<?php
/**
 * XML Lib Tests.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once dirname(dirname(__FILE__)).'/XML.inc';

/**
 * Unit tests for the getFlatValueListFromHTMLDom() Method of XML.
 *
 * @since 4.0.0
 */
class LibsXMLGetFlatValueListFromHTMLDomUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Basic HTML test.
     *
     * @since  4.0.0
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
