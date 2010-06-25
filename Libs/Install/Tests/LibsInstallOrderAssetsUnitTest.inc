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
 * Unit tests for Install::orderAssets().
 *
 * NB: These tests check addAssetTree() but not directly.
 *
 * @since 4.0.0
 */
class LibsInstallOrderAssetsUnitTest extends AbstractMySourceUnitTest
{

    /**
     * Test types for the ordering of assets.
     *
     * @var   $_simpleTest The test types.
     * @since 4.0.0
     */
    private $_simpleTest = array(
                            array(
                             'type_code'   => 'type1',
                             'parent_type' => 'type2',
                            ),
                            array(
                             'type_code'   => 'type3',
                             'parent_type' => 'type13',
                            ),
                            array(
                             'type_code'   => 'type2',
                             'parent_type' => 'type4',
                            ),
                            array(
                             'type_code'   => 'type4',
                             'parent_type' => 'asset',
                            ),
                            array(
                             'type_code'   => 'type6',
                             'parent_type' => 'type4',
                            ),
                            array(
                             'type_code'   => 'type10',
                             'parent_type' => 'type6',
                            ),
                            array(
                             'type_code'   => 'type8',
                             'parent_type' => 'type10',
                            ),
                            array(
                             'type_code'   => 'type13',
                             'parent_type' => 'type8',
                            ),
                           );

    /**
     * A more complex array of test types to order.
     *
     * @var   $_complexTest The test types.
     * @since 4.0.0
     */
    private $_complexTest = array(
                             array(
                              'type_code'   => 'type1',
                              'parent_type' => 'type2',
                             ),
                             array(
                              'type_code'   => 'type3',
                              'parent_type' => 'type1',
                             ),
                             array(
                              'type_code'   => 'type2',
                              'parent_type' => 'type4',
                              'extra_info'  => '',
                             ),
                             array(
                              'type_code'   => 'type4',
                              'parent_type' => 'asset',
                              'extra_info'  => 'blank',
                             ),
                             array(
                              'type_code'   => 'type6',
                              'parent_type' => 'type4',
                             ),
                             array(
                              'type_code'   => 'type10',
                              'parent_type' => 'type6',
                             ),
                             array(
                              'type_code'   => 'type8',
                              'parent_type' => 'type4',
                              'extra_info'  => 'blank',
                             ),
                             array(
                              'type_code'   => 'type13',
                              'parent_type' => 'type14',
                             ),
                             array(
                              'type_code'   => 'type14',
                              'parent_type' => 'type1',
                             ),
                             array(
                              'type_code'   => 'type15',
                              'parent_type' => 'type13',
                             ),
                            );


    /**
     * Tests a set of assets that have a straight line dependency.
     *
     * @since  4.0.0
     * @return void
     */
    public function testOrderSystemsSimple()
    {
        $ret      = Install::orderAssets($this->_simpleTest);
        $expected = array(
                     'type4'  => array(
                                  'type_code'   => 'type4',
                                  'parent_type' => 'asset',
                                 ),
                     'type2'  => array(
                                  'type_code'   => 'type2',
                                  'parent_type' => 'type4',
                                 ),
                     'type1'  => array(
                                  'type_code'   => 'type1',
                                  'parent_type' => 'type2',
                                 ),
                     'type6'  => array(
                                  'type_code'   => 'type6',
                                  'parent_type' => 'type4',
                                 ),
                     'type10' => array(
                                  'type_code'   => 'type10',
                                  'parent_type' => 'type6',
                                 ),
                     'type8'  => array(
                                  'type_code'   => 'type8',
                                  'parent_type' => 'type10',
                                 ),
                     'type13' => array(
                                  'type_code'   => 'type13',
                                  'parent_type' => 'type8',
                                 ),
                     'type3'  => array(
                                  'type_code'   => 'type3',
                                  'parent_type' => 'type13',
                                 ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testOrderSystemsSimple()


    /**
     * Tests a set of assets that have a parents with multiple children.
     *
     * Also checks that any extra information in the assets array is retained
     * in the result.
     *
     * @since  4.0.0
     * @return void
     */
    public function testOrderSystemsComplex()
    {
        $ret      = Install::orderAssets($this->_complexTest);
        $expected = array(
                     'type4'  => array(
                                  'type_code'   => 'type4',
                                  'parent_type' => 'asset',
                                  'extra_info'  => 'blank',
                                 ),
                     'type2'  => array(
                                  'type_code'   => 'type2',
                                  'parent_type' => 'type4',
                                  'extra_info'  => '',
                                 ),
                     'type1'  => array(
                                  'type_code'   => 'type1',
                                  'parent_type' => 'type2',
                                 ),
                     'type14' => array(
                                  'type_code'   => 'type14',
                                  'parent_type' => 'type1',
                                 ),
                     'type3'  => array(
                                  'type_code'   => 'type3',
                                  'parent_type' => 'type1',
                                 ),
                     'type6'  => array(
                                  'type_code'   => 'type6',
                                  'parent_type' => 'type4',
                                 ),
                     'type10' => array(
                                  'type_code'   => 'type10',
                                  'parent_type' => 'type6',
                                 ),
                     'type8'  => array(
                                  'type_code'   => 'type8',
                                  'parent_type' => 'type4',
                                  'extra_info'  => 'blank',
                                 ),
                     'type13' => array(
                                  'type_code'   => 'type13',
                                  'parent_type' => 'type14',
                                 ),
                     'type15' => array(
                                  'type_code'   => 'type15',
                                  'parent_type' => 'type13',
                                 ),
                    );
        PHPUnit_Framework_Assert::assertEquals($expected, $ret);

    }//end testOrderSystemsComplex()


}//end class

?>
