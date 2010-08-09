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
 * Unit tests for the getFirstChildElement() Method of XML.
 *
 * Unlike fisrChild, getFirstChildElement returns the first XML_ELEMENT_NODE.
 *
 */
class LibsXMLGetFirstChildElementUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Text Node test.
     *
     * Test the getFirstChildElement() returns null for a node that has text
     * and no node elements.
     *
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
