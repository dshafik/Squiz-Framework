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
 * Unit tests for the getElementsByTagNames() Method of XML.
 *
 */
class LibsXMLGetElementsByTagNamesUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test the content/order getElementsByTagNames().
     *
     * @return void
     */
    public function testGetElementsOrder()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $elem      = $dom->getElementsByTagName('systems')->item(0);
        $systems   = XML::getElementsByTagNames(array('system'), $elem);
        $firstName = $systems[0]->getAttribute('name');
        PHPUnit_Framework_Assert::assertEquals('Url', $firstName);

    }//end testGetElementsOrder()


    /**
     * Test the wildcard element selector (*).
     *
     * @return void
     */
    public function testGetAllElements()
    {
        $dom = new DOMDocument;
        $dom->load(dirname(__FILE__).'/TestFiles/testSystems.xml');
        $elem        = $dom->getElementsByTagName('systems')->item(0);
        $allElements = XML::getElementsByTagNames(array('*'), $elem);
        PHPUnit_Framework_Assert::assertEquals(5, count($allElements));

    }//end testGetAllElements()


}//end class

?>
