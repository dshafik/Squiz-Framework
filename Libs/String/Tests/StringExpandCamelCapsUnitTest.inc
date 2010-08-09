<?php
/**
 * Unit Tests for the expandCamelCaps function.
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
 * Unit tests for the expandCamelCaps function.
 *
 */
class StringExpandCamelCapsUnitTest extends AbstractMySourceUnitTest
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
     * Test expandCamelCaps.
     *
     * @return void
     */
    public function testExpandCamelCaps()
    {
        $expected = 'Hello world';
        $result   = String::expandCamelCaps('helloWorld');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello world from home';
        $result   = String::expandCamelCaps('helloWorldFromHome');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello';
        $result   = String::expandCamelCaps('hello');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello 123';
        $result   = String::expandCamelCaps('hello123');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello 123 world';
        $result   = String::expandCamelCaps('hello123World');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello AWorld';
        $result   = String::expandCamelCaps('helloAWorld');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Hello world';
        $result   = String::expandCamelCaps('helloWorld  ');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

        $expected = 'Get HTML';
        $result   = String::expandCamelCaps('getHTML');
        PHPUnit_Framework_Assert::assertEquals($expected, $result);

    }//end testExpandCamelCaps()


    /**
     * Test invalid values.
     *
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::expandCamelCaps(NULL);
        PHPUnit_Framework_Assert::assertEquals('', $result);

        $result = String::expandCamelCaps('');
        PHPUnit_Framework_Assert::assertEquals('', $result);

    }//end testWithInvalidValues()


}//end class

?>
