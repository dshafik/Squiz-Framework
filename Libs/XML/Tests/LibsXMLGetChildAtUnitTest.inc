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
 * Unit tests for the getChildAt() method of XML.
 *
 */
class LibsXMLGetChildAtUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test the getChildAt() returns null for a node that has no child elements.
     *
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
