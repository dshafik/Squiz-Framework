<?php
/**
 * Provides methods to retrieve information about channel data.
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
 * @subpackage Channels
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once dirname(__FILE__).'/Exceptions/ChannelException.inc';
require_once dirname(__FILE__).'/Exceptions/ChannelPlugException.inc';
require_once dirname(__FILE__).'/Exceptions/ChannelActionException.inc';
require_once 'Libs/Util/Util.inc';

// Include Debug for Dev.
require_once 'Libs/Debug/Debug.inc';
require_once 'Libs/Debug/DebugExecutionPath.inc';
require_once 'Systems/BaseSystem.inc';

/**
 * Provides methods to retrieve information about channel data.
 *
 */
class Channels
{

    /**
     * Contains data for all the channels.
     *
     * Array is indexed by the event names.
     *
     * @var   array
     */
    private static $_eventBasket = array();

    /**
     * Unique ID for the baked oven.
     *
     * By default, ChannelsBaker will bake in 'Oven' directory. If the name
     * is set, then name + 'Oven' directory will be created. e.g.) 'debug' will
     * create 'debugOven'.
     *
     * @var   boolean
     */
    public static $channelsMode = '';

    /**
     * List of included systems.
     *
     * @var   array
     */
    private static $_includedSystems = array();

    /**
     * If TRUE the system is currently being installed.
     *
     * @var   boolean
     */
    public static $installing = FALSE;

    /**
     * List of plugs to run in the channels.
     *
     * Introduced to be used during the testing.
     *
     * @var   array
     */
    private static $_runPlugList = array();

    /**
     * Flag saying run all the plugs.
     *
     * Introduced to be used during the testing.
     *
     * @var   boolean
     */
    public static $runAllPlugs = FALSE;


    /**
     * Channels constructor.
     *
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Will be called from MySource System only.
     *
     * @return string
     */
    public static function init()
    {
        self::includeSystem('MySource');
        $contents = MySource::main();
        return $contents;

    }//end init()


    /**
     * Will be called from the init() channel.
     *
     * This is the main channel action for MySource 4.
     *
     * @return void
     */
    public static function main()
    {

    }//end main()


    /**
     * Will be called from MySource System only.
     *
     * Any non fatal errors will be handled by this function.
     *
     * @param Exception $e Exception object thrown.
     *
     * @return void
     * @throws Exception If given eception is unknown.
     */
    public static function handleError(Exception $e)
    {
        $className = get_class($e);
        switch ($className) {
            case 'ChannelPlugException':
                // TODO: Need to make some error handling policies.
                error_log('ChannelPlugException thrown:'.$e->getMessage());
            break;

            default:
                // Don't know how to handle, throw again.
                throw $e;
            break;
        }

    }//end handleError()


    /**
     * Includes the given system's actions file.
     *
     * @param string $system System name.
     *
     * @return boolean
     * @throws ChannelException If the actions path doesn't exist.
     */
    public static function requireSystemActions($system)
    {
        if ($system === 'MySource') {
            return TRUE;
        }

        $systemPath  = self::getSystemsPath($system);
        $actionsPath = $systemPath.'/'.$system.'Actions.inc';

        if (file_exists($actionsPath) === FALSE) {
            $msg = 'Path "'.$actionsPath.'" does not exist.';
            throw new ChannelException($msg);
            return FALSE;
        }

        include_once $actionsPath;

        return TRUE;

    }//end requireSystemActions()


    /**
     * Returns the systems dir path.
     *
     * @param string $systemName System name.
     *
     * @return string
     */
    public static function getSystemsPath($systemName='')
    {
        $dir = '';
        if ($systemName !== '') {
            $listEl  = self::getSystemXml();
            $xpath   = new DOMXPATH($listEl);
            $systems = $xpath->query('/systems/system[@name="'.$systemName.'"]');
            foreach ($systems as $system) {
                $dir = rtrim($system->getAttribute('path'), '/');
                break;
            }//end foreach
        } else {
            $dir = dirname(dirname(__FILE__)).'/Systems';
        }

        return $dir;

    }//end getSystemsPath()


    /**
     * Returns a DomDocument containing the contents of the systems XML file.
     *
     * @return DomDocument
     */
    public static function getSystemXml()
    {
        $ovenDir    = self::getOvenPath();
        $systemFile = $ovenDir.'/systems.xml';
        if (file_exists($systemFile) === FALSE) {
            include_once 'ChannelsBaker.inc';
            ChannelsBaker::refreshSystemList();
        }

        $doc = new DomDocument();
        $doc->load($systemFile);

        return $doc;

    }//end getSystemXml()


