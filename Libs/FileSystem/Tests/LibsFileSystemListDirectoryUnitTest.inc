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
 * Unit test for FileSystem::listDirectory().
 *
 * Tests for
 *
 * @since 4.0.0
 */
class LibsFileSystemListDirectoryUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Test that listing the directory with no parameters returns all files.
     *
     * Files returned include those in nested directories.
     * No extensions are specified, nesting is on.
     *
     * @since  4.0.0
     * @return void
     */
    public function testBasicList()
    {
        $path   = dirname(__FILE__).'/TestDir/';
        $retval = FileSystem::listDirectory($path);
        PHPUnit_Framework_Assert::assertTrue(count($retval) === 13);

    }//end testBasicList()


    /**
     * Test that listing the base directory returns the correct number of files.
     *
     * Files returned are only those that lie in the base of the test dir
     * directory.
     * No extensions are specified, nesting is off.
     *
     * @since  4.0.0
     * @return void
     */
    public function testBasicListNoNesting()
    {
        $path   = dirname(__FILE__).'/TestDir/';
        $retval = FileSystem::listDirectory($path, array(), FALSE);
        PHPUnit_Framework_Assert::assertTrue(count($retval) === 5);

    }//end testBasicListNoNesting()


    /**
     * Test that listing the directory with Extensions returns correct files.
     *
     * Files returned include those in nested directories.
     * Extensions are (.inc, .php).
     * Nesting is on.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExtensionList()
    {
        $path       = dirname(__FILE__).'/TestDir/';
        $extensions = array(
                       '.inc',
                       '.php',
                      );
        $retval     = FileSystem::listDirectory($path, $extensions);
        PHPUnit_Framework_Assert::assertTrue(count($retval) === 7);

    }//end testExtensionList()


    /**
     * Test listing the root directory with Extensions returns correct files.
     *
     * Files returned are only those in the base of TestDir.
     * Extensions are (.txt, .xml).
     * Nesting is off.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExtensionListNesting()
    {
        $path       = dirname(__FILE__).'/TestDir/';
        $extensions = array(
                       '.txt',
                       '.xml',
                      );
        $retval     = FileSystem::listDirectory($path, $extensions, FALSE);
        PHPUnit_Framework_Assert::assertTrue(count($retval) === 2);

    }//end testExtensionListNesting()


    /**
     * Tests that the full path parameter returns only file names when false.
     *
     * Checks that c.php is in the returned array, as it is the only .php
     * file in the test directory.
     *
     * Extensions are (.php).
     * Nesting is on.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFileNameOnlyListing()
    {
        $path       = dirname(__FILE__).'/TestDir/';
        $extensions = array('.php');
        $retval     = FileSystem::listDirectory($path, $extensions, TRUE, FALSE);
        $expected   = array('c.php');
        PHPUnit_Framework_Assert::assertEquals($expected, $retval);

    }//end testFileNameOnlyListing()


    /**
     * Tests that the full path parameter returns full paths when true.
     *
     * Checks that c.php is not in the returned array, as it is the only .php
     * file in the test directory. If the returned array does not contain just
     * its' file name, then the full path has been returned.
     *
     * Extensions are (.php).
     * Nesting is on.
     *
     * @since  4.0.0
     * @return void
     */
    public function testFullPathListing()
    {
        $path       = dirname(__FILE__).'/TestDir/';
        $extensions = array('.php');
        $retval     = FileSystem::listDirectory($path, $extensions, TRUE, TRUE);
        $expected   = array($path.'c.php');
        PHPUnit_Framework_Assert::assertEquals($expected, $retval);

    }//end testFullPathListing()


}//end class

?>
