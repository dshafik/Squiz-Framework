<?php
/**
 * Unit Tests for the getArrayIndex() action.
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

require_once 'Libs/Util/Util.inc';

/**
 * Unit Tests for the getArrayIndex() action.
 *
 */
class LibsUtilGetArrayIndexUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test correct result is returned with an array of items.
     *
     * @return void
     */
    public function testArrayWithItems()
    {
        $array = array(
                  'first item',
                  'second item',
                 );

        $result = Util::getArrayIndex($array, -1, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 0, NULL);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 1, NULL);
        PHPUnit_Framework_Assert::assertEquals('second item', $result);

        $result = Util::getArrayIndex($array, 2, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 'one', NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

    }//end testArrayWithItems()


    /**
     * Test correct result is returned with an array of named items.
     *
     * @return void
     */
    public function testArrayWithNamedItems()
    {
        $array = array(
                  'one' => 'first item',
                  'two' => 'second item',
                 );

        $result = Util::getArrayIndex($array, '', NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 'one', NULL);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 'two', NULL);
        PHPUnit_Framework_Assert::assertEquals('second item', $result);

        $result = Util::getArrayIndex($array, 'three', NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

        $result = Util::getArrayIndex($array, 1, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

    }//end testArrayWithNamedItems()


    /**
     * Test passing through a default value with an array.
     *
     * @return void
     */
    public function testPassingThroughDefaultValues()
    {
        // Array of items.
        $default = 'Default';
        $array   = array('first item');

        $result = Util::getArrayIndex($array, 0, $default);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 2, $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        // Array of named items.
        $array = array('one' => 'first item');

        $result = Util::getArrayIndex($array, '', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        $result = Util::getArrayIndex($array, 'one', $default);
        PHPUnit_Framework_Assert::assertEquals('first item', $result);

        $result = Util::getArrayIndex($array, 'three', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        // No array.
        $result = Util::getArrayIndex(NULL, 0, $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        $result = Util::getArrayIndex(NULL, '', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        // Different default value.
        $default = 6;

        $result = Util::getArrayIndex(NULL, 0, $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

        $result = Util::getArrayIndex(NULL, '', $default);
        PHPUnit_Framework_Assert::assertEquals($default, $result);

    }//end testPassingThroughDefaultValues()


    /**
     * Test passing through empty values.
     *
     * @return void
     */
    public function testPassingThroughNullValues()
    {
        $result = Util::getArrayIndex(NULL, NULL, NULL);
        PHPUnit_Framework_Assert::assertEquals(NULL, $result);

    }//end testPassingThroughNullValues()


}//end class

?>
