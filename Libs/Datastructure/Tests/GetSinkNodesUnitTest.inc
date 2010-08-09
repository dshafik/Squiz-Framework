<?php
/**
 * Unit Tests for getSinkNodes.
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
 * Unit Tests for getSinkNodes.
 *
 */
class GetSinkNodesUnitTest extends AbstractMySourceUnitTest
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
     * Test getSinkNodes.
     *
     * @return void
     */
    public function testGetSinkNodes()
    {
        $dag = new DirectedAcyclicGraph();

        $dag->connectNodes('1', '2');
        $dag->connectNodes('2', '3');
        $dag->connectNodes('3', '4');
        $dag->connectNodes('4', '5');

        $sourceNodes = $dag->getSinkNodes();

        PHPUnit_Framework_Assert::assertEquals(1, count($sourceNodes));
        PHPUnit_Framework_Assert::assertEquals(5, $sourceNodes[0]);

    }//end testGetSinkNodes()


}//end class

?>
