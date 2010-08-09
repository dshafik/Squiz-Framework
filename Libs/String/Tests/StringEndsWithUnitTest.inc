<?php
/**
 * Unit Tests for the endsWith function.
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
 * Unit tests for the endsWith function.
 *
 */
class StringEndsWithUnitTest extends AbstractMySourceUnitTest
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
     * Test endsWith.
     *
     * @return void
     */
    public function testEndsWith()
    {
        $result = String::endsWith('Hello World', 'World');
        PHPUnit_Framework_Assert::assertTrue($result);

        $result = String::endsWith('Hello World', ' World');
        PHPUnit_Framework_Assert::assertTrue($result);

        $result = String::endsWith('Hello World', 'Hello World');
        PHPUnit_Framework_Assert::assertTrue($result);

        $result = String::endsWith('Hello World', 'World.');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith('Hello World', 'Test Hello World');
        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testEndsWith()


    /**
     * Test invalid values.
     *
     * @return void
     */
    public function testWithInvalidValues()
    {
        $result = String::endsWith(NULL, 'World');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith('Hello World', NULL);
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith(NULL, NULL);
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith(1, 'World.');
        PHPUnit_Framework_Assert::assertFalse($result);

        $result = String::endsWith('Hello World', 1);
        PHPUnit_Framework_Assert::assertFalse($result);

    }//end testWithInvalidValues()


}//end class

?>
