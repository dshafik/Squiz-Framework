<?php
/**
 * Unit Tests for the secondsToHms function.
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
 * Unit tests for the secondsToHms function.
 *
 */
class StringSecondsToHmsUnitTest extends AbstractMySourceUnitTest
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
     * Test secondsToHms.
     *
     * @return void
     */
    public function testSecondsToHms()
    {
        $expected = '0:00';
        $result   = String::secondsToHms(0);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '0:01';
        $result   = String::secondsToHms(1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '0:59';
        $result   = String::secondsToHms(59);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00';
        $result   = String::secondsToHms(60);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:01';
        $result   = String::secondsToHms(61);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '59:59';
        $result   = String::secondsToHms(3599);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00:00';
        $result   = String::secondsToHms(3600);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00:01';
        $result   = String::secondsToHms(3601);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:00:59';
        $result   = String::secondsToHms(3659);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:01:00';
        $result   = String::secondsToHms(3660);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1:59:59';
        $result   = String::secondsToHms(7199);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testSecondsToHms()


    /**
     * Test invalid values.
     *
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::secondsToHms(NULL);
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

        $result = String::secondsToHms('');
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

        $result = String::secondsToHms(-1);
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

        $result = String::secondsToHms('A');
        PHPUnit_Framework_Assert::assertEquals('0:00', $result);

    }//end testWithInvalidValues()


}//end class

?>
