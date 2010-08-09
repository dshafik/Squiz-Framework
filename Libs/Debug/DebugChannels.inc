<?php
/**
 * DebugChannels Library.
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

require_once 'Libs/Debug/Debug.inc';
require_once 'Libs/Debug/DotCreator.inc';

/**
 * Library for development and debugging.
 *
 */
class DebugChannels
{

    /**
     * If TRUE then debugging is ON.
     *
     * @var   boolean
     */
    private static $_debugging = TRUE;

    /**
     * List of classes that are ignored by debugger.
     *
     * @var   array
     */
    private static $_ignoredClasses = array(
                                       'Channels',
                                       'DAL',
                                       'Init',
                                       'BaseSystem',
                                       'Query',
                                       'DebugChannels',
                                      );

    /**
     * This var will change to FALSE after first valid tick.
     *
     * @var   boolean
     */
    private static $_firstTick = TRUE;

    /**
     * Tracks the backtrace level.
     *
     * @var   integer
     */
    private static $_prevCount = 0;

    /**
     * Tracks the previous method call.
     *
     * @var   string
     */
    private static $_prevMethodCalled = NULL;

    /**
     * DotCreator object.
     *
     * @var   object
     */
    public static $dotCreator = NULL;

    /**
     * List of called methods.
     *
     * @var   string
     */
    private static $_called = array();

    /**
     * Function call stack.
     *
     * @var   array
     */
     private static $_stack = array();


    /**
     * Start the debug ignore block.
     *
     * This may be required for things like array_walk_recursive() which causes
     * segfault when debug_backtrace() is called.
     *
     * @return void
     */
    public static function ignoreStart()
    {
        self::$_debugging = FALSE;

    }//end ignoreStart()


    /**
     * End the debug ignore block.
     *
     * @return void
     */
    public static function ignoreEnd()
    {
        self::$_debugging = TRUE;

    }//end ignoreEnd()


    /**
     * The tick function.
     *
     * @return void
     */
    public static function tick()
    {
        if (self::$_debugging === FALSE) {
            return;
        }

        $time = microtime(TRUE);

        $bt = debug_backtrace();

        $info = self::_getCallInfo($bt);
        if (empty($info) === TRUE) {
            return;
        }

        // Get caller and called method names.
        $currentMethod = $info['method'];
        $caller        = $info['caller'];

        if (self::$_firstTick === TRUE) {
            self::$_stack     = array();
            self::$dotCreator = new DotCreator();
            self::$_firstTick = FALSE;
        }

        if ($currentMethod === $caller) {
            return;
        }

        $btCount = (count($bt) - $info['mIndex']);

        if ($btCount > self::$_prevCount) {
            // New function call.
            self::_startCall($currentMethod, $caller, $time, $bt);
        } else if ($btCount < self::$_prevCount) {
            // Function finished.
            self::_endCall($time);
        } else if (self::$_prevMethodCalled !== $currentMethod) {
            self::_endCall($time);
            // New function call so the $_prevMethodCalled has anded and a new
            // ($currentMethod) method is called.
            self::_startCall($currentMethod, $caller, $time, $bt);
        }

        self::$_prevMethodCalled = $currentMethod;
        self::$_prevCount        = $btCount;

    }//end tick()


    /**
     * Adds a new function call to the list.
     *
     * @param string  $currentMethod The method that was called.
     * @param string  $caller        The method that called.
     * @param float   $startTime     Start time of the function call.
     * @param array   $bt            The backtrace.
     * @param integer $offset        The backtrace offset (used for recursion).
     *
     * @return void
     */
    private static function _startCall($currentMethod, $caller, $startTime, array $bt, $offset=1)
    {
        if (isset(self::$_called[$caller]) === FALSE) {
            if ($caller !== 'index.php') {
                // This usually happens due to inheritance or a function calls another right at the beginning.
                $callerInfo = self::_getCallInfo($bt, ($offset + 1));
                if (isset($callerInfo['method']) === FALSE) {
                    return;
                }

                self::_startCall($caller, $callerInfo['method'], $startTime, $bt, ($offset + 1));
            }
        }

        if ($caller === $currentMethod) {
            return;
        }

        self::$_called[$currentMethod] = TRUE;
        self::$dotCreator->addNode($currentMethod, $currentMethod);
        self::$_stack[] = array(
                           'caller' => $caller,
                           'method' => $currentMethod,
                           'start'  => $startTime,
                          );

    }//end _startCall()


    /**
     * Ends a function call and adds a connection with total time.
     *
     * @param float $endTime The time.
     *
     * @return void
     */
    private static function _endCall($endTime)
    {
        if (empty(self::$_stack) === TRUE) {
            return;
        }

        $call      = array_pop(self::$_stack);
        $totalTime = ($endTime - $call['start']);

        $colour = NULL;
        if (strpos($call['method'], 'Util') === 0
            || strpos($call['method'], 'Security') === 0
            || strpos($call['method'], 'Web') === 0
        ) {
            $colour = 'darkviolet';
        }

        self::$dotCreator->addConnection(
            $call['caller'],
            $call['method'],
            number_format($totalTime, 5),
            NULL,
            $colour
        );

    }//end _endCall()


    /**
     * Returns TRUE if the specified method is from an ignored class.
     *
     * @param string $method The method string to check.
     *
     * @return boolean
     */
    public static function isIgnoredClass($method)
    {
        if ($method === NULL) {
            return TRUE;
        }

        if (strpos($method, 'Queries') !== FALSE
            || strpos($method, 'Actions') !== FALSE
        ) {
            return TRUE;
        }

        foreach (self::$_ignoredClasses as $class) {
            if (strpos($method, $class) === 0) {
                return TRUE;
            }
        }

        return FALSE;

    }//end isIgnoredClass()


    /**
     * Returns the method call information from given backtrace.
     *
     * @param array   $bt     The backtrace.
     * @param integer $offset The start index.
     *
     * @return array
     */
    private static function _getCallInfo(array $bt, $offset=1)
    {
        $info = array();
        if (empty($bt) === TRUE) {
            return $info;
        }

        // Now find the first index that is a valid function call.
        $count  = count($bt);
        $mFound = FALSE;
        for ($i = $offset; $i < $count; $i++) {
            if (isset($bt[$i]['class']) === FALSE
                || isset($bt[$i]['function']) === FALSE
            ) {
                continue;
            }

            $method = $bt[$i]['class'].'::'.$bt[$i]['function'];
            if (self::isIgnoredClass($method) === FALSE) {
                if ($mFound !== TRUE) {
                    $mFound         = TRUE;
                    $info['method'] = $method;
                    $info['mIndex'] = $i;
                } else {
                    // This is the caller.
                    $info['caller'] = $method;
                    break;
                }
            }
        }

        if ($mFound === TRUE && isset($info['caller']) === FALSE) {
            $info['caller'] = 'index.php';
        }

        return $info;

    }//end _getCallInfo()


}//end class

?>
