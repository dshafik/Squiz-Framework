<?php
/**
 * Basic structure for all session handling systems.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Session
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

/**
 * Basic structure for all session handling systems.
 *
 * @since 4.0.0
 */
interface SessionActions
{


    /**
     * Starts a new session.
     *
     * @since  4.0.0
     * @return string
     */
    public static function startSession();


    /**
     * Destroys the current session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function destroySession();


    /**
     * Custom handler for the opening of a new session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionOpenHandler();


    /**
     * Custom handler for the closing of an existing session.
     *
     * @since  4.0.0
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
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionWriteHandler();


    /**
     * Custom handler for the destruction of an existing session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionDestroyHandler();


    /**
     * Custom handler for the garbage collection of an existing session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionCleanHandler();


    /**
     * Returns the id of the current session.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSessionid();


    /**
     * Regenerates and returns the current session id.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function regenerateSessionid();


    /**
     * Works out if we are using custom session handling functions.
     *
     * If this returns TRUE, the default PHP session handling operations
     * will be overridden with the session handler channels in this system.
     *
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
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
     * @since  4.0.0
     * @return mixed
     */
    public static function getSessionData($systemName, $dataKey);


}//end interface

?>