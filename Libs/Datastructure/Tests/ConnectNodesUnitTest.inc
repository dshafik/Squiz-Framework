<?php
/**
 * Unit Tests for connectNodes.
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
 * Unit Tests for connectNodes.
 *
 */
class ConnectNodesUnitTest extends AbstractMySourceUnitTest
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
     * Test connecting two nodes.
     *
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
