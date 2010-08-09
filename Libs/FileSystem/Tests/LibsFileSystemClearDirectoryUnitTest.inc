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
 * Unit test for FileSystem::clearDirectory().
 *
 */
class LibsFileSystemClearDirectoryUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Path to the test dir.
     *
     * @var   string
     */
    private static $_path = '';


    /**
     * Sets up the directory structure required for this unit test.
     *
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
