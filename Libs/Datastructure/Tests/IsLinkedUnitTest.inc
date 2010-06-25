<?php
/**
 * Unit Tests for isLinked.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for isLinked.
 *
 * @since 4.0.0
 */
class IsLinkedUnitTest extends AbstractMySourceUnitTest
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
     * Test nodes are linked.
     *
     * @since  4.0.0
     * @return void
     */
    public function testIsLinked()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        $dag->connectNodes('2', '3');
        $dag->connectNodes('1', '4');

        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('1', '2'));
        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('2', '3'));
        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('1', '4'));

        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('2', '1'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('3', '2'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('4', '1'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('1', '3'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('2', '4'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('3', '4'));

    }//end testIsLinked()


    /**
     * Test getting exception for nodes that don't exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testExpection()
    {
        $dag = new DirectedAcyclicGraph();

        // We expect an exception as we are passing in nodes that don't exist.
        try {
            $dag->isLinked('4', '5');
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::assertTrue(TRUE);
            return;
        }

        PHPUnit_Framework_Assert::fail('Expected exception');

    }//end testExpection()


}//end class

?>
