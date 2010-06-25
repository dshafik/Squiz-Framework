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
 * Unit tests for Install::orderDependencyList().
 *
 * @since 4.0.0
 */
class LibsInstallOrderDependencyListUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that specifying empty list for the first arg will return array().
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyList()
    {
        $list         = array();
        $dependencies = array(
                         'A' => array('B'),
                         'B' => array(),
                         'C' => array(),
                         'D' => array(),
                        );

        $expeceted = $list;

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testEmptyList()


    /**
     * Empty dependency test.
     *
     * Tests that specifying empty dependencies list for the second arg will
     * return the list array.
     *
     * @since  4.0.0
     * @return void
     */
    public function testEmptyDependencies()
    {
        $list = array(
                 'A',
                 'B',
                 'C',
                 'D',
                );
        $dependencies = array();

        $expeceted = $list;

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testEmptyDependencies()


    /**
     * Tests that specifying no dependencies will return the same list.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNoDependencies()
    {
        $list = array(
                 'A',
                 'B',
                 'C',
                 'D',
                );

        $dependencies = array(
                         'A' => array(),
                         'B' => array(),
                         'C' => array(),
                         'D' => array(),
                        );

        $expeceted = $list;

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testNoDependencies()


    /**
     * Tests that orderDependencyList returns correct results.
     *
     * @since  4.0.0
     * @return void
     */
    public function testDependencyListOne()
    {
        $list = array(
                 'A',
                 'B',
                 'C',
                 'D',
                 'E',
                );

        $dependencies = array(
                         'A' => array(
                                 'B',
                                 'C',
                                 'D',
                                ),
                         'B' => array('D'),
                         'C' => array('D'),
                         'D' => array(),
                         'E' => array('B'),
                        );

        $expeceted = array(
                      'D',
                      'C',
                      'B',
                      'A',
                      'E',
                     );

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testDependencyListOne()


    /**
     * Tests that orderDependencyList returns correct results.
     *
     * @since  4.0.0
     * @return void
     */
    public function testDependencyListTwo()
    {
        $list = array(
                 'B',
                 'A',
                 'C',
                 'D',
                 'E',
                );

        $dependencies = array(
                         'A' => array(
                                 'B',
                                 'C',
                                 'D',
                                ),
                         'B' => array('D'),
                         'C' => array('D'),
                         'D' => array(),
                         'E' => array('B'),
                        );

        $expeceted = array(
                      'D',
                      'C',
                      'B',
                      'A',
                      'E',
                     );

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testDependencyListTwo()


    /**
     * Tests that orderDependencyList returns correct results.
     *
     * @since  4.0.0
     * @return void
     */
    public function testDependencyListThree()
    {
        $list = array(
                 'A',
                 'B',
                 'C',
                 'D',
                 'E',
                );

        $dependencies = array(
                         'A' => array(
                                 'B',
                                 'C',
                                 'D',
                                ),
                         'B' => array('E'),
                         'C' => array('D'),
                         'D' => array(),
                         'E' => array('D'),
                        );

        $expeceted = array(
                      'D',
                      'E',
                      'C',
                      'B',
                      'A',
                     );

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testDependencyListThree()


    /**
     * Tests that orderDependencyList returns correct results.
     *
     * @since  4.0.0
     * @return void
     */
    public function testDependencyListFour()
    {
        $list = array(
                 'A',
                 'B',
                 'C',
                 'D',
                 'E',
                );

        $dependencies = array(
                         'A' => array(
                                 'B',
                                 'C',
                                 'D',
                                ),
                         'B' => array('E'),
                         'C' => array('E'),
                         'D' => array(),
                         'E' => array('D'),
                        );

        $expeceted = array(
                      'D',
                      'E',
                      'C',
                      'B',
                      'A',
                     );

        $res = Install::orderDependencyList($list, $dependencies);

        PHPUnit_Framework_Assert::assertEquals($res, $expeceted);

    }//end testDependencyListFour()


}//end class

?>
