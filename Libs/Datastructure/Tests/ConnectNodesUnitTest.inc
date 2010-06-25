<?php
/**
 * Unit Tests for connectNodes.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for connectNodes.
 *
 * @since 4.0.0
 */
class ConnectNodesUnitTest extends AbstractMySourceUnitTest
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
     * Test connecting two nodes.
     *
     * @since  4.0.0
     * @return void
     */
    public function testConnectingNodes()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('1', '2'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('2', '1'));

        $dag->connectNodes('2', '3');
        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('2', '3'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('1', '3'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('3', '2'));

        // Conecting nodes that exist.
        $dag->connectNodes('2', '4');
        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('2', '4'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('4', '2'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('1', '3'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('1', '4'));

        // Connecting nodes that are already connected.
        $dag->connectNodes('3', '4');
        PHPUnit_Framework_Assert::assertTrue($dag->isLinked('3', '4'));
        PHPUnit_Framework_Assert::assertFalse($dag->isLinked('4', '3'));

    }//end testConnectingNodes()


}//end class

?>
