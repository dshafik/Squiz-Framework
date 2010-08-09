<?php
/**
 * Unit tests for Attribute::setAttribute().
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
 * @subpackage Attribute
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Unit tests for special characters in page_content attribute.
 *
 */
class AttributeSetAttributeValueUnitTest extends AbstractDataCleanUnitTest
{


    /**
     * Test all characters available on the US keyboard.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAllCharacters()
    {
        $tid = 'oneLevel1';
        $this->createAssetStructure($tid);
        Channels::includeSystem('Attribute');

        $string = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`~!@#$%^&*()-_=+{}[]\|;\':",<.>/?';
        Attribute::setAttributeValue($this->asts[$tid]['cid1'], 'page_content', $string);
        $value = Attribute::getAttributeValue($this->asts[$tid]['cid1'], 'page_content');
        PHPUnit_Framework_Assert::assertEquals($string, $value);

    }//end testAllCharacters()


}//end class

?>