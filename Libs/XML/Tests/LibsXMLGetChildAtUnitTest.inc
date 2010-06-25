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
 * Unit tests for the getChildAt() method of XML.
 *
 * @since 4.0.0
 */
class LibsXMLGetChildAtUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test the getChildAt() returns null for a node that has no child elements.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text</test>');
        $root = $dom->documentElement;
        $ret  = XML::getChildAt($root, 4);
        PHPUnit_Framework_Assert::assertNull($ret);

    }//end testNoChildren()


    /**
     * Test the getChildAt() returns correct node.
     *
     * @since  4.0.0
     * @return void
     */
    public function testChildAtOne()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child1 /></test>');
        $root = $dom->documentElement;
        $exp  = $root->getElementsByTagName('child1')->item(0);
        $ret  = XML::getChildAt($root, 0);
        PHPUnit_Framework_Assert::assertEquals($ret, $exp);

    }//end testChildAtOne()


    /**
     * Test the getChildAt() returns correct node.
     *
     * @since  4.0.0
     * @return void
     */
    public function testChildAtTwo()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child1 /><child1 /><tmp>test</tmp></test>');
        $root = $dom->documentElement;
        $exp  = $root->getElementsByTagName('tmp')->item(0);
        $ret  = XML::getChildAt($root, 2);
        PHPUnit_Framework_Assert::assertEquals($ret, $exp);

    }//end testChildAtTwo()


}//end class

?>
