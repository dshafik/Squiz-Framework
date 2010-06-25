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
 * Abstract class for generic node in Datastructure.
 *
 * @since 4.0.0
 */
abstract class AbstractNode
{

    /**
     * Unique id for the node.
     *
     * @var   string
     * @since 4.0.0
     */
    protected $id = '';

    /**
     * Data storage for each node.
     *
     * @var   array
     * @since 4.0.0
     */
    protected $data = array();


    /**
     * Return the reference to the stored data with the key.
     *
     * @param string $key The key for the value.
     *
     * @since  4.0.0
     * @return mixed
     */
    abstract public function getData($key);


    /**
     * Set the given value with the key.
     *
     * @param string $key The key for the data.
     * @param mixed  $val The value associated with the key.
     *
     * @since  4.0.0
     * @return void
     */
    abstract public function setData($key, $val);


    /**
     * Unset the key and value from data array.
     *
     * @param string $key The key for the data.
     *
     * @since  4.0.0
     * @return mixed
     */
    abstract public function unsetData($key);


}//end class

?>