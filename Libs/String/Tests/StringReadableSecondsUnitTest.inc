<?php
/**
 * Unit Tests for the readableSeconds function.
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
 * Unit tests for the readableSeconds function.
 *
 */
class StringReadableSecondsUnitTest extends AbstractMySourceUnitTest
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
     * Test the singular and plural versions of all time units.
     *
     * @return void
     */
    public function testPlurals()
    {
        $expected = '1 second';
        $result   = String::readableSeconds(1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 seconds';
        $result   = String::readableSeconds(2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 minute';
        $result   = String::readableSeconds(61);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 minutes';
        $result   = String::readableSeconds(120);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 hour';
        $result   = String::readableSeconds(3601);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 hours';
        $result   = String::readableSeconds(7200);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 day';
        $result   = String::readableSeconds(86401);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 days';
        $result   = String::readableSeconds(86400 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 week';
        $result   = String::readableSeconds(604801);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 weeks';
        $result   = String::readableSeconds(604800 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 month';
        $result   = String::readableSeconds(2419201);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 months';
        $result   = String::readableSeconds(2419200 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '1 year';
        $result   = String::readableSeconds(31536001);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '2 years';
        $result   = String::readableSeconds(31536000 * 2);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testPlurals()


    /**
     * Test boundary cases where strings roll over.
     *
     * @return void
     */
    public function testBoundaries()
    {
        $expected = '0 seconds';
        $result   = String::readableSeconds(0);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '60 seconds';
        $result   = String::readableSeconds(60);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '60 minutes';
        $result   = String::readableSeconds(3600);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '24 hours';
        $result   = String::readableSeconds(86400);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '7 days';
        $result   = String::readableSeconds(604800);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '4 weeks';
        $result   = String::readableSeconds(2419200);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '13 months';
        $result   = String::readableSeconds(31536000);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testBoundaries()


    /**
     * Test negative second values.
     *
     * @return void
     */
    public function testNegativeValues()
    {
        $expected = '-1 seconds';
        $result   = String::readableSeconds(-1);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '-61 seconds';
        $result   = String::readableSeconds(-61);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = '-6000 seconds';
        $result   = String::readableSeconds(-6000);
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testNegativeValues()


}//end class

?>
