<?php
/**
 * Datastructure Lib.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Libs
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Libs/Datastructure/AbstractNode.inc';

/**
 * SystemNode class.
 *
 * @since 4.0.0
 */
class SystemNode extends AbstractNode
{

    /**
     * The reference to the graph that this node is conntected to.
     *
     * @var   object
     * @since 4.0.0
     */
    private $_graph = NULL;

    /**
     * The list of nodes which have outgoing link to this node.
     *
     * @var   array
     * @since 4.0.0
     */
    private $_ins = array();

    /**
     * The list of nodes which this node has outgoing link.
     *
     * @var   array
     * @since 4.0.0
     */
    private $_outs = array();


    /**
     * Constructor.
     *
     * @param string               $id     The id for the node.
     * @param DirectedAcyclicGraph &$graph The graph which the node is
     *                                     connected to.
     *
     * @since  4.0.0
     * @throws Exception No id specified.
     */
    public function __construct($id='', DirectedAcyclicGraph &$graph=NULL)
    {
        if (empty($id) === TRUE) {
            throw new Exception('Can not create SystemNode without id.');
        }

        $this->id = $id;
        if ($graph !== NULL) {
            $this->setGraph($graph);
        }

    }//end __construct()


    /**
     * Return the reference to the stored data with the key.
     *
     * @param string $key The key for the value.
     *
     * @since  4.0.0
     * @return mixed
     * @throws Exception Unknown key specified.
     */
    public function getData($key)
    {
        if (isset($this->data[$key]) === TRUE) {
            return $this->data[$key];
        } else {
            $msg = 'Unknown key is requested to get data from the node.';
            throw new Exception($msg);
        }

    }//end getData()


    /**
     * Set the given value with the key.
     *
     * @param string $key The key for the data.
     * @param mixed  $val The value associated with the key.
     *
     * @since  4.0.0
     * @return void
     */
    public function setData($key, $val)
    {
        $this->data[$key] = $val;

    }//end setData()


    /**
     * Unset the key and value from data array.
     *
     * @param string $key The key for the data.
     *
     * @since  4.0.0
     * @return void
     */
    public function unsetData($key)
    {
        unset($this->data[$key]);

    }//end unsetData()


    /**
     * Return the id of the node.
     *
     * @since  4.0.0
     * @return mixed
     */
    public function getId()
    {
        return $this->id;

    }//end getId()


    /**
     * Connect this node to the given node.
     *
     * @param SystemNode &$node The node which this node is connecting to.
     *
     * @since  4.0.0
     * @return void
     * @throws Exception Error occurred during the node connection.
     */
    public function connectTo(SystemNode &$node)
    {
        $destNodeId = $node->getId();
        if (array_key_exists($destNodeId, $this->_outs) === TRUE) {
            $msg  = 'You can not connect '.$this->id;
            $msg .= ' to '.$destNodeId.' node more than once.';
            throw new Exception($msg);
        }

        if ($this->isGraphSet() === FALSE) {
            $msg  = 'Can not connect to '.$destNodeId;
            $msg .= ' before this node is connected to the graph.';
            throw new Exception($msg);
        }

        if ($node->isGraphSet() === FALSE) {
            $msg  = 'Can not connect to '.$destNodeId;
            $msg .= ' before '.$destNodeId.' node is connected to the graph.';
            throw new Exception($msg);
        }

        $this->_outs[$destNodeId] =& $node;
        $node->setInNode($this);

    }//end connectTo()


    /**
     * Set the incoming node to this node.
     *
     * @param SystemNode &$node The node which is connected to this node.
     *
     * @since  4.0.0
     * @return void
     * @throws Exception Error occurred during the node connection.
     */
    public function setInNode(SystemNode &$node)
    {
        $inNodeId = $node->getId();
        if (array_key_exists($inNodeId, $this->_ins) === TRUE) {
            $msg  = 'You can not connect '.$inNodeId;
            $msg .= ' to '.$this->id.'node more than once.';
            throw new Exception($msg);
        }

        $this->_ins[$inNodeId] =& $node;

    }//end setInNode()


    /**
     * Return the graph this node is connected to.
     *
     * @since  4.0.0
     * @return object
     */
    public function &getGraph()
    {
        return $this->_graph;

    }//end getGraph()