    /**
     * Returns the oven dir path.
     *
     * If system name is provided then, that system's oven dir will be returned.
     *
     * @param string $systemName System name.
     *
     * @return string
     */
    public static function getOvenPath($systemName='')
    {
        $dir    = dirname(__FILE__);
        $result = $dir.'/';

        // Has anybody set alternative channels mode?
        if (self::$channelsMode !== '') {
            $result .= self::$channelsMode;
        }

        $result .= 'Oven';

        // System name specified. Append it!
        if ($systemName !== '') {
            $result .= '/'.$systemName;
        }

        return $result;

    }//end getOvenPath()


    /**
     * Checks if the given variable is in the current event basket.
     *
     * @param string $var Variable name.
     *
     * @return boolean
     */
    public static function eventVarExists($var)
    {
        $basket = self::getCurrentEventBasketData();
        if (isset($basket[$var]) === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end eventVarExists()


    /**
     * Adds the given var and its value to the current event basket.
     *
     * If var already exists in the basket then no changes will be made
     * to the basket and function will return false. To modify the var value
     * call modifyBasket().
     *
     * @param string  $var    Name of the event basket variable.
     * @param mixed   $value  Value of the variable.
     * @param boolean $update If true var value will be updated.
     *
     * @return boolean
     */
    public static function addToBasket($var, $value, $update=FALSE)
    {
        $data = self::getCurrentEventBasketData();
        if ((isset($data[$var]) === TRUE) && ($update === FALSE)) {
            // Data already exists in the basket. Use modifyBasket() instead.
            return FALSE;
        }

        self::$_eventBasket[(count(self::$_eventBasket) - 1)][$var] = $value;
        return TRUE;

    }//end addToBasket()


    /**
     * Modifies the given var value in the current channel.
     *
     * Returns false if variable is not in the basket.
     *
     * @param string $var      Name of the event basket variable.
     * @param mixed  $newValue Value of the variable.
     *
     * @return boolean
     */
    public static function modifyBasket($var, $newValue)
    {
        $data = self::getCurrentEventBasketData();
        if (isset($data[$var]) === FALSE) {
            return FALSE;
        }

        self::$_eventBasket[(count(self::$_eventBasket) - 1)][$var] = $newValue;
        return TRUE;

    }//end modifyBasket()


    /**
     * Removes a variable from the current event basket.
     *
     * Returns true on success, false if variable was not removed.
     *
     * @param string $var Variable name.
     *
     * @return boolean
     */
    public static function removeFromBasket($var)
    {
        $data = self::getCurrentEventBasketData();
        if (isset($data[$var]) === FALSE) {
            return FALSE;
        }

        unset(self::$_eventBasket[(count(self::$_eventBasket) - 1)][$var]);
        return TRUE;

    }//end removeFromBasket()


    /**
     * Returns the current event/channel's data.
     *
     * @return array
     */
    public static function getCurrentEventBasketData()
    {
        // Last element in event basket is the current channel/event data.
        if (empty(self::$_eventBasket) === TRUE) {
            $data = array();
        } else {
            $data = self::$_eventBasket[(count(self::$_eventBasket) - 1)];
        }

        return $data;

    }//end getCurrentEventBasketData()


    /**
     * Creates an empty basket for a new channel.
     *
     * Adds an empty array to the end of the event basket array.
     *
     * @return void
     */
    public static function createEventBasket()
    {
        $basket = array();
        array_push(self::$_eventBasket, $basket);

    }//end createEventBasket()


    /**
     * Removes the last basket from the event Basket array.
     *
     * @return void
     */
    public static function removeEventBasket()
    {
        array_pop(self::$_eventBasket);

    }//end removeEventBasket()


    /**
     * Returns TRUE if the system is installed.
     *
     * @param string $system System name.
     *
     * @return boolean
     */
    public static function systemExists($system)
    {
        include_once self::getOvenPath().'/systems.inc';
        if (isset(Systems::$systems[$system]) === TRUE) {
            $exist = TRUE;
        } else {
            $exist = FALSE;
        }

        return $exist;

    }//end systemExists()


    /**
     * Includes the given system's baked file.
     *
     * @param string $system System name.
     *
     * @return boolean
     * @throws ChannelException When the system path doesn't exist.
     */
    public static function includeSystem($system)
    {
        if (isset(self::$_includedSystems[$system]) === FALSE) {
            $systemPath = self::getOvenPath($system).'/'.$system.'.inc';
            if (file_exists($systemPath) === FALSE) {
                $msg = 'Path "'.$systemPath.'" does not exist.';
                throw new ChannelException($msg);
            }

            self::$_includedSystems[$system] = TRUE;
            include_once $systemPath;
        }

        return TRUE;

    }//end includeSystem()


    /**
     * Returns a list of systems available to channels.
     *
     * @param boolean $enabledOnly   If true then returns enabled systems only.
     * @param boolean $incInterfaces If true, interfaces will be included in
     *                               the results set.
     * @param boolean $namesOnly     Returns the system names only if set to
     *                               true, else it will return full file paths.
     *
     * @return array
     */
    public static function getSystemList(
        $enabledOnly=TRUE,
        $incInterfaces=FALSE,
        $namesOnly=TRUE
    ) {
        $systemList = array();

        $listEl  = self::getSystemXml();
        $systems = $listEl->getElementsByTagName('system');

        foreach ($systems as $system) {
            // Skip if system is not enabled and we are only getting enableds.
            if ($enabledOnly === TRUE) {
                if ($system->getAttribute('enabled') !== 'yes') {
                    continue;
                }
            }

            // Skip if this is interface and incInterface is set to false.
            if ($system->getAttribute('interface') === 'yes') {
                if ($incInterfaces === FALSE) {
                    continue;
                }
            }

            if ($namesOnly === TRUE) {
                $systemList[] = $system->getAttribute('name');
            } else {
                $systemList[$system->getAttribute('name')] = $system->getAttribute('path');
            }
        }//end foreach

        return $systemList;

    }//end getSystemList()


    /**
     * Returns a list of system dependencies.
     *
     * Each dependency element takes a form as:
     * array(
     *  'from' => SYSTEM_NAME,
     *  'to'   => SYSTEM_NAME,
     *  'type' => DEPENDENCY TYPE,
     * ).
     *
     * @param mixed $type The type of dependency to get. The value can be
     *                    any combination of 'concrete', 'optional',
     *                    'inheritance' and 'install'.
     *
     * @return array
     */
    public static function getAllDependencies($type=NULL)
    {
        $result    = array();
        $listEl    = self::getSystemXml();
        $systemsEl = $listEl->getElementsByTagName('systems')->item(0);
        if ($systemsEl === NULL) {
            return $result;
        }

        if (is_string($type) === TRUE) {
            $type = (array) $type;
        } else if ($type === NULL || is_array($type) === FALSE) {
            $type = array();
        }

        $systems = $systemsEl->getElementsByTagName('system');
        foreach ($systems as $system) {
            $result = array_merge(
                $result,
                self::_getSystemDependencies($system, $type)
            );
        }

        return $result;

    }//end getAllDependencies()


    /**
     * Returns a list of single system dependencies by name.
     *
     * @param string $systemName The name of the system.
     * @param mixed  $type       The type of dependency to get. The value can be
     *                           any combination of 'concrete', 'optional',
     *                           'inheritance' and 'install'.
     *
     * @return array
     */
    public static function getSystemDependencies($systemName, $type=NULL)
    {
        $result    = array();
        $listEl    = self::getSystemXml();
        $systemsEl = $listEl->getElementsByTagName('systems')->item(0);
        if ($systemsEl === NULL) {
            return $result;
        }

        if (is_string($type) === TRUE) {
            $type = (array) $type;
        } else if ($type === NULL || is_array($type) === FALSE) {
            $type = array();
        }

        $systems = $systemsEl->getElementsByTagName('system');

        foreach ($systems as $system) {
            if ($system->getAttribute('name') === $systemName) {
                $result = self::_getSystemDependencies($system, $type);
            }
        }

        return $result;

    }//end getSystemDependencies()


    /**
     * Returns a list of single system dependencies by name.
     *
     * @param DomNode $system The system DOM node from systems.xml.
     * @param array   $type   The type of dependency to get. The value can be
     *                        any combination of 'concrete', 'optional',
     *                        'inheritance' and 'install'.
     *
     * @return array
     */
    private static function _getSystemDependencies(
        DomNode $system,
        array $type=array()
    ) {
        $result         = array();
        $dependenciesEl = $system->getElementsByTagName('dependencies')->item(0);

        $typeFilter = FALSE;
        if (empty($type) === FALSE) {
            $typeFilter = TRUE;
        }

        if ($dependenciesEl !== NULL) {
            $systemName = $system->getAttribute('name');

            $dependencies = $dependenciesEl->getElementsByTagName('dependency');
            foreach ($dependencies as $dependency) {
                $typ = $dependency->getAttribute('type');
                if ($typeFilter === TRUE && in_array($typ, $type) === FALSE) {
                    continue;
                }

                $result[] = array(
                             'from' => $systemName,
                             'to'   => $dependency->getAttribute('system'),
                             'type' => $typ,
                            );
            }
        }

        return $result;

    }//end _getSystemDependencies()


    /**
     * Returns the list of baked system actions.
     *
     * @param string $systemName Name of the system.
     *
     * @return array
     */
    public static function getAvailableSystemActions($systemName)
    {
        $actions = array();
        if (self::systemExists($systemName) === FALSE) {
            return $actions;
        } else {
            self::includeSystem($systemName);
        }

        $refC    = new ReflectionClass($systemName);
        $methods = $refC->getMethods();
        foreach ($methods as $refM) {
            $actionName                   = $refM->getName();
            $actions[$actionName]         = array();
            $actions[$actionName]['args'] = array();

            $parameters = $refM->getParameters();
            foreach ($parameters as $refP) {
                $actions[$actionName]['args'][] = $refP->getName();
            }
        }

        return $actions;

    }//end getAvailableSystemActions()


    /**
     * Returns the list of Widget Type systems with its inheritance info.
     *
     * @return array
     */
    public static function getWidgetSystemsMapArray()
    {
        $widgetSystems = array();
        $systemList    = self::getSystemList(TRUE, TRUE, FALSE);

        foreach ($systemList as $systemName => $systemPath) {
            if ($systemName === 'Widget'
                || substr($systemName, -6) !== 'Widget'
            ) {
                continue;
            }

            $widgetSystems[$systemName] = $systemPath;
        }

        // Every widget's top parent must be AbstractWidgetWidgetType.
        $result = array(
                   'AbstractWidgetWidgetType' => array(
                                                  'children' => array(),
                                                  'type'     => 'abstract',
                                                 ),
                  );

        foreach ($widgetSystems as $widgetName => $widgetPath) {
            if ($widgetName === 'AbstractWidgetWidgetType') {
                continue;
            }

            $widgetTypeClassName = $widgetName.'Type';
            include_once $widgetPath.'/'.$widgetTypeClassName.'.inc';
            $ref        = new ReflectionClass($widgetTypeClassName);
            $parentRef  = $ref->getParentClass();
            $widgetTree = array($widgetTypeClassName);

            while ($parentRef !== FALSE) {
                $widgetTree[] = $parentRef->getName();
                $parentRef    = $parentRef->getParentClass();
            }

            $widgetTree = array_reverse($widgetTree);

            $parentType = 'AbstractWidgetWidgetType';
            foreach ($widgetTree as $widgetType) {
                if (isset($result[$parentType]) === FALSE) {
                    $result[$parentType] = array(
                                            'children' => array(),
                                            'type'     => '',
                                           );
                } else {
                    if (isset($result[$parentType]['children'][$widgetType]) === FALSE
                        && $widgetType !== 'AbstractWidgetWidgetType'
                    ) {
                        $result[$parentType]['children'][$widgetType] = TRUE;
                    }
                }

                $parentType = $widgetType;
            }

            if (isset($result[$parentType]) === FALSE) {
                $result[$parentType] = array(
                                        'children' => array(),
                                        'type'     => '',
                                       );
            }
        }//end foreach

        return $result;

    }//end getWidgetSystemsMapArray()


    /**
     * Returns TRUE if the given plug id exists in $_runPlugList.
     *
     * Introduced to be used in the testing.
     *
     * @param string $pid Unique id of the plug to run.
     *
     * @return boolean
     */
    public static function runPlug($pid)
    {
        // By default if the passed plugid does not exist,
        // it does not run.
        if (self::$runAllPlugs === TRUE) {
            return TRUE;
        }

        if (isset(self::$_runPlugList[$pid]) === FALSE) {
            // Check if all plugs need to run for this action.
            // E.g. "*::Keyword::replaceKeywordScopes".
            $npid = preg_replace('/^\w+::\w+/', '', $pid);
            if (isset(self::$_runPlugList['*'.$npid]) === FALSE) {
                return FALSE;
            }
        }

        return TRUE;

    }//end runPlug()


    /**
     * Resets $_runPlugList array.
     *
     * Introduced to be used in the testing.
     *
     * @return void
     */
    public static function resetRunPlug()
    {
        self::$_runPlugList = array();

    }//end resetRunPlug()


    /**
     * Adds a plug id to $_runPlugList array.
     *
     * Introduced to be used in the testing.
     *
     * @param string $plugid Unique id of the plug to run.
     *
     * @return void
     */
    public static function addPlugToRun($plugid)
    {
        self::$_runPlugList[$plugid] = TRUE;

    }//end addPlugToRun()


    /**
     * Remove a plug id to $_runPlugList array.
     *
     * Introduced to be used in the testing.
     *
     * @param string $plugid Unique id of the plug to run.
     *
     * @return void
     */
    public static function removePlugToRun($plugid)
    {
        if (isset(self::$_runPlugList[$plugid]) === TRUE) {
            unset(self::$_runPlugList[$plugid]);
        }

    }//end removePlugToRun()


}//end class

?>
