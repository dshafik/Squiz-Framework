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
 * Unit test for FileSystem::clearDirectory().
 *
 * @since 4.0.0
 */
class LibsFileSystemClearDirectoryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Path to the test dir.
     *
     * @var   string
     * @since 4.0.0
     */
    private static $_path = '';


    /**
     * Sets up the directory structure required for this unit test.
     *
     * @since  4.0.0
     * @return void
     */
    protected function setUp()
    {
        parent::setUp();

        self::$_path = dirname(__FILE__).'/TestDir/clearDirTest';
        mkdir(self::$_path);
        mkdir(self::$_path.'/dir1');
        mkdir(self::$_path.'/dir2');
        touch(self::$_path.'/test1.txt');
        touch(self::$_path.'/test2.js');
        touch(self::$_path.'/test3.txt');

    }//end setUp()


    /**
     * Undos all the setup for this unit test.
     *
     * @since  4.0.0
     * @return void
     */
    protected function tearDown()
    {
        parent::tearDown();
        exec('rm -rf '.self::$_path);

    }//end tearDown()


    /**
     * Test that clearDirectory() with deleteRoot === FALSE leaves an empty dir.
     *
     * @since  4.0.0
     * @return void
     */
    public function testClearNoDeleteRoot()
    {
        $path = self::$_path;
        FileSystem::clearDirectory($path, FALSE);
        $dirList = FileSystem::listDirectory($path);
        PHPUnit_Framework_Assert::assertTrue(empty($dirList));

    }//end testClearNoDeleteRoot()


    /**
     * Test that clearDirectory() with deleteRoot === TRUE removes everything.
     *
     * @since  4.0.0
     * @return void
     */
    public function testClearDeleteRoot()
    {
        $path = self::$_path;
        FileSystem::clearDirectory($path, TRUE);
        PHPUnit_Framework_Assert::assertFalse(file_exists($path));

    }//end testClearDeleteRoot()


    /**
     * Test that clearDirectory() returns FALSE for non-existent path.
     *
     * @since  4.0.0
     * @return void
     */
    public function testClearInvalidPath()
    {
        $path   = self::$_path.'/invalidpath';
        $retval = FileSystem::clearDirectory($path);
        PHPUnit_Framework_Assert::assertFalse($retval);

    }//end testClearInvalidPath()


    /**
     * Test clearDirectory() omit file feature.
     *
     * @since  4.0.0
     * @return void
     */
    public function testClearOmitFile()
    {
        $path     = self::$_path;
        $status   = FileSystem::clearDirectory($path, FALSE, array('test2.js'));
        $list     = FileSystem::listDirectory($path);
        $expected = array($path.'/test2.js');
        PHPUnit_Framework_Assert::assertEquals($expected, $list);
        PHPUnit_Framework_Assert::assertTrue($status);

    }//end testClearOmitFile()


}//end class

?>
