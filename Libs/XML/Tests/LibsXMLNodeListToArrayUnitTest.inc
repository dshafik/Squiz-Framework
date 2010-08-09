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
 * Unit tests for the NodeListToArray Method of XML.
 *
 */
class LibsXMLNodeListToArrayUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test that clearDirectory() with deleteRoot === FALSE leaves an empty dir.
     *
     * @return void
     */
    public function testNodeListSize()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $root    = $dom->documentElement;
        $systems = $root->getElementsByTagName('system');
        $ret     = XML::nodeListToArray($systems);
        PHPUnit_Framework_Assert::assertTrue(count($ret) === 5);

    }//end testNodeListSize()


    /**
     * Test the content/order of values returned by NodeListToArray().
     *
     * @return void
     */
    public function testNodeListOrder()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $root    = $dom->documentElement;
        $systems = $root->getElementsByTagName('system');
        $ret     = XML::nodeListToArray($systems);

        $firstElement = $ret[0];
        $firstName    = $firstElement->getAttribute('name');
        PHPUnit_Framework_Assert::assertTrue($firstName === 'Url');

    }//end testNodeListOrder()


}//end class

?>
