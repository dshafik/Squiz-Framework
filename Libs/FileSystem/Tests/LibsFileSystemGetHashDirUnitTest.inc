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
 * Unit test for FileSystem::getHashDir().
 *
 */
class LibsFileSystemGetHashDirUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that the hash directory for id 1 is correct.
     *
     * @return void
     */
    public function testHashDir1()
    {
        $dir      = FileSystem::getHashDir(1);
        $expected = '1-10000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDir1()


    /**
     * Tests that the hash directory for id 2 is correct.
     *
     * @return void
     */
    public function testHashDir2()
    {
        $dir      = FileSystem::getHashDir(2);
        $expected = '1-10000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDir2()


    /**
     * Tests that the hash directory for id 10001 is correct.
     *
     * @return void
     */
    public function testHashDir10001()
    {
        $dir      = FileSystem::getHashDir(10001);
        $expected = '10001-20000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDir10001()


    /**
     * Tests that the hash directory for id 20001 is correct.
     *
     * @return void
     */
    public function testHashDir20001()
    {
        $dir      = FileSystem::getHashDir(20001);
        $expected = '20001-30000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDir20001()


    /**
     * Tests that the hash directory for id 10001 is correct.
     *
     * @return void
     */
    public function testHashDir1000450()
    {
        $dir      = FileSystem::getHashDir(1000450);
        $expected = '1000001-1010000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDir1000450()


    /**
     * Tests that the hash directory for id 0 is correct.
     *
     * @return void
     */
    public function testHashDir0()
    {
        $dir      = FileSystem::getHashDir(0);
        $expected = '0';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDir0()


    /**
     * Tests that the hash directory for negative id (shadow asset) is correct.
     *
     * @return void
     */
    public function testHashDirNegative()
    {
        $dir      = FileSystem::getHashDir(-1);
        $expected = '-1-10000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

        $dir      = FileSystem::getHashDir(-2);
        $expected = '-1-10000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

        $dir      = FileSystem::getHashDir(-9999);
        $expected = '-1-10000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

        $dir      = FileSystem::getHashDir(-10000);
        $expected = '-1-10000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

        $dir      = FileSystem::getHashDir(-10001);
        $expected = '-10001-20000';
        PHPUnit_Framework_Assert::assertEquals($expected, $dir);

    }//end testHashDirNegative()


    /**
     * Also test Asset::getAssetDataDir() method that is based on this.
     *
     * Note: If this test fail, double check client system's dir structure.
     *
     * @return void
     */
    public function testGetAssetDataDir()
    {
        Channels::includeSystem('Asset');
        $dir      = BaseSystem::getDataDir('Asset');
        $assetids = array(
                     1,
                     2,
                     10001,
                     20001,
                     1000450,
                     0,
                     -1,
                     -2,
                     -9999,
                     -10000,
                     -10001,
                     31999,
                     32000,
                     32001,
                     63999,
                     64000,
                     64001,
                     -31999,
                     -32000,
                     -32001,
                     -63999,
                     -64000,
                     -64001,
                    );
        $expected = array(
                     '1-32000',
                     '1-32000',
                     '1-32000',
                     '1-32000',
                     '992001-1024000',
                     '0',
                     '-1-32000',
                     '-1-32000',
                     '-1-32000',
                     '-1-32000',
                     '-1-32000',
                     '1-32000',
                     '1-32000',
                     '32001-64000',
                     '32001-64000',
                     '32001-64000',
                     '64001-96000',
                     '-1-32000',
                     '-1-32000',
                     '-32001-64000',
                     '-32001-64000',
                     '-32001-64000',
                     '-64001-96000',
                    );

        foreach ($assetids as $i => $assetid) {
            $ret = Asset::getAssetDataDir($assetid);
            $exp = $dir.'/'.$expected[$i].'/'.$assetid;
            PHPUnit_Framework_Assert::assertEquals($exp, $ret);
        }

    }//end testGetAssetDataDir()


}//end class

?>
