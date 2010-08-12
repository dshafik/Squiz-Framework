<?php
/**
 * Datastructure Lib.
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

require_once 'Libs/Datastructure/AbstractNode.inc';
require_once 'Libs/Datastructure/Dependency/SystemNode.inc';
require_once 'Libs/Datastructure/Graph/AbstractGraph.inc';

/**
 * DirectedAcyclicGraph class.
 */
class DirectedAcyclicGraph extends AbstractGraph
{

    /**
     * The list of cycle strings found in the graph.
     *
     * @var array
     */
    protected $cycles = array();

    /**
     * The list of nodes containing cycle(s).
     *
     * @var array
     */
    protected $cycleNodes = array();


    /**
     * Constructor.
     */
    public function __construct()
    {

    }//end __construct()


    /**
     * Returns TRUE if the given nodeid exists in the graph.
     *
     * @param string $nodeid The id of the node.
     *
     * @return boolean
     */
    public function nodeExists($nodeid)
    {
        if (array_key_exists($nodeid, $this->nodes) === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end nodeExists()


    /**
     * Register the node to the graph.
     *
     * @param AbstractNode &$node The reference to the node object.
     *
     * @return void
     * @throws Exception The given node already exists.
     */
    private function _registerNode(AbstractNode &$node)
    {
        $nodeid = $node->getId();
        if ($this->nodeExists($node->getId()) === TRUE) {
            $msg = 'Can not register the same node more than once.';
            throw new Exception($msg);
        }

        $this->nodes[$nodeid] =& $node;

    }//end _registerNode()


    /**
     * Add a new node to aCyclicGraph.
     *
     * @param string $nodeid Node id.
     *
     * @return void
     */
    public function addNode($nodeid)
    {
        $sysNode = new SystemNode($nodeid, $this);
        $this->_registerNode($sysNode);

    }//end addNode()


    /**
     * Connect two nodes.
     *
     * @param object $fromNodeid The node obj connects from.
     * @param object $toNodeid   The node obj connects to.
     *
     * @return void
     * @throws Exception Error occurred during the connection.
     */
    public function connectNodes($fromNodeid, $toNodeid)
    {
        if ($this->nodeExists($fromNodeid) === FALSE) {
            $sysNode = new SystemNode($fromNodeid, $this);
            $this->_registerNode($sysNode);
        }

        if ($this->nodeExists($toNodeid) === FALSE) {
            $systemNode = new SystemNode($toNodeid, $this);
            $this->_registerNode($systemNode);
        }

        $toNode =& $this->nodes[$toNodeid];
        try {
            // Check if nodes are already connected.
            $outs = $this->nodes[$fromNodeid]->getOutIds();
            if (in_array($toNodeid, $outs) === FALSE) {
                $this->nodes[$fromNodeid]->connectTo($toNode);
            }
        } catch (Exception $e) {
            $msg = 'Error occurred while connecting two nodes:';
            throw new Exception($msg.$e->getMessage());
        }

    }//end connectNodes()


    /**
     * Gets the reverse order of the nodes in the graph.
     *
     * @return array The array of nodes in reverse order.
     * @throws Exception Cycle exists in the graph.
     */
    public function getDependencySafeList()
    {
        if ($this->isAcyclic() === FALSE) {
            $msg = 'Cycle exists in the graph.';
            foreach ($this->nodes as $nodeid => &$node) {
                $node->acyclicTestCleanup();
            }

            throw new Exception($msg);
        }

        $list  = array();
        $sinks = $this->getSinkNodes();

        $continue = TRUE;
        do {
            $inNodes = array();
            foreach ($sinks as $sinkNodeid) {
                $prevIdx = array_search($sinkNodeid, $list);

                // Special case bug fix. If the sinkNodeid was already added
                // to the list, then it means it still depends on one of
                // node in the previous loop. Remove previously added node
                // and add it to the end.
                if ($prevIdx !== FALSE) {
                    unset($list[$prevIdx]);
                }

                $list[]  = $sinkNodeid;
                $inIds   = $this->nodes[$sinkNodeid]->getInIds();
                $inNodes = array_merge($inNodes, $inIds);
            }

            $sinks = array_unique($inNodes);

            if (empty($sinks) === TRUE) {
                $continue = FALSE;
            }
        } while ($continue);

        $list = array_values($list);
        return $list;

    }//end getDependencySafeList()


    /**
     * Tests whether there is a loop in the graph.
     *
     * It returns TRUE if no loop cycle exists in the graph.
     *
     * @return boolean
     */
    public function isAcyclic()
    {
        // Let's set some initial data to test acyclic.
        $sinkNodes = array();
        foreach ($this->nodes as $nodeid => &$node) {
            $node->acyclicTestSetData();
            if ($node->outDegree() === 0) {
                $sinkNodes[] =& $node;
            }
        }

        $nodeList = array_flip(array_keys($this->nodes));

        // No sinks exist in the graph, then it's definitely cyclic.
        if (count($nodeList) > 0 && count($sinkNodes) === 0) {
            $this->_setCycles($nodeList);
            return FALSE;
        }

        $continue = TRUE;
        do {
            // Loop every sink nodes and peel them off from the graph.
            // Once all the links to the sink node is removed, visited flag
            // becomes true.
            foreach ($sinkNodes as &$sinkNode) {
                $sinkNodeid = $sinkNode->getId();
                foreach ($sinkNode->acyclicTestGetInData() as $nodeid => $val) {
                    $this->nodes[$nodeid]->acyclicTestDelOutNode($sinkNodeid);
                }

                unset($nodeList[$sinkNodeid]);
            }

            // Is there more sink nodes?
            $sinkNodes = array();
            foreach (array_keys($nodeList) as $nodeid) {
                $node =& $this->nodes[$nodeid];
                if ($node->acyclicTestIsSinkNode() === TRUE) {
                    $sinkNodes[] =& $node;
                }
            }

            // No sink nodes left, break the loop.
            if (empty($sinkNodes) === TRUE) {
                $continue = FALSE;
            }
        } while ($continue);

        // All the sink nodes are removed, but there are sill some node
        // hasn't been visited. Cycle exists in the graph.
        if (empty($nodeList) === FALSE) {
            $this->_setCycles($nodeList);
            return FALSE;
        }

        return TRUE;

    }//end isAcyclic()


    /**
     * Returns the list of cycles string.
     *
     * @return array
     */
    public function getCycles()
    {
        return $this->cycles;

    }//end getCycles()


    /**
     * Recursive function to determine the cycle.
     *
     * @param array $nodeList The list of nodes which the cycle exists.
     *
     * @return void
     */
    private function _setCycles(array $nodeList)
    {
        // To detect the cycle, first remove all the source nodes until
        // no more source nodes left. It will eventually give us only
        // the nodes with both of in and out going links, which means cycle.
        $nodesToProcess = $nodeList;
        do {
            $continue = FALSE;
            foreach ($nodeList as $key => $val) {
                if ($this->nodes[$key]->acyclicTestIsSourceNode() !== TRUE) {
                    continue;
                }

                unset($nodesToProcess[$key]);
                foreach ($this->nodes[$key]->acyclicTestGetOutData() as $nodeid => $val) {
                    $this->nodes[$nodeid]->acyclicTestDelInNode($key);
                }
            }

            $nodeList = $nodesToProcess;
            foreach ($nodeList as $key => $val) {
                if ($this->nodes[$key]->acyclicTestIsSourceNode() === TRUE) {
                    $continue = TRUE;
                }
            }
        } while ($continue);

        $this->cycleNodes = $nodesToProcess;
        foreach ($this->cycleNodes as $nodeid => $val) {
            $this->_setCycle($nodeid);
            break;
        }

    }//end _setCycles()


    /**
     * Recursive function to determine the cycle.
     *
     * @param string $nodeid The current nodeid.
     * @param array  $list   The list of nodes have been processed.
     *
     * @return void
     */
    private function _setCycle($nodeid, array $list=array())
    {
        if (empty($list) === FALSE && in_array($nodeid, $list) === TRUE) {
            $key = array_search($nodeid, $list);
            if ($key !== FALSE) {
                $list           = array_slice($list, $key);
                $list[]         = $nodeid;
                $this->cycles[] = implode(' -> ', $list);
            }
        } else {
            $list[] = $nodeid;
            foreach ($this->nodes[$nodeid]->acyclicTestGetOutData() as $id => $val) {
                $this->_setCycle($id, $list);
            }
        }

    }//end _setCycle()


    /**
     * Returns the list of source nodes.
     *
     * Sourcenode is the node with outgoing link but with no incoming link.
     *
     * @return array
     */
    public function getSourceNodes()
    {
        $ids = array();
        foreach ($this->nodes as $nodeid => &$node) {
            if ($node->inDegree() === 0) {
                $ids[] = $nodeid;
            }
        }

        return $ids;

    }//end getSourceNodes()


    /**
     * Returns the list of sink nodes.
     *
     * Sourcenode is the node with ingoing link but with no outgoing link.
     *
     * @return array
     */
    public function getSinkNodes()
    {
        $ids = array();
        foreach ($this->nodes as $nodeid => &$node) {
            if ($node->outDegree() === 0) {
                $ids[] = $nodeid;
            }
        }

        return $ids;

    }//end getSinkNodes()


    /**
     * Return the list of nodes in the graph.
     *
     * @return array
     */
    public function getNodes()
    {
        return $this->nodes;

    }//end getNodes()


    /**
     * Checks to see if two nodes are linked.
     *
     * @param string $fromid The nodeid to check the link from.
     * @param string $toid   The nodeid to check the link to.
     *
     * @return boolean
     * @throws Exception The given nodes don't exist.
     */
    public function isLinked($fromid, $toid)
    {
        if ($this->nodeExists($fromid) === FALSE) {
            $msg = 'From node does not exist';
            throw new Exception($msg);
        }

        if ($this->nodeExists($toid) === FALSE) {
            $msg = 'To node does not exist';
            throw new Exception($msg);
        }

        $isLinked = FALSE;

        $fromNodes = $this->nodes[$fromid]->getOutIds();
        $toNodes   = $this->nodes[$toid]->getInIds();

        $toIdInFromNodes = in_array($toid, $fromNodes);
        $fromIdInToNodes = in_array($fromid, $toNodes);

        if ($toIdInFromNodes === TRUE && $fromIdInToNodes === TRUE) {
            $isLinked = TRUE;
        } else if ($toIdInFromNodes === TRUE || $fromIdInToNodes === TRUE) {
            $msg = 'Fix Me. From Id: '.$fromid.' To Id: '.$toid;
            throw new Exception($msg);
        }

        return $isLinked;

    }//end isLinked()


}//end class

?>