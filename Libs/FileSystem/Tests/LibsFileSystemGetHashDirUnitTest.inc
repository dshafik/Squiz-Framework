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
 * Unit test for FileSystem::getHashDir().
 *
 * @since 4.0.0
 */
class LibsFileSystemGetHashDirUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that the hash directory for id 1 is correct.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
