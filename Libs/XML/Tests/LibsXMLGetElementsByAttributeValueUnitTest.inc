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
 * Unit tests for the getElementsByAttributeValue() Method of XML.
 *
 */
class LibsXMLGetElementsByAttributeValueUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test for no attribute name.
     *
     * Test that getElementsByAttributeValue() return empty array when no
     * attribute name is given.
     *
     * @return void
     */
    public function testNoAttributeName()
    {
        $file = dirname(__FILE__).'/TestFiles/getElementsByAttributeValue.xml';
        $doc  = new DomDocument();
        $doc->load($file);

        $el1    = $doc->getElementsByTagName('test')->item(0);
        $retVal = XML::getElementsByAttributeValue($doc, $el1, '', 'cool');

        // Should be empty.
        PHPUnit_Framework_Assert::assertTrue(empty($retVal));

    }//end testNoAttributeName()


    /**
     * Test the content/order getElementsByTagNames().
     *
     * @return void
     */
    public function testSingleLevelSearch()
    {
        $file = dirname(__FILE__).'/TestFiles/getElementsByAttributeValue.xml';
        $doc  = new DomDocument();
        $doc->load($file);

        $el1    = $doc->getElementsByTagName('test')->item(0);
        $retVal = XML::getElementsByAttributeValue(
            $doc,
            $el1,
            'name',
            'cool',
            FALSE
        );

        // Should be empty.
        PHPUnit_Framework_Assert::assertEquals(2, $retVal->length);

    }//end testSingleLevelSearch()


    /**
     * Test the content/order getElementsByTagNames().
     *
     * @return void
     */
    public function testMultiLevelSearch()
    {
        $file = dirname(__FILE__).'/TestFiles/getElementsByAttributeValue.xml';
        $doc  = new DomDocument();
        $doc->load($file);

        $el1    = $doc->getElementsByTagName('test')->item(0);
        $retVal = XML::getElementsByAttributeValue($doc, $el1, 'name', 'cool');

        // Should be empty.
        PHPUnit_Framework_Assert::assertEquals(3, $retVal->length);

    }//end testMultiLevelSearch()


}//end class

?>
