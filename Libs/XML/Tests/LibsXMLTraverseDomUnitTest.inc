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
 * Unit tests for the traverseDom() method of the XML lib.
 *
 */
class LibsXMLTraverseDomUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Test XML for running DOM tests on.
     *
     * @var   string $_testDom The string of XML to test on.
     */
    private $_testDom = '<xml>
                          <items>
                           <item name="Box 1">
                            <type>Box</type>
                            <size>10</size>
                           </item>
                           <item name="Box 2">
                            <type>Box</type>
                            <size>20</size>
                           </item>
                           <item name="Basketball 1">
                            <type>Ball</type>
                            <size>7</size>
                            <accessories>
                             <accessory>
                              <type>hoop</type>
                              <size>10 metres</size>
                             </accessory>
                            </accessories>
                           </item>
                           <item name="Soccer Ball">
                            <type>Ball</type>
                            <size>5</size>
                           </item>
                          </items>
                         </xml>';


    /**
     * Test traverseDom produces the correct results.
     *
     * @return void
     */
    public function testNodeListSize()
    {
        $dom = new DOMDocument;
        $dom->loadXML($this->_testDom);
        $root = $dom->documentElement;
        $elem = $root->getElementsByTagName('items')->item(0);
        $ret  = XML::traverseDom($elem);

        // @codingStandardsIgnoreStart
        $expected = array(
                     'tagName'  => 'items',
                     'value'    => '',
                     'children' => array(
                                    array(
                                     'tagName'  => 'item',
                                     'attrs'    => array('name' => 'Box 1'),
                                     'value'    => '',
                                     'children' => array(
                                                    array(
                                                     'tagName' => 'type',
                                                     'value'   => 'Box',
                                                    ),
                                                    array(
                                                     'tagName' => 'size',
                                                     'value'   => '10',
                                                    ),
                                                   ),
                                    ),
                                    array(
                                     'tagName'  => 'item',
                                     'attrs'    => array('name' => 'Box 2'),
                                     'value'    => '',
                                     'children' => array(
                                                    array(
                                                     'tagName' => 'type',
                                                     'value'   => 'Box',
                                                    ),
                                                    array(
                                                     'tagName' => 'size',
                                                     'value'   => '20',
                                                    ),
                                                   ),
                                    ),
                                    array(
                                     'tagName'  => 'item',
                                     'attrs'    => array('name' => 'Basketball 1'),
                                     'value'    => '',
                                     'children' => array(
                                                    array(
                                                     'tagName' => 'type',
                                                     'value'   => 'Ball',
                                                    ),
                                                    array(
                                                     'tagName' => 'size',
                                                     'value'   => '7',
                                                    ),
                                                    array(
                                                     'tagName'  => 'accessories',
                                                     'value'    => '',
                                                     'children' => array(
                                                                    array(
                                                                     'tagName'  => 'accessory',
                                                                     'value'    => '',
                                                                     'children' => array(
                                                                                    array(
                                                                                     'tagName' => 'type',
                                                                                     'value'   => 'hoop',
                                                                                    ),
                                                                                    array(
                                                                                     'tagName' => 'size',
                                                                                     'value'   => '10 metres',
                                                                                    ),
                                                                                   ),
                                                                    ),
                                                                   ),
                                                    ),
                                                   ),
                                    ),
                                    array(
                                     'tagName'  => 'item',
                                     'attrs'    => array('name' => 'Soccer Ball'),
                                     'value'    => '',
                                     'children' => array(
                                                    array(
                                                     'tagName' => 'type',
                                                     'value'   => 'Ball',
                                                    ),
                                                    array(
                                                     'tagName' => 'size',
                                                     'value'   => '5',
                                                    ),
                                                   ),
                                    ),
                                   ),
                    );
        // @codingStandardsIgnoreEnd
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testNodeListSize()


}//end class

?>
