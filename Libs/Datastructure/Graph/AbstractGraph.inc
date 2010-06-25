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

/**
 * Abstract class for generic graph in Datastructure.
 *
 * @since 4.0.0
 */
abstract class AbstractGraph
{

    /**
     * List of nodes in the graph.
     *
     * @var   array
     * @since 4.0.0
     */
    protected $nodes = array();


    /**
     * Return the list of nodes in the graph.
     *
     * @since  4.0.0
     * @return array
     */
    abstract public function getNodes();


}//end class

?>