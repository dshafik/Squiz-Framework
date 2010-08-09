<?php
/**
 * FileSystem library unit test.
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

require_once 'Libs/FileSystem/FileSystem.inc';

/**
 * Unit test for FileSystem::readableSize().
 *
 */
class LibsFileSystemReadableSizeUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test boundary for each file size.
     *
     * @return void
     */
    public function testBoundaryForEachFileSize()
    {
        $expect = '0.00'.FileSystem::FILE_SIZE_BYTE;
        $size   = 0;
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Test BYTES.
        $expect = '1.00'.FileSystem::FILE_SIZE_BYTE;
        $size   = 1;
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '2.00'.FileSystem::FILE_SIZE_BYTE;
        $size   = 2;
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Test KILOBYTES.
        $expect = '1023.00'.FileSystem::FILE_SIZE_BYTE;
        $size   = 1023;
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '1.00'.FileSystem::FILE_SIZE_KILOBYTE;
        $size   = 1024;
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '1.01'.FileSystem::FILE_SIZE_KILOBYTE;
        $size   = 1030;
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Test MEGABYTES.
        $expect = '1024.00'.FileSystem::FILE_SIZE_KILOBYTE;
        $size   = ((1024 * 1024) - 1);
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '1.00'.FileSystem::FILE_SIZE_MEGABYTE;
        $size   = (1024 * 1024);
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '1.01'.FileSystem::FILE_SIZE_MEGABYTE;
        $size   = ((1024 * 1024) + 10000);
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Test GIGABYTES.
        $expect = '1024.00'.FileSystem::FILE_SIZE_MEGABYTE;
        $size   = ((1024 * 1024 * 1024) - 1);
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '1.00'.FileSystem::FILE_SIZE_GIGABYTE;
        $size   = (1024 * 1024 * 1024);
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        $expect = '1.01'.FileSystem::FILE_SIZE_GIGABYTE;
        $size   = ((1024 * 1024 * 1024) + 6000000);
        $result = FileSystem::readableSize(NULL, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testBoundaryForEachFileSize()


    /**
     * Test converting the file size into a specific unit type.
     *
     * @return void
     */
    public function testConvertingFileSize()
    {
        // Convert KILOBYTES into BYTES.
        $size   = 1024;
        $unit   = FileSystem::FILE_SIZE_BYTE;
        $expect = '1024.00'.$unit;
        $result = FileSystem::readableSize(NULL, $unit, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Convert MEGABYTES into KILOBYTES.
        $size   = ((1024 * 1024) + 10000);
        $unit   = FileSystem::FILE_SIZE_KILOBYTE;
        $expect = '1033.77'.$unit;
        $result = FileSystem::readableSize(NULL, $unit, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Converts GIGABYTES into MEGABYTES.
        $size   = ((1024 * 1024 * 1024) + 6000000);
        $unit   = FileSystem::FILE_SIZE_MEGABYTE;
        $expect = '1029.72'.$unit;
        $result = FileSystem::readableSize(NULL, $unit, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Passing in TB returns GIGABYTES.
        $size   = (1024 * 1024 * 1024 * 1024);
        $unit   = 'TB';
        $expect = '1024.00'.FileSystem::FILE_SIZE_GIGABYTE;
        $result = FileSystem::readableSize(NULL, $unit, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testConvertingFileSize()


    /**
     * Test passing through a file gives a file size.
     *
     * @return void
     */
    public function testReturningFileSizeForAFile()
    {
        // Passing through a file with no size.
        $unit   = FileSystem::FILE_SIZE_BYTE;
        $result = (int) FileSystem::readableSize(__FILE__, $unit);
        PHPUnit_Framework_Assert::assertLessThan((1024 * 100), $result);

        // Passing through a file with size.
        $size   = (1024 * 1024);
        $expect = '1.00'.FileSystem::FILE_SIZE_MEGABYTE;
        $result = FileSystem::readableSize(__FILE__, NULL, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

    }//end testReturningFileSizeForAFile()


    /**
     * Test invalid values for unit and size.
     *
     * @return void
     */
    public function testInvalidValues()
    {
        // Passing through an invalid unit.
        $size   = (1024 * 1024);
        $expect = '1.00'.FileSystem::FILE_SIZE_MEGABYTE;
        $result = FileSystem::readableSize(NULL, 1, $size);
        PHPUnit_Framework_Assert::assertEquals($expect, $result);

        // Passing through an invalid size and no file.
        $result = FileSystem::readableSize(NULL, 1024, 'GB');
        PHPUnit_Framework_Assert::assertEquals(FALSE, $result);

        // Passing through an invalid size with a file.
        $result = (int) FileSystem::readableSize(__FILE__, NULL, 'GB');
        PHPUnit_Framework_Assert::assertLessThan((1024 * 1024), $result);

    }//end testInvalidValues()


}//end class

?>
