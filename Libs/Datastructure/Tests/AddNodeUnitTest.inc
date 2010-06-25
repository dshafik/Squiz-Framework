<?php
/**
 * Unit Tests for addNode.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for addNode.
 *
 * @since 4.0.0
 */
class AddNodeUnitTest extends AbstractMySourceUnitTest
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
     * Test that nodes are added to the graph.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddNode()
    {
        $dag = new DirectedAcyclicGraph();
        $dag->addNode('1');
        PHPUnit_Framework_Assert::assertTrue($dag->nodeExists('1'));

    }//end testAddNode()


    /**
     * Test adding the same node twice.
     *
     * @since  4.0.0
     * @return void
     */
    public function testAddingSameNodeTwice()
    {
        $dag = new DirectedAcyclicGraph();
        $dag->addNode('1');
        PHPUnit_Framework_Assert::assertTrue($dag->nodeExists('1'));

        // We expect an exception as we are passing in the same node.
        try {
            $dag->addNode('1');
        } catch (Exception $e) {
            PHPUnit_Framework_Assert::assertTrue(TRUE);
            return;
        }

        PHPUnit_Framework_Assert::fail('Expected exception');

    }//end testAddingSameNodeTwice()


}//end class

?>