    /**
     * Connect the node to the given graph.
     *
     * @param DirectedAcyclicGraph &$graph The graph this noded is connected to.
     *
     * @since  4.0.0
     * @return void
     */
    public function setGraph(DirectedAcyclicGraph &$graph)
    {
        $this->_graph =& $graph;

    }//end setGraph()


    /**
     * Returns TRUE if the node is connected to the graph.
     *
     * @since  4.0.0
     * @return boolean
     */
    public function isGraphSet()
    {
        if ($this->_graph === NULL) {
            return FALSE;
        }

        return TRUE;

    }//end isGraphSet()


    /**
     * Returns the list of nodeids connected to this node.
     *
     * @since  4.0.0
     * @return array
     */
    public function getInIds()
    {
        $ids = array_keys($this->_ins);
        return $ids;

    }//end getInIds()


    /**
     * Returns the list of nodeids connected from this node.
     *
     * @since  4.0.0
     * @return array
     */
    public function getOutIds()
    {
        $ids = array_keys($this->_outs);
        return $ids;

    }//end getOutIds()


    /**
     * Returns the number of incoming links.
     *
     * @since  4.0.0
     * @return integer
     */
    public function inDegree()
    {
        $count = count($this->_ins);
        return $count;

    }//end inDegree()


    /**
     * Returns the number of outgoing links.
     *
     * @since  4.0.0
     * @return integer
     */
    public function outDegree()
    {
        $count = count($this->_outs);
        return $count;

    }//end outDegree()


    /**
     * Returns the number of outgoing links.
     *
     * Note: acyclic test helper function.
     *
     * @since  4.0.0
     * @return void
     */
    public function acyclicTestSetData()
    {
        // Note: The nodeid becomes a key of the array here.
        $this->setData('in', array_flip($this->getInIds()));
        $this->setData('out', array_flip($this->getOutIds()));

    }//end acyclicTestSetData()


    /**
     * Returns the number of outgoing links.
     *
     * Note: acyclic test helper function.
     *
     * @since  4.0.0
     * @return void
     */
    public function acyclicTestCleanup()
    {
        $this->unsetData('in');
        $this->unsetData('out');

    }//end acyclicTestCleanup()


    /**
     * Returns the list of incoming nodeids.
     *
     * Note: acyclic test helper function.
     *
     * @since  4.0.0
     * @return integer
     */
    public function acyclicTestGetInData()
    {
        $data = $this->getData('in');
        return $data;

    }//end acyclicTestGetInData()


    /**
     * Returns the list of outgoing nodeids.
     *
     * Note: acyclic test helper function.
     *
     * @since  4.0.0
     * @return integer
     */
    public function acyclicTestGetOutData()
    {
        $data = $this->getData('out');
        return $data;

    }//end acyclicTestGetOutData()


    /**
     * Returns TRUE if acyclic test shows it's a sink node.
     *
     * Note: acyclic test helper function.
     *
     * @since  4.0.0
     * @return integer
     */
    public function acyclicTestIsSinkNode()
    {
        if (count($this->data['out']) > 0) {
            return FALSE;
        }

        return TRUE;

    }//end acyclicTestIsSinkNode()


    /**
     * Returns TRUE if acyclic test shows it's a source node.
     *
     * Note: acyclic test helper function.
     *
     * @since  4.0.0
     * @return integer
     */
    public function acyclicTestIsSourceNode()
    {
        if (count($this->data['in']) > 0) {
            return FALSE;
        }

        return TRUE;

    }//end acyclicTestIsSourceNode()


    /**
     * Returns the number of incoming links.
     *
     * Note: acyclic test helper function.
     *
     * @param string $nodeid The id of the node to unlink.
     *
     * @since  4.0.0
     * @return void
     */
    public function acyclicTestDelOutNode($nodeid)
    {
        unset($this->data['out'][$nodeid]);

    }//end acyclicTestDelOutNode()


    /**
     * Returns the number of outgoing links.
     *
     * Note: acyclic test helper function.
     *
     * @param string $nodeid The id of the node to unlink.
     *
     * @since  4.0.0
     * @return void
     */
    public function acyclicTestDelInNode($nodeid)
    {
        unset($this->data['in'][$nodeid]);

    }//end acyclicTestDelInNode()


}//end class

?>