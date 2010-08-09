<?php
/**
 * Unit Tests for the ellipsisize method.
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
 * Unit Tests for the ellipsisize method.
 *
 */
class StringEllipsisizeUnitTest extends AbstractMySourceUnitTest
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
     * Test passing in invalid values for each parameter.
     *
     * @return void
     */
    public function testTypeVerification()
    {
        $expect = '';
        $result = String::ellipsisize(1, 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('', 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize(NULL, 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('Hello World', NULL);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('Hello World', 'Test');
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testTypeVerification()


    /**
     * Test minimum boundary cases of length argument.
     *
     * @return void
     */
    public function testMinimumLengthBoundary()
    {
        $expect = '';
        $result = String::ellipsisize('Hello World', -1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '...';
        $result = String::ellipsisize('Hello World', 0);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'H...';
        $result = String::ellipsisize('Hello World', 1);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testMinimumLengthBoundary()


    /**
     * Test General strings.
     *
     * @return void
     */
    public function testGeneralStrings()
    {
        $expect = 'Hel...';
        $result = String::ellipsisize('Hello World', 3);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hell...';
        $result = String::ellipsisize('Hello World', 4);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello...';
        $result = String::ellipsisize('Hello World', 5);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello...';
        $result = String::ellipsisize('Hello World', 6);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testGeneralStrings()


    /**
     * Test strings which are shorter than the length argument.
     *
     * @return void
     */
    public function testStringEqualOrShorterThanLength()
    {
        // Length is 11.
        $expect = 'Hello World';

        $result = String::ellipsisize('Hello World', 11);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $result = String::ellipsisize('Hello World', 12);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testStringEqualOrShorterThanLength()


    /**
     * Test strings which end with dot(s).
     *
     * @return void
     */
    public function testStringWithDots()
    {
        $expect = 'Hel...';
        $result = String::ellipsisize('Hello World.', 3);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hel...';
        $result = String::ellipsisize('Hello World..', 3);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 11);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 12);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 13);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = 'Hello World...';
        $result = String::ellipsisize('Hello World...', 14);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testStringWithDots()


}//end class

?>
