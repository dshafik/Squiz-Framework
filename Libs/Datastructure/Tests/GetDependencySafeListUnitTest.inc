<?php
/**
 * Unit Tests for getDependencySafeList.
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

/**
 * Unit Tests for getDependencySafeList.
 *
 */
class GetDependencySafeListUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Sets up this unit test.
     *
     * This bakes out a working system that contains the systems that this
     * unit test requires to be able to run.
     *
     * @return void
     */
    protected function setUp()
    {
        parent::setUp();
        include_once 'Libs/Datastructure/Graph/DirectedAcyclicGraph.inc';

    }//end setUp()


    /**
     * Test getDependencySafeList with a simple list.
     *
     * @return void
     */
    public function testGetDependencySafeListWithSimpleList()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        $dag->connectNodes('2', '3');
        $dag->connectNodes('3', '4');
        $dag->connectNodes('4', '5');

        $nodeOrder = $dag->getDependencySafeList();

        PHPUnit_Framework_Assert::assertEquals('5', $nodeOrder[0]);
        PHPUnit_Framework_Assert::assertEquals('4', $nodeOrder[1]);
        PHPUnit_Framework_Assert::assertEquals('3', $nodeOrder[2]);
        PHPUnit_Framework_Assert::assertEquals('2', $nodeOrder[3]);
        PHPUnit_Framework_Assert::assertEquals('1', $nodeOrder[4]);

    }//end testGetDependencySafeListWithSimpleList()


    /**
     * Test array returned using the correct indexing.
     *
     * @return void
     */
    public function testDependencyListUsesCorrectIndexOrder()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        $dag->connectNodes('2', '3');
        $dag->connectNodes('3', '4');
        $dag->connectNodes('4', '5');
        $dag->connectNodes('1', '3');

        $nodeOrder = $dag->getDependencySafeList();

        PHPUnit_Framework_Assert::assertEquals('5', $nodeOrder[0]);
        PHPUnit_Framework_Assert::assertEquals('4', $nodeOrder[1]);
        PHPUnit_Framework_Assert::assertEquals('3', $nodeOrder[2]);
        PHPUnit_Framework_Assert::assertEquals('2', $nodeOrder[3]);
        PHPUnit_Framework_Assert::assertEquals('1', $nodeOrder[4]);

    }//end testDependencyListUsesCorrectIndexOrder()


    /**
     * Test getDependencySafeList when the order that is returned may change.
     *
     * @return void
     */
    public function testDependencyListWhereOrderMayBeDifferent()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('2', '1');
        $dag->connectNodes('3', '1');

        $nodeOrder = $dag->getDependencySafeList();
        $firstPotenialOrder = array(
                               1,
                               2,
                               3,
                              );

        $secondPotenialOrder = array(
                                1,
                                3,
                                2,
                               );

        PHPUnit_Framework_Assert::assertTrue(
            $nodeOrder === $firstPotenialOrder ||
            $nodeOrder == $secondPotenialOrder
        );

    }//end testDependencyListWhereOrderMayBeDifferent()


    /**
     * Test getDependencySafeList when there is a loop.
     *
     * @return void
     */
    public function testGetDependencySafeListWhenLoop()
    {
         $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        $dag->connectNodes('2', '3');
        $dag->connectNodes('3', '1');

        // We expect an exception as these is a loop.
        try {
            $dag->getDependencySafeList();
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::assertTrue(TRUE);
            return;
        }

        PHPUnit_Framework_Assert::fail('Expected exception');

    }//end testGetDependencySafeListWhenLoop()


}//end class

?>
