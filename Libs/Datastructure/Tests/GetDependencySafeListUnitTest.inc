<?php
/**
 * Unit Tests for getDependencySafeList.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for getDependencySafeList.
 *
 * @since 4.0.0
 */
class GetDependencySafeListUnitTest extends AbstractMySourceUnitTest
{


    /**
     * Sets up this unit test.
     *
     * This bakes out a working system that contains the systems that this
     * unit test requires to be able to run.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
