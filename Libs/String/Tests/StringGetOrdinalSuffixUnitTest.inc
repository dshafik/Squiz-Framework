<?php
/**
 * Unit Tests for the getOrGetOrdinalSuffix function.
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

/**
 * Unit tests for the getOrdinalSuffix function.
 *
 */
class StringGetOrdinalSuffixUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Sets up this unit test.
     *
     * This bakes out a working system that contains the systems that this
     * unit test requires to be able to run.
     *
     * @return void
     */
    protected function setUp()
    {
        parent::setUp();
        include_once 'Libs/String/String.inc';

    }//end setUp()


    /**
     * Test getOrdinalSuffix.
     *
     * @return void
     */
    public function testGetOrdinalSuffix()
    {
        $expected = 'st';
        $result   = String::getOrdinalSuffix(1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'rd';
        $result   = String::getOrdinalSuffix(3);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(4);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(19);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(20);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'st';
        $result   = String::getOrdinalSuffix(21);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'nd';
        $result   = String::getOrdinalSuffix(32);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'rd';
        $result   = String::getOrdinalSuffix(43);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'th';
        $result   = String::getOrdinalSuffix(54);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testGetOrdinalSuffix()


    /**
     * Test invalid values.
     *
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::getOrdinalSuffix('Number');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::getOrdinalSuffix('Number 123');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::getOrdinalSuffix(NULL);
        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testWithInvalidValues()


}//end class

?>
