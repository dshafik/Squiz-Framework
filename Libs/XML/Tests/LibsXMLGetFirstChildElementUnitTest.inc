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
 * Unit tests for the getFirstChildElement() Method of XML.
 *
 * Unlike fisrChild, getFirstChildElement returns the first XML_ELEMENT_NODE.
 *
 * @since 4.0.0
 */
class LibsXMLGetFirstChildElementUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Text Node test.
     *
     * Test the getFirstChildElement() returns null for a node that has text
     * and no node elements.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTextNode()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text</test>');
        $root = $dom->documentElement;
        $ret  = XML::getFirstChildElement($root);
        PHPUnit_Framework_Assert::assertNull($ret);

    }//end testTextNode()


    /**
     * Text Node test.
     *
     * Test the hasChildElements() returns child for a node that has text
     * and node elements.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTextNodeWithNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text<child></child></test>');
        $root = $dom->documentElement;
        $ret  = XML::getFirstChildElement($root);
        PHPUnit_Framework_Assert::assertEquals('child', $ret->tagName);

    }//end testTextNodeWithNodeElement()


    /**
     * Text Node test.
     *
     * Test the hasChildElements() returns child for a node that has text
     * and empty node elements.
     *
     * @since  4.0.0
     * @return void
     */
    public function testTextNodeWithEmptyNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text<child /></test>');
        $root = $dom->documentElement;
        $ret  = XML::getFirstChildElement($root);
        PHPUnit_Framework_Assert::assertEquals('child', $ret->tagName);

    }//end testTextNodeWithEmptyNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test that hasChildElements() returns child for a node that has
     * node element.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child>text</child></test>');
        $root = $dom->documentElement;
        $ret  = XML::getFirstChildElement($root);
        PHPUnit_Framework_Assert::assertEquals('child', $ret->tagName);

    }//end testNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns child for a node that has
     * empty node element.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child /></test>');
        $root = $dom->documentElement;
        $ret  = XML::getFirstChildElement($root);
        PHPUnit_Framework_Assert::assertEquals('child', $ret->tagName);

    }//end testEmptyNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns child2 for a node that has
     * empty node element.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyNodeElements()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child2 /><child /></test>');
        $root = $dom->documentElement;
        $ret  = XML::getFirstChildElement($root);
        PHPUnit_Framework_Assert::assertEquals('child2', $ret->tagName);

    }//end testEmptyNodeElements()


}//end class

?>
