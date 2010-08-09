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

/**
 * Abstract class for generic node in Datastructure.
 *
 */
abstract class AbstractNode
{

    /**
     * Unique id for the node.
     *
     * @var   string
     */
    protected $id = '';

    /**
     * Data storage for each node.
     *
     * @var   array
     */
    protected $data = array();


    /**
     * Return the reference to the stored data with the key.
     *
     * @param string $key The key for the value.
     *
     * @return mixed
     */
    abstract public function getData($key);


    /**
     * Set the given value with the key.
     *
     * @param string $key The key for the data.
     * @param mixed  $val The value associated with the key.
     *
     * @return void
     */
    abstract public function setData($key, $val);


    /**
     * Unset the key and value from data array.
     *
     * @param string $key The key for the data.
     *
     * @return mixed
     */
    abstract public function unsetData($key);


}//end class

?>