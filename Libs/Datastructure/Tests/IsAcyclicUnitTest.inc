<?php
/**
 * Unit Tests for isAcyclic.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Unit Tests for isAcyclic.
 *
 * @since 4.0.0
 */
class IsAcyclicUnitTest extends AbstractMySourceUnitTest
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
     * Test isAcyclic.
     *
     * @since  4.0.0
     * @return void
     */
    public function testIsAcyclic()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        $dag->connectNodes('2', '3');
        PHPUnit_Framework_Assert::assertTrue($dag->isAcyclic());

        $dag->connectNodes('2', '4');
        PHPUnit_Framework_Assert::assertTrue($dag->isAcyclic());

        $dag->connectNodes('3', '4');
        PHPUnit_Framework_Assert::assertTrue($dag->isAcyclic());

        $dag->connectNodes('3', '1');
        PHPUnit_Framework_Assert::assertFalse($dag->isAcyclic());

    }//end testIsAcyclic()


}//end class

?>
