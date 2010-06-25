<?php
/**
 * Unit Tests for nodeExists.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for nodeExists.
 *
 * @since 4.0.0
 */
class NodeExistsUnitTest extends AbstractMySourceUnitTest
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
     * Test that node do and do not exist.
     *
     * @since  4.0.0
     * @return void
     */
    public function testNodeExists()
    {
        $dag = new DirectedAcyclicGraph();
        $dag->addNode('1');

        // Node exists.
        PHPUnit_Framework_Assert::assertTrue($dag->nodeExists('1'));

        // Node does not exist.
        PHPUnit_Framework_Assert::assertFalse($dag->nodeExists('2'));

    }//end testNodeExists()


}//end class

?>
