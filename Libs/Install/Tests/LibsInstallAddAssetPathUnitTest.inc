<?php
/**
 * Install Lib Tests.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage UnitTest
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Install/Install.inc';

/**
 * Unit tests for Install::addAssetPath().
 *
 * NB: Most methods in the Install Lib are used to install these tests so are
 * tested constantly. Some tests require user input, so are not tested here.
 * Methods not tested:
 *      - getInstallOptions()
 *      - getRealSystems() - Not able to determine which systems will exist.
 *      - installQueries()
 *      - installSystems()
 *      - listNestedDirectories()
 *      - runSystemInstallMethods()
 *
 * @since 4.0.0
 */
class LibsInstallAddAssetPathUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that addAssetPath stores the appropriate path.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddAssetPath()
    {
        Channels::includeSystem('AssetType');
        Install::addAssetPath('test1', dirname(__FILE__));
        $ret = AssetType::getTypePath('test1');
        PHPUnit_Framework_Assert::assertEquals($ret, dirname(__FILE__));

    }//end testAddAssetPath()


}//end class

?>
