<?php
/**
 * Unit Tests for addNode.
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
 * Unit Tests for addNode.
 *
 */
class AddNodeUnitTest extends AbstractMySourceUnitTest
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
     * Test that nodes are added to the graph.
     *
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
