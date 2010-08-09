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
 * Unit tests for the hasChildElements() Method of XML.
 *
 * Unlike hasChildNodes, hasChildElements returns false if it cannot find a
 * XML_ELEMENT_NODE.
 *
 */
class LibsXMLHasChildElementsUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns false for a node that has text
     * and no node elements.
     *
     * @return void
     */
    public function testTextNode()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text</test>');
        $root = $dom->documentElement;
        $ret  = XML::hasChildElements($root);
        PHPUnit_Framework_Assert::assertFalse($ret);

    }//end testTextNode()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns true for a node that has text
     * and node elements.
     *
     * @return void
     */
    public function testTextNodeWithNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text<child></child></test>');
        $root = $dom->documentElement;
        $ret  = XML::hasChildElements($root);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testTextNodeWithNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns true for a node that has text
     * and empty node elements.
     *
     * @return void
     */
    public function testTextNodeWithEmptyNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text<child /></test>');
        $root = $dom->documentElement;
        $ret  = XML::hasChildElements($root);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testTextNodeWithEmptyNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns true for a node that has
     * node element.
     *
     * @return void
     */
    public function testNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child>text</child></test>');
        $root = $dom->documentElement;
        $ret  = XML::hasChildElements($root);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns true for a node that has
     * empty node element.
     *
     * @return void
     */
    public function testEmptyNodeElement()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child /></test>');
        $root = $dom->documentElement;
        $ret  = XML::hasChildElements($root);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testEmptyNodeElement()


    /**
     * Test for hasChildElements().
     *
     * Test the hasChildElements() returns true for a node that has
     * empty node elements.
     *
     * @return void
     */
    public function testEmptyNodeElements()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><child /><child2 /></test>');
        $root = $dom->documentElement;
        $ret  = XML::hasChildElements($root);
        PHPUnit_Framework_Assert::assertTrue($ret);

    }//end testEmptyNodeElements()


}//end class

?>
