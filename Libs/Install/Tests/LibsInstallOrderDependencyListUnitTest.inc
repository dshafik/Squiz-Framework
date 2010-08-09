<?php
/**
 * Install Lib Tests.
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

require_once 'Libs/Install/Install.inc';

/**
 * Unit tests for Install::orderDependencyList().
 *
 */
class LibsInstallOrderDependencyListUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Tests that specifying empty list for the first arg will return array().
     *
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
