<?php
/**
 * Actions for the FileSession System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage FileSession
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/Session/SessionActions.inc';

/**
 * FileSessionActions.
 *
 * @since 4.0.0
 */
class FileSessionActions implements SessionActions
{

    /**
     * If TRUE, the sesssion has already been started.
     *
     * @var   boolean
     * @since 4.0.0
     */
    private static $_sessionStarted = FALSE;


    /**
     * Starts a new session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function startSession()
    {
        Session::setSessionConfig('save_path', BaseSystem::getDataDir('Session'));

        if (self::$_sessionStarted === TRUE) {
            return TRUE;
        }

        // When it runs from command line, do not start the session.
        if (php_sapi_name() === 'cli') {
            return FALSE;
        }

        $sessid = Session::getSessionid();
        if (empty($sessid) === FALSE) {
            return FALSE;
        }

        if (Session::usesCustomHandler() === TRUE) {
            $handlers = array(
                         'open'    => array(
                                       'Session',
                                       'sessionOpenHandler',
                                      ),
                         'close'   => array(
                                       'Session',
                                       'sessionCloseHandler',
                                      ),
                         'read'    => array(
                                       'Session',
                                       'sessionReadHandler',
                                      ),
                         'write'   => array(
                                       'Session',
                                       'sessionWriteHandler',
                                      ),
                         'destroy' => array(
                                       'Session',
                                       'sessionDestroyHandler',
                                      ),
                         'gc'      => array(
                                       'Session',
                                       'sessionCleanHandler',
                                      ),
                        );

            // Allow for override of default PHP session operations.
            call_user_func_array('session_set_save_handler', $handlers);
        }//end if

        self::$_sessionStarted = session_start();
        return self::$_sessionStarted;

    }//end startSession()


    /**
     * Destroys the current session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function destroySession()
    {
        $result = session_destroy();

        if ($result === TRUE) {
            unset($_SESSION);
        }

        return $result;

    }//end destroySession()


    /**
     * Custom handler for the opening of a new session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionOpenHandler()
    {
        return TRUE;

    }//end sessionOpenHandler()


    /**
     * Custom handler for the closing of an existing session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionCloseHandler()
    {
        return TRUE;

    }//end sessionCloseHandler()


    /**
     * Custom handler for the reading of an existing session.
     *
     * @since  4.0.0
     * @return string
     */
    public static function sessionReadHandler()
    {
        return '';

    }//end sessionReadHandler()


    /**
     * Custom handler for the writing of an existing session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionWriteHandler()
    {
        return TRUE;

    }//end sessionWriteHandler()


    /**
     * Custom handler for the destruction of an existing session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionDestroyHandler()
    {
        return TRUE;

    }//end sessionDestroyHandler()


    /**
     * Custom handler for the garbage collection of an existing session.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function sessionCleanHandler()
    {
        return TRUE;

    }//end sessionCleanHandler()


    /**
     * Returns the id of the current session.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getSessionid()
    {
        $sessionid = session_id();
        return $sessionid;

    }//end getSessionid()


    /**
     * Regenerates and returns the current session id.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function regenerateSessionid()
    {
        $sessionid = session_regenerate_id();
        return $sessionid;

    }//end regenerateSessionid()


    /**
     * Works out if we are using custom session handling functions.
     *
     * If this returns TRUE, the default PHP session handling operations
     * will be overridden with the session handler channels in this system.
     *
     * @since  4.0.0
     * @return boolean
     */
    public static function usesCustomHandler()
    {
        return FALSE;

    }//end usesCustomHandler()


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
    public static function setSessionConfig($sessionConfig, $value)
    {
        // The ini_set() function returns FALSE when something
        // went bad, otherwise we'll get the old value.
        $retval = (bool) ini_set('session.'.$sessionConfig, $value);
        return $retval;

    }//end setSessionConfig()


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
    public static function setSessionData($systemName, $dataKey, $data)
    {
        Channels::includeSystem('Session');
        Session::startSession();

        // You should really start your session properly first.
        $sessid = Session::getSessionid();
        if (empty($sessid) === TRUE && php_sapi_name() !== 'cli') {
            return FALSE;
        }

        $_SESSION[$systemName][$dataKey] = $data;
        return TRUE;

    }//end setSessionData()


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
    public static function getSessionData($systemName, $dataKey)
    {
        if (self::$_sessionStarted === FALSE && isset($_SESSION) === FALSE && (isset($_COOKIE) === FALSE || isset($_COOKIE['PHPSESSID']) === FALSE)) {
            return NULL;
        }

        Channels::includeSystem('Session');
        Session::startSession();

        if (isset($_SESSION[$systemName][$dataKey]) === TRUE) {
            return $_SESSION[$systemName][$dataKey];
        }

        return NULL;

    }//end getSessionData()


    /**
     * Remove session data of the given system.
     *
     * @param string $systemName The name of the system which
     *                           set the value.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function removeSystemSession($systemName)
    {
        Channels::includeSystem('Session');
        Session::startSession();

        if (isset($_SESSION[$systemName]) === TRUE) {
            unset($_SESSION[$systemName]);
            return TRUE;
        }

        return FALSE;

    }//end removeSystemSession()


    /**
     * Remove session data of the given key in the given system.
     *
     * @param string $systemName The name of the system which
     *                           set the value.
     * @param string $dataKey    Identifier/reference of value to get.
     *
     * @since  4.0.0
     * @return mixed
     */
    public static function removeSystemDataSession($systemName, $dataKey)
    {
        Channels::includeSystem('Session');
        Session::startSession();

        if (isset($_SESSION[$systemName]) === TRUE) {
            if (isset($_SESSION[$systemName][$dataKey]) === TRUE) {
                unset($_SESSION[$systemName][$dataKey]);
                return TRUE;
            }
        }

        return FALSE;

    }//end removeSystemDataSession()


}//end class

?>