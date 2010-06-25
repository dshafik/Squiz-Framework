<?php
/**
 * FileSystem library unit test.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/FileSystem/FileSystem.inc';

/**
 * Unit test for FileSystem::readableSize().
 *
 * @since 4.0.0
 */
class LibsFileSystemReadableSizeUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test boundary for each file size.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
