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
 * Unit tests for the childCount() method of XML.
 *
 */
class LibsXMLChildCountUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test the childCount() returns 0 for a node that has no child elements.
     *
     * @return void
     */
    public function testNoChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test>text</test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(0, $ret);

    }//end testNoChildren()


    /**
     * Test the childCount() returns 0 for a node that has no child elements.
     *
     * @return void
     */
    public function testSingleChild()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><tmp /></test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(1, $ret);

    }//end testSingleChild()


    /**
     * Test the childCount() returns correct result for multiple children.
     *
     * @return void
     */
    public function testChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><tmp /><tmp /><tmp /></test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(3, $ret);

    }//end testChildren()


    /**
     * Sub-children test.
     *
     * Test the childCount() returns correct result for a node that has children
     * with sub children.
     *
     * @return void
     */
    public function testSubChildren()
    {
        $dom = new DOMDocument;
        $dom->loadXML('<test><tmp /><tmp /><tmp><child /></tmp></test>');
        $root = $dom->documentElement;
        $ret  = XML::childCount($root);
        PHPUnit_Framework_Assert::assertEquals(3, $ret);

    }//end testSubChildren()


}//end class

?>
