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
 * Unit tests for Install::getInstallOrder().
 *
 * @since 4.0.0
 */
class LibsInstallGetInstallOrderUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that passing getInstallOrder() orders systems appropriately.
     *
     * @since  4.0.0
     * @return void
     */
    public function testGetInstallOrder()
    {
        $path = dirname(dirname(dirname(dirname(__FILE__))));

        $systemNames = array(
                        $path.'/Systems/Attribute',
                        $path.'/Systems/LocalAsset',
                        $path.'/Systems/AssetType',
                       );

        $expected = array(
                     $path.'/Systems/AssetType',
                     $path.'/Systems/Attribute',
                     $path.'/Systems/LocalAsset',
                    );

        $ret = Install::getInstallOrder($systemNames);

        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testGetInstallOrder()


}//end class

?>
