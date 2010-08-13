<?php
/**
 * Basic structure for all session handling systems.
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
 * @subpackage Session
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

/**
 * Basic structure for all session handling systems.
 */
interface SessionActions
{


    /**
     * Starts a new session.
     *
     * @return string
     */
    public static function startSession();


    /**
     * Destroys the current session.
     *
     * @return boolean
     */
    public static function destroySession();


    /**
     * Custom handler for the opening of a new session.
     *
     * @return boolean
     */
    public static function sessionOpenHandler();


    /**
     * Custom handler for the closing of an existing session.
     *
     * @return boolean
     */
    public static function sessionCloseHandler();


    /**
     * Custom handler for the reading of an existing session.
     *
     * @since  4.0.0
     * @return string
     */
    public static function sessionReadHandler();


    /**
     * Custom handler for the writing of an existing session.
     *
     * @return boolean
     */
    public static function sessionWriteHandler();


    /**
     * Custom handler for the destruction of an existing session.
     *
     * @return boolean
     */
    public static function sessionDestroyHandler();


    /**
     * Custom handler for the garbage collection of an existing session.
     *
     * @return boolean
     */
    public static function sessionCleanHandler();


    /**
     * Returns the id of the current session.
     *
     * @return string
     */
    public static function getSessionid();


    /**
     * Regenerates and returns the current session id.
     *
     * @return boolean
     */
    public static function regenerateSessionid();


    /**
     * Works out if we are using custom session handling functions.
     *
     * If this returns TRUE, the default PHP session handling operations
     * will be overridden with the session handler channels in this system.
     *
     * @return boolean
     */
    public static function usesCustomHandler();


    /**
     * Set the value of PHP session configuration option.
     *
     * @param string $sessionConfig Name of configuration setting
     *                              omitting the preceding 'session'
     *                              identifier as in php.ini.
     * @param string $value         Value to set.
     *
     * @return boolean
     */
    public static function setSessionConfig($sessionConfig, $value);


    /**
     * Standard way of storing a value in the session.
     *
     * @param string $systemName The name of the system that is
     *                           setting the session value.
     * @param string $dataKey    A value by which this data can
     *                           be identified/referenced.
     * @param mixed  $data       The data to set.
     *
     * @return boolean
     */
    public static function setSessionData($systemName, $dataKey, $data);


    /**
     * Standard way of retreiving a value stored in the session.
     *
     * Returns NULL when the value has not been set in the session.
     *
     * @param string $systemName The name of the system which
     *                           set the value.
     * @param string $dataKey    Identifier/reference of value to get.
     *
     * @return mixed
     */
    public static function getSessionData($systemName, $dataKey);


}//end interface

?>