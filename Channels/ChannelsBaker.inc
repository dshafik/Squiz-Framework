<?php
/**
 * Controls all files involved in the baking of channels.
 *
 * The files handled include both the XML channels definitions files, as well
 * as the PHP classes that are baked out to represent these channel definitions.
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

require_once dirname(__FILE__).'/Channels.inc';
require_once dirname(__FILE__).'/Exceptions/ChannelException.inc';
require_once dirname(__FILE__).'/Exceptions/ChannelPlugException.inc';
require_once dirname(__FILE__).'/Exceptions/ChannelActionException.inc';
require_once dirname(__FILE__).'/Debug/ChannelsDebugger.inc';

/**
 * Controls all files involved in the baking of channels.
 *
 * The files handled include both the XML channels definitions files, as well
 * as the PHP classes that are baked out to represent these channel definitions.
 *
 */
class ChannelsBaker
{

    /**
     * The channels that will be generated for the main MySource system.
     *
     * @var   array
     */
    private static $_mySourceChannels = array(
                                         'init'        => array(
                                                           'inheritance'  => 'concrete',
                                                           'system'       => 'MySource',
                                                           'name'         => 'init',
                                                           'action'       => 'Channels::init',
                                                           'params'       => array(),
                                                           'fatal'        => FALSE,
                                                           'type'         => 'method',
                                                           'allowedPlugs' => array(
                                                                              'before' => array(
                                                                                           'blocker',
                                                                                           'gatherer',
                                                                                           'initial_gatherer',
                                                                                           'modifier',
                                                                                           'runner',
                                                                                           'switcher',
                                                                                          ),
                                                                              'after'  => array(),
                                                                             ),
                                                          ),
                                         'main'        => array(
                                                           'inheritance'  => 'concrete',
                                                           'system'       => 'MySource',
                                                           'name'         => 'main',
                                                           'action'       => 'Channels::main',
                                                           'params'       => array(),
                                                           'fatal'        => FALSE,
                                                           'type'         => 'method',
                                                           'allowedPlugs' => array(
                                                                              'before' => array(
                                                                                           'blocker',
                                                                                           'gatherer',
                                                                                           'initial_gatherer',
                                                                                           'modifier',
                                                                                           'runner',
                                                                                           'switcher',
                                                                                          ),
                                                                              'after'  => array(
                                                                                           'refiner',
                                                                                           'runner',
                                                                                          ),
                                                                             ),
                                                          ),
                                         'handleError' => array(
                                                           'inheritance'  => 'concrete',
                                                           'system'       => 'MySource',
                                                           'name'         => 'handleError',
                                                           'action'       => 'Channels::handleError',
                                                           'params'       => array(
                                                                              array('name' => 'message'),
                                                                              array('name' => 'line'),
                                                                              array('name' => 'file'),
                                                                             ),
                                                           'fatal'        => FALSE,
                                                           'type'         => 'method',
                                                           'allowedPlugs' => array(
                                                                              'before' => array(
                                                                                           'blocker',
                                                                                           'gatherer',
                                                                                           'modifier',
                                                                                           'runner',
                                                                                           'switcher',
                                                                                          ),
                                                                              'after'  => array(
                                                                                           'refiner',
                                                                                           'runner',
                                                                                          ),
                                                                             ),
                                                          ),
                                        );

    /**
     * Array of type locations.
     *
     * @var   array
     */
     private static $_typeLocs = array(
                                  'before' => array(
                                               'runner',
                                               'blocker',
                                               'gatherer',
                                               'modifier',
                                               'switcher',
                                               'initial_gatherer',
                                              ),
                                  'after'  => array(
                                               'runner',
                                               'refiner',
                                              ),
                                 );

     /**
      * Location types that are valid in the system.
      *
      * This has been included, as there may be new definition types available
      * in the future (such as SQL bindings) that will require a new location
      * type, and this will facilitate adding those locations.
      *
      * @var   array
      */
     private static $_locTypes = array(
                                  'before',
                                  'after',
                                 );

    /**
     * Stores the current system being painted to be used by the debug output.
     *
     * @var   array
     */
    private static $_currentSystem = '';

    /**
     * List of the system to be installed.
     *
     * @var   array
     */
    private static $_systemsInfo = array();

    /**
     * List of the system names to be installed.
     *
     * @var   array
     */
    private static $_systemNames = array();

    /**
     * Turns on channel execution path report mode.
     *
     * @var   array
     */
    public static $addReportCode = FALSE;

    /**
     * System implementers cache.
     *
     * @var   array
     */
    private static $_systemImplementersCache = array();


    /**
     * ChannelsBaker constructor.
     *
     */
    private function __construct()
    {

    }//end __construct()


    /**
     * Adds the system supplied into the system's XML structure.
     *
     * Adds the system to the system list, and then adds all the data from its'
     * plugs to the XML files of other systems, along with
     *
     * @param string $systemPath The system path/name to add to the channels
     *                           structure. If the string is not an absolute
     *                           path the system will be looked for in the
     *                           system's directory.
     *
     * @return void
     */
    public static function addSystem($systemPath)
    {
        self::addSystems(array($systemPath));

    }//end addSystem()


    /**
     * Wrapper function for adding multiple systems at once.
     *
     * Calls addSystem() foreach system supplied in $systems. Returns FALSE
     * if one of the systems is unable to be installed.
     *
     * @param array $systems An array of system names or full system paths
     *                       to add.
     *
     * @return boolean
     * @throws Exception When an exclusive systems' rules are violated.
     */
    public static function addSystems(array $systems)
    {
        $allSystems  = $systems;
        $systemXml   = Channels::getSystemXml();
        $systemNodes = $systemXml->getElementsByTagName('system');
        foreach ($systemNodes as $sysInfo) {
            if ($sysInfo->getAttribute('enabled') === 'yes') {
                $allSystems[] = $sysInfo->getAttribute('path');
            }
        }

        // Setup the systems, checking the database type required for each
        // and excluding any that don't run on the current database.
        self::$_systemsInfo = array();

        $allSystems    = array_unique($allSystems);
        $realSystems   = self::_prepareSystemsToInstall($systems, $allSystems);
        $result        = self::_categoriseSystems($realSystems);
        $concretes     = $result['concretes'];
        $systemSummary = $result['systemSummary'];
        asort($realSystems);
        foreach ($realSystems as $sysName => $sysPath) {
            if (isset($systemSummary[$sysName]) === FALSE) {
                // System name does not exist in $systemSummary array,
                // which means it is a concrete system.
                // By default it becomes its own implementor.
                $implements = $sysName;

                // Let's go through my parent systems.
                foreach ($concretes[$sysName]['parents'] as $parent) {
                    // We only check parent system that is an abstract/interface
                    // systems.
                    if (isset($systemSummary[$parent]['exclusive']) === TRUE && $systemSummary[$parent]['exclusive'] === TRUE) {
                        // Exclusive system found.
                        // Let's work out who implements whom.
                        $someImplementor = $systemSummary[$parent]['implementor'];
                        $doImplement     = TRUE;

                        // Has anybody already implemented?
                        if ($someImplementor !== NULL) {
                            // If so, then is it one of my parents?
                            if (in_array($someImplementor, $concretes[$sysName]['parents']) === FALSE) {
                                // Okay, then are you my child?
                                if (in_array($sysName, $concretes[$someImplementor]['parents']) === FALSE) {
                                    // Oooops. You can not do this.
                                    $errMsg  = $sysName.' tries to implements '.$parent.' system, but it is already';
                                    $errMsg .= 'implemented by '.$someImplementor.'.';
                                    $errMsg .= ' More than one system can not implement exlusive system.';
                                    throw new Exception($errMsg);
                                }

                                $doImplement = FALSE;
                            }
                        }

                        if ($doImplement === TRUE) {
                            $systemSummary[$parent]['implementor'] = $sysName;
                            $implements                            = $parent;
                        }
                    }//end if
                }//end foreach

                $systemSummary[$sysName] = array(
                                            'sysPath'    => $sysPath,
                                            'implements' => $implements,
                                            'interface'  => FALSE,
                                            'abstract'   => FALSE,
                                            'concrete'   => TRUE,
                                           );
            }//end if
        }//end foreach

        foreach ($systemSummary as $sysName => $data) {
            if ($data['abstract'] === TRUE && $data['exclusive'] === FALSE) {
                // E.g. AdjacencyList. We need to set which system it implements.
                foreach ($data['parents'] as $parent) {
                    if (isset($systemSummary[$parent]['exclusive']) === TRUE
                        && $systemSummary[$parent]['exclusive'] === TRUE
                    ) {
                        $systemSummary[$sysName]['implements'] = $parent;
                        break;
                    }
                }
            }
        }

        $ovenDir        = Channels::getOvenPath();
        $systemInfoFile = $ovenDir.'/systemsInfo.txt';
        file_put_contents($systemInfoFile, serialize($systemSummary));
        // Make sure that the MySource Channel is baked.
        if (self::_xmlFileExists('MySource::init') === FALSE) {
            self::_initMySourceXml();
        }

        self::addToSystemList($realSystems, TRUE, $systemSummary);
        foreach ($realSystems as $systemName => $systemPath) {
            if (self::_bakeSystemData($systemName) === FALSE) {
                return FALSE;
            }
        }

    }//end addSystems()


    /**
     * Returns a new array with categorised system info.
     *
     * @param array $realSystems An array of system names or full system paths
     *                           to add.
     *
     * @return boolean
     * @throws Exception When an exclusive systems' rules are violated.
     */
    private static function _categoriseSystems(array $realSystems)
    {
        $result                  = array();
        $result['concretes']     = array();
        $result['systemSummary'] = array();
        foreach ($realSystems as $sysName => $sysPath) {
            $parents     = array();
            $isExclusive = self::$_systemsInfo[$sysName]['exclusive'];
            $actionClass = $sysName.'Actions';
            include_once $sysPath.'/'.$actionClass.'.inc';
            $ref = new ReflectionClass($actionClass);
            if ($ref->isInterface() === TRUE) {
                $result['systemSummary'][$sysName] = array(
                                                      'sysPath'     => $sysPath,
                                                      'exclusive'   => $isExclusive,
                                                      'implementor' => NULL,
                                                      'interface'   => TRUE,
                                                      'abstract'    => FALSE,
                                                      'concrete'    => FALSE,
                                                     );
            } else if ($ref->isAbstract() === TRUE) {
                $result['systemSummary'][$sysName] = array(
                                                      'sysPath'     => $sysPath,
                                                      'exclusive'   => $isExclusive,
                                                      'implementor' => NULL,
                                                      'interface'   => FALSE,
                                                      'abstract'    => TRUE,
                                                      'concrete'    => FALSE,
                                                      'parents'     => self::_getAllParentsSystems($ref),
                                                     );
            } else {
                if ($isExclusive === TRUE) {
                    $errMsg  = $sysName.' is set to be exclusive, but only ';
                    $errMsg .= 'interface/abstract Actions classes are';
                    $errMsg .= ' allowed to be exclusive.';
                    throw new Exception($errMsg);
                }

                $result['concretes'][$sysName] = array(
                                                  'parents' => self::_getAllParentsSystems($ref),
                                                 );
            }//end if
        }//end foreach

        return $result;

    }//end _categoriseSystems()


    /**
     * Populate $realSystem array with some system information.
     *
     * @param array $systems    An array of system names or full system paths
     *                          to add.
     * @param array $allSystems The list of enabled systems.
     *
     * @return boolean
     */
    private static function _prepareSystemsToInstall(
        array $systems,
        array $allSystems
    ) {
        $realSystems   = array();
        $currentDbType = DAL::getDbType();
        foreach ($allSystems as $sysName => $sysPath) {
            $sysClass = $sysName.'System';
            include_once $sysPath.'/'.$sysClass.'.inc';

            $sysObj  = new $sysClass;
            $options = $sysObj->getAllSystemOptions();

            if (in_array($currentDbType, $options['databases']) === FALSE) {
                continue;
            }

            // Cache the system option so that we can use them for
            // exclusivity test.
            self::$_systemsInfo[$sysName] = $options;

            $sysData = array(
                        'exclusive' => $options['exclusive'],
                        'path'      => $sysPath,
                        'children'  => array(),
                       );

            // This system is valid for this system, add it to our array.
            // Check if it was one of the systems we wanted to add first.
            if (in_array($sysPath, $systems) === TRUE) {
                $realSystems[$sysName] = $sysPath;
            }
        }//end foreach

        return $realSystems;

    }//end _prepareSystemsToInstall()


    /**
     * Adds the passed system(s) to the system list.
     *
     * @param array   $systems         An array of systems add to the system list.
     *                                 If it's not an absolute path,
     *                                 then it has to be in the systems dir.
     * @param boolean $enabled         Should the added system be flagged as enabled.
     * @param array   $inheritanceInfo Inheritance information for all systems.
     *
     * @return void
     * @throws ChannelException When the system being added already exists.
     */
    public static function addToSystemList(
        array $systems,
        $enabled=TRUE,
        array $inheritanceInfo=array()
    ) {
        $doc        = Channels::getSystemXml();
        $systemNode = $doc->getElementsByTagName('systems')->item(0);
        if ($enabled === TRUE) {
            $enabledText = 'yes';
        } else {
            $enabledText = 'no';
        }

        self::_setupSystemsInfo($systems, $enabled);

        self::$_systemNames = array_keys(self::$_systemsInfo);

        foreach (self::$_systemsInfo as $systemName => $systemInfo) {
            $newSystemNode = $doc->createElement('system');
            $newSystemNode->setAttribute('name', $systemInfo['name']);
            $newSystemNode->setAttribute('path', $systemInfo['path']);
            $newSystemNode->setAttribute('enabled', $systemInfo['enabled']);

            if (isset($inheritanceInfo[$systemInfo['name']]) === TRUE) {
                $systemTypes = array(
                                'interface',
                                'abstract',
                                'concrete',
                               );
                foreach ($systemTypes as $sType) {
                    if ($inheritanceInfo[$systemName][$sType] === TRUE) {
                        $value = 'true';
                    } else {
                        $value = 'false';
                    }

                    $newSystemNode->setAttribute($sType, $value);
                }

                if ($inheritanceInfo[$systemName]['interface'] === TRUE
                    || $inheritanceInfo[$systemName]['abstract'] === TRUE
                ) {
                    if ($inheritanceInfo[$systemName]['exclusive'] === TRUE) {
                        $exclusive = 'true';
                        $newSystemNode->setAttribute(
                            'implementor',
                            $inheritanceInfo[$systemName]['implementor']
                        );
                    } else {
                        $exclusive = 'false';
                        if ($inheritanceInfo[$systemName]['abstract'] === TRUE
                            && isset($inheritanceInfo[$systemName]['implements']) === TRUE
                        ) {
                            $newSystemNode->setAttribute(
                                'implements',
                                $inheritanceInfo[$systemName]['implements']
                            );
                        }
                    }//end if

                    $newSystemNode->setAttribute('exclusive', $exclusive);
                } else {
                    $newSystemNode->setAttribute(
                        'implements',
                        $inheritanceInfo[$systemName]['implements']
                    );
                }//end if
            }//end if

            $dependencies = self::_dependencySystemArray(
                $systemName,
                $systemInfo['path']
            );

            if (empty($dependencies) === FALSE) {
                $newDependenciesNode = $doc->createElement('dependencies');
                foreach ($dependencies as $data) {
                    $sysName = $data['name'];

                    if ($systemName === $sysName) {
                        continue;
                    }

                    // After it gets all the dependencies from the classes,
                    // it lastly performs type filtering by inheritance. If the
                    // dependent system is a parent type, it updates the type as
                    // 'inheritance'.
                    // All together, there are four types exists. concrete, optional,
                    // inheritance and install.
                    if (isset(self::$_systemsInfo[$systemName]['parents']) === FALSE) {
                        $parent  = self::$_systemsInfo[$systemName]['parent'];
                        $parents = array();
                        while (empty($parent) === FALSE) {
                            $parents[] = $parent;
                            if (isset(self::$_systemsInfo[$parent]) === FALSE) {
                                break;
                            }

                            $parent = self::$_systemsInfo[$parent]['parent'];
                        }

                        self::$_systemsInfo[$systemName]['parents'] = $parents;
                    }

                    if (in_array($sysName, self::$_systemsInfo[$systemName]['parents']) === TRUE) {
                        $typ = 'inheritance';
                    } else {
                        $typ = $data['type'];
                    }

                    $newDependencyNode = $doc->createElement('dependency');
                    $newDependencyNode->setAttribute('system', $sysName);
                    $newDependencyNode->setAttribute('type', $typ);
                    $newDependenciesNode->appendChild($newDependencyNode);
                }//end foreach

                $newSystemNode->appendChild($newDependenciesNode);
            }//end if

            $systemNode->appendChild($newSystemNode);
        }//end foreach

        self::_writeSystemXml($doc);
        self::_writeSystemsList($doc);
        self::_writePlugsList();

    }//end addToSystemList()


    /**
     * Populate self::$_systemsInfo array for further prosessing.
     *
     * @param array   $systems An array of systems add to the system list.
     *                         If it's not an absolute path, then it has to be
     *                         in the systems dir.
     * @param boolean $enabled Should the added system(s) be flagged as enabled.
     *
     * @return void
     * @throws ChannelException When the system being added already exists.
     */
    private static function _setupSystemsInfo(array $systems, $enabled=TRUE)
    {
        $doc        = Channels::getSystemXml();
        $systemNode = $doc->getElementsByTagName('systems')->item(0);
        if ($enabled === TRUE) {
            $enabledText = 'yes';
        } else {
            $enabledText = 'no';
        }

        $currentSystems = array();
        // Check the system is unique.
        foreach ($systemNode->childNodes as $system) {
            if ($system->nodeType === XML_ELEMENT_NODE) {
                $currentSystems[] = $system->getAttribute('name');
            }
        }

        self::$_systemsInfo = array();
        foreach ($systems as $systemName => $systemPath) {
            $path = '';
            if (file_exists($systemPath) === TRUE) {
                // Absolute path.
                $path = $systemPath;
            }

            // Check if system is already in the system list.
            if (in_array($systemName, $currentSystems) === TRUE) {
                throw new ChannelException('System "'.$systemName.'" already exists.');
            }

            self::$_systemsInfo[$systemName]            = array();
            self::$_systemsInfo[$systemName]['name']    = $systemName;
            self::$_systemsInfo[$systemName]['path']    = $path;
            self::$_systemsInfo[$systemName]['enabled'] = $enabledText;

            include_once $path.'/'.$systemName.'Actions.inc';
            $className        = $systemName.'Actions';
            $ref              = new ReflectionClass($className);
            $parentSystemName = self::_getParentSystem($ref, TRUE);
            if ($parentSystemName !== NULL) {
                self::$_systemsInfo[$systemName]['parent'] = $parentSystemName;
            } else {
                self::$_systemsInfo[$systemName]['parent'] = '';
            }
        }//end foreach

    }//end _setupSystemsInfo()


    /**
     * Returns the list of system calls within the system.
     *
     * @param string $sysName The name of the system.
     * @param string $sysPath The path to the system folder.
     *
     * @return array
     * @throws Exception Inconsistency found in system dependencies.
     */
    private static function _dependencySystemArray($sysName, $sysPath)
    {
        $sysInfo = Channels::getSystemList(FALSE, TRUE, FALSE);

        $result       = array();
        $parents      = array();
        $uniqueNames  = array();
        $fileSuffixes = array(
                         'Actions',
                         'System',
                        );

        foreach ($fileSuffixes as $fileSuffix) {
            $className = $sysName.$fileSuffix;
            $filePath  = $sysPath.'/'.$className.'.inc';

            $deps = self::_dependencyGetSystemNamesInActions($className, $filePath);
            if (empty($deps) === FALSE) {
                foreach ($deps as $dep) {
                    $sysN = $dep['name'];
                    $type = $dep['type'];

                    if ($sysN === $sysName) {
                        continue;
                    }

                    if (isset($uniqueNames[$sysN.$type]) === FALSE) {
                        $uniqueNames[$sysN.$type] = TRUE;
                        $result[]                 = array(
                                                     'name' => $sysN,
                                                     'type' => $type,
                                                    );
                    }
                }
            }//end if
        }//end foreach

        return $result;

    }//end _dependencySystemArray()


    /**
     * Returns the list of system calls within the class.
     *
     * After it gets the list from _dependencyGetSystemNamesInFunc()
     * helper function, it does further perform further categorisation.
     * If the dependency was found in System class's install() function,
     * it changes the type as 'install', which indicates the installation time
     * dependency.
     *
     * @param string $className The name of the class.
     * @param string $filePath  The path to the file.
     *
     * @return array
     * @throws Exception Inconsistency found in system dependencies.
     */
    private static function _dependencyGetSystemNamesInActions($className, $filePath)
    {
        include_once $filePath;
        $ref     = new ReflectionClass($className);
        $methods = $ref->getMethods();
        $result  = array();

        if (count($methods) > 0) {
            $text    = file_get_contents($filePath);
            $arr     = explode("\n", $text);
            $systems = array();

            $systemClass = FALSE;
            if (substr($className, -6) === 'System') {
                $systemClass = TRUE;
            }

            $unique = array();
            foreach ($methods as $method) {
                $methodN  = $method->getName();
                $startIdx = ($method->getStartLine() - 1);
                $endIdx   = $method->getEndLine();
                $funcArr  = array_slice($arr, $startIdx, ($endIdx - $startIdx));
                $funcBody = implode("\n", $funcArr);
                $names    = self::_dependencyGetSystemNamesInFunc($funcBody);

                if ($methodN === 'install' && $systemClass === TRUE) {
                    $optionalSystems = self::_dependencyGetOptionalSystems($funcBody);
                    foreach ($optionalSystems as $opSysName) {
                        $result[] = array(
                                     'name' => $opSysName,
                                     'type' => 'install',
                                    );
                    }
                }

                foreach ($names as $data) {
                    $name = $data['name'];
                    $type = $data['type'];
                    $id   = $name.$type;
                    if (isset($unique[$id]) === FALSE) {
                        if ($methodN === 'install' && $systemClass === TRUE) {
                            $type = 'install';
                        }

                        $unique[$id] = TRUE;
                        $result[]    = array(
                                        'name' => $name,
                                        'type' => $type,
                                       );
                    }
                }
            }//end foreach
        }//end if

        return $result;

    }//end _dependencyGetSystemNamesInActions()


    /**
     * Returns the list of system calls within the passed functio body.
     *
     * This function only categorises found dependency either 'concrete' or
     * 'optional'. 'optional' means the Channels::systemExists('SYSTEM_NAME')
     * pattern has been found within the function. It assumes the system is
     * used conditionally.
     *
     * @param string $funcBody The string of function body.
     *
     * @return array
     */
    private static function _dependencyGetSystemNamesInFunc($funcBody)
    {
        $result          = array();
        $matches         = array();
        $optionalSystems = self::_dependencyGetOptionalSystems($funcBody);
        $systemNameList  = array_keys(self::$_systemsInfo);

        $silent  = array(
                    'self',
                    'parent',
                    'PDO',
                    'DAL',
                    'init',
                    'Channels',
                    'BaseSystem',
                   );
        $pattern = '/([a-zA-Z]+)::([a-zA-Z]+)/';
        $matches = array();
        preg_match_all($pattern, $funcBody, $matches);

        if (empty($matches[0]) === FALSE) {
            $matches = array_diff(array_unique($matches[1]), $silent);
            $matches = array_intersect($matches, $systemNameList);

            foreach ($matches as $systemName) {
                if (in_array($systemName, $optionalSystems) === TRUE) {
                    $result[] = array(
                                 'name' => $systemName,
                                 'type' => 'optional',
                                );
                } else {
                    $result[] = array(
                                 'name' => $systemName,
                                 'type' => 'concrete',
                                );
                }
            }
        }

        return $result;

    }//end _dependencyGetSystemNamesInFunc()


    /**
     * Returns the list of optional system names found in the given text.
     *
     * This function only categorises found dependency either 'concrete' or
     * 'optional'. 'optional' means the Channels::systemExists('SYSTEM_NAME')
     * pattern has been found within the function. It assumes the system is
     * used conditionally.
     *
     * @param string $text The string of function body.
     *
     * @return array
     */
    private static function _dependencyGetOptionalSystems($text)
    {
        $optionalPattern = '/Channels::systemExists\(\'([^\']+)\'\)/';
        $matches         = array();
        $result          = array();
        $optionalSystems = array();
        preg_match_all($optionalPattern, $text, $matches);

        if (empty($matches[0]) === FALSE) {
            $result = array_unique($matches[1]);
        }

        return $result;

    }//end _dependencyGetOptionalSystems()


    /**
     * Rebakes PHP Files for all systems.
     *
     * This is to generate new PHP code for each system, from each channel
     * XML file that has been generated.
     *
     * @return void
     * @throws ChannelException When the a system fails to install.
     */
    public static function cleanCodeBake()
    {
        // Bake out the base MySource channel first.
        self::_bakeMySourceCode();

        $systems = Channels::getSystemList();
        foreach ($systems as $systemName) {
            self::_bakeFile($systemName);
        }

        // Now run a check to make sure each system was baked correctly.
        foreach ($systems as $systemName) {
            if (self::systemInstalled($systemName) === FALSE) {
                $error = 'Error in system installation: '.$systemName."\n";
                throw new ChannelException($error);
            }
        }

    }//end cleanCodeBake()


    /**
     * Checks if the given system is installed.
     *
     * First it calls getNonBakedPlugs, to make sure all xml contents are
     * complete. Then it loads the events file (i.e. Assets.inc) and checks
     * the contents of the event function for the plug method call. This
     * function should be called after baking a system to confirm installation.
     *
     * @param string $system System to test.
     *
     * @return boolean
     */
    public static function systemInstalled($system)
    {
        try {
            $nonBaked = self::_getNonBakedPlugs($system);
        } catch (ChannelException $e) {
            return FALSE;
        }

        if (empty($nonBaked) === FALSE) {
            // Check if these are non required plugs.
            foreach ($nonBaked as $nonBakedSystems) {
                foreach ($nonBakedSystems as $nonBakedPlug) {
                    if ($nonBakedPlug['required'] === 'yes') {
                        return FALSE;
                    }
                }
            }
        }

        return TRUE;

    }//end systemInstalled()


    /**
     * Returns all plugs that have not been baked.
     *
     * @param string $system System name.
     *
     * This check is done by reading the [SystemName]Actions.inc file of systems
     * and comparing them to the baked xml files. Does not check baked PHP files.
     *
     * @return array
     */
    private static function _getNonBakedPlugs($system=NULL)
    {
        $systemList = array();
        if ($system === NULL) {
            $systemList = Channels::getSystemList();
        } else {
            $systemList[] = $system;
        }

        $plugList = array();

        foreach ($systemList as $system) {
            if (self::_isInterface($system) === TRUE) {
                continue;
            }

            $systemPlugs = NULL;
            $fileName    = Channels::getSystemsPath($system).'/'.$system.'.xml';
            if (file_exists($fileName) === FALSE) {
                continue;
            } else {
                $systemPlugs = new DOMDocument();
                $systemPlugs->load($fileName);
            }

            $methods = $systemPlugs->getElementsByTagName('method');

            foreach ($methods as $method) {
                if (($method->parentNode->parentNode->tagName !== 'plugs')) {
                    continue;
                }

                include_once 'Channels/Parsers/ChannelsPlugParser.inc';
                $info = ChannelsPlugParser::getPlugInfo($method->parentNode);
                if (isset($info['field']) === TRUE) {
                    $field = $info['field'];
                } else {
                    $field = '';
                }

                if (self::plugExists($info['event'], $info['type'], $system, $info['loc'], $info['method'], $field) === FALSE) {
                    $plugList[$system][] = $info;
                }
            }//end foreach
        }//end foreach

        return $plugList;

    }//end _getNonBakedPlugs()


    /**
     * Rebakes the entire system.
     *
     * All XML files for the system's present are regenerated, and all PHP
     * code is baked out.
     *
     * @return boolean
     */
    public static function cleanBake()
    {
        if (self::cleanDataBake() === FALSE) {
            // Error in data bake. Exit.
            // NB: This should be changed to an exception when something is
            // sitting above it and able to catch this exception.
            exit("Clean Data Bake Failed\n");
        }

        if (self::cleanCodeBake() === FALSE) {
            // Error in code bake. Exit.
            // NB: See comment above about throwing an exception here.
            exit("Clean Code Bake Failed\n");
        }

        return TRUE;

    }//end cleanBake()


    /**
     * Rebakes XML Files for all channels in the system.
     *
     * This is to generate new XML definitions for each channel in the system.
     * This is on a whole system basis as plugs rely on each other so much,
     * any new channels need to have XML files generated before plugs can be
     * added.
     * As the XML files are not as time-sensitive (ie. they don't get run by
     * in real time) we can afford to generate all of the XML files at once.
     * Single plugs are able to be added (by interfaces or triggers etc) by
     * using the addPlug and addPlugXML methods.
     *
     * @return boolean
     */
    public static function cleanDataBake()
    {
        // Remove old files.
        self::_removeBakedDataFiles(array());

        self::_initMySourceXml();

        $systems = Channels::getSystemList();

        // Gather together all the plugs that are in the system.
        foreach ($systems as $systemName) {
            if (self::_bakeSystemData($systemName) === FALSE) {
                // Problem creating the system dir (write permissions?).
                // Should probably return.
                return FALSE;
            }
        }

        return TRUE;

    }//end cleanDataBake()


    /**
     * Bakes out the 'MySource.inc' file.
     *
     * This is treated differently to other baked out code files, as it is
     * created dynamically, rather than from an actions file.
     *
     * @return boolean
     */
    private static function _bakeMySourceCode()
    {
        $mySourceCode = '<'."?php\n\n";

        /*
            As this will be the start of the Channels system's execution
            we neeed to include all of the required files including:
                * Exceptions
                * Channels.inc
        */

        // Include the Channels.inc file.
        $dirName       = dirname(__FILE__);
        $mySourceCode .= 'require_once \''.$dirName.'/Channels.inc\';'."\n";

        // Include Channel Exceptions.
        $exceptions = self::_getChannelsExceptionsFiles();
        foreach ($exceptions as $exceptionFile) {
            $mySourceCode .= 'require_once \''.$exceptionFile.'\';'."\n";
        }

        $mySourceCode .= "class MySource {\n\n";

        foreach (self::$_mySourceChannels as $channelName => $channelData) {
            // Use the same method baker as the rest of the system.
            $mySourceCode .= self::_bakeMethod('MySource', $channelData, 'MySource');
        }

        $mySourceCode .= "\n}\n\n?".'>';

        $ovenFile = Channels::getOvenPath('MySource').'/MySource.inc';
        $success  = self::_writeBakeFile($ovenFile, $mySourceCode);
        return $success;

    }//end _bakeMySourceCode()


    /**
     * Returns the file path of the system's baked php file.
     *
     * @param boolean $fullPath Return Full Paths to the exceptions files, or
     *                          just their file names.
     *
     * @return array
     */
    private static function _getChannelsExceptionsFiles($fullPath=TRUE)
    {
        $dirName           = dirname(__FILE__);
        $exceptionsDirName = $dirName.'/Exceptions';

        $exceptions = array();

        $exceptionDir = new DirectoryIterator($exceptionsDirName);

        while ($exceptionDir->valid()) {
            if ($exceptionDir->isDir() === TRUE) {
                $exceptionDir->next();
                continue;
            }

            $fileName      = $exceptionDir->getFileName();
            $dotPos        = strrpos($fileName, '.');
            $fileExtension = substr($fileName, $dotPos);

            if (strtolower($fileExtension) === '.inc') {
                if ($fullPath === TRUE) {
                    $exceptions[] = $exceptionDir->getPathName();
                } else {
                    $exceptions[] = $exceptionDir->getFileName();
                }
            }

            $exceptionDir->next();
        }//end while

        return $exceptions;

    }//end _getChannelsExceptionsFiles()


    /**
     * Outputs the appropriate PHP code for a system and all of its actions.
     *
     * Each plug has appropriate calls for including its source file, a call
     * to its method, and exception handling depending on whether it is fatal.
     *
     * @param string $system The name of the system to bake.
     *
     * @return void
     * @throws ChannelException When there is a problem with the declaration of
     *                          the system.
     */
    private static function _bakeFile($system)
    {
        // No need to handle now. Its child system will bake for them.
        $systemTypeData = self::_getSystemTypeData($system);
        if ($systemTypeData['interface'] === TRUE
            || $systemTypeData['abstract'] === TRUE
        ) {
            return;
        }

        include_once $systemTypeData['path'].'/'.$system.'Actions.inc';
        $ref         = new ReflectionClass($system.'Actions');
        $parents     = self::_getAllParentsSystems($ref);
        $displayName = $system;
        $methods     = array();
        foreach ($parents as $parent) {
            $parentData = self::_getSystemTypeData($parent);
            if (isset($parentData['exclusive']) === TRUE
                && $parentData['exclusive'] === TRUE
            ) {
                $displayName = $parent;
                break;
            }
        }

        // This is the case that more than one level of inheritance found under
        // exclusive system.
        // For example, Session <- PHPSession <- LocalSession.
        // PHPSession is a concrete system which is not supposed to be baked since
        // Session is an exclusive Interface system. Because of inheritance
        // $displayName is 'Session' but we just have to pass it.
        if (isset($systemTypeData['implements']) === TRUE) {
            if ($displayName !== $systemTypeData['implements']) {
                return;
            }
        }

        // Okay, let's get all the actions from the system. Parent system's actions
        // are handled in _getSystemActions() function.
        $methods = self::_getSystemActions($system);

        // Start the outputting of the file.
        $fileContents  = '';
        $fileContents .= '<'."?php\n\n";
        $fileContents .= "\n\n".'Channels::requireSystemActions(\''.$system.'\');';
        $fileContents .= "\n\n\t\n\n".'class '.$displayName."\n{\n\n";

        // Loop through each method and bake out its' structure.
        foreach ($methods as $method) {
            $fileContents .= self::_bakeMethod($system, $method, $displayName);
        }//end foreach

        // End Class.
        $fileContents .= "}\n\n";
        $fileContents .= '?>';
        // Replace any tabs that were used with spaces.
        $fileContents = str_replace("\t", '    ', $fileContents);
        $ovenFile     = Channels::getOvenPath($displayName).'/'.$displayName.'.inc';

        // Write this file out using a write then copy method.
        self::_writeBakeFile($ovenFile, $fileContents);

    }//end _bakeFile()


    /**
     * Outputs the appropriate PHP code for a system and all of its actions.
     *
     * Each plug has appropriate calls for including its source file, a call
     * to its method, and exception handling depending on whether it is fatal.
     *
     * @param string $system      The name of the system the method is
     *                            from.
     * @param array  $method      Array containing method information.
     * @param string $displayName The name this system is getting baked as.
     *
     * @return string
     * @throws ChannelException If the file for the event isn't found.
     */
    private static function _bakeMethod($system, array $method, $displayName)
    {
        $docComment = '';
        if ($system !== 'MySource') {
            Channels::requireSystemActions($system);
            $refM       = new ReflectionMethod($system.'Actions', $method['name']);
            $docComment = $refM->getDocComment();
        }

        $fileName       = self::getEventFilePath($system.'::'.$method['name']);
        $systemTypeData = self::_getSystemTypeData($system);

        $methodContents = '';
        if (empty($docComment) === FALSE) {
            $methodContents = '    '.$docComment."\n";
        }

        $methodContents .= 'public static function '.$method['name'];
        $parentMethod    = FALSE;
        if ($system !== $method['system']) {
            // An inherited method.
            $parentMethod = TRUE;
            $fileName     = self::getEventFilePath($method['system'].'::'.$method['name']);
        }

        // Get all the plug info.
        $doc = new DomDocument();
        $doc->load($fileName);
        // Retrieve the before and after nodes.
        self::$_currentSystem = $displayName;
        $beforeNode           = $doc->getElementsByTagName('before')->item(0);
        $blockers             = $beforeNode->getElementsByTagName('blocker');
        $beforeContents       = '';
        if ($blockers->length !== 0) {
            $blockerExists = TRUE;
            // If blockers exist they need variable initialisation to work.
            // Maybe one day it's worth cycling over all plugs and
            // asking for initialisation code.
            $beforeContents .= "\n".'// Variable initialisation for blockers.'."\n";
            $beforeContents .= '$retval = FALSE;'."\n";
            $beforeContents .= '$blockerLastRun = 0;'."\n";
            $beforeContents .= '$flowValues = array($blockerLastRun => FALSE);'."\n";
        } else {
            $blockerExists = FALSE;
        }

        foreach ($beforeNode->childNodes as $plug) {
            // Don't process initial gatherers, as they are handled by bakeFile.
            if ($plug->tagName !== 'initial_gatherer') {
                include_once 'Parsers/ChannelsPlugParser.inc';
                $beforeContents .= ChannelsPlugParser::bake($plug);
            }
        }

        $afterNode     = $doc->getElementsByTagName('after')->item(0);
        $afterContents = '';
        foreach ($afterNode->childNodes as $plug) {
            // Don't process initial gatherers, as they are handled by bakeFile.
            if ($plug->tagName !== 'initial_gatherer') {
                include_once 'Parsers/ChannelsPlugParser.inc';
                $afterContents .= ChannelsPlugParser::bake($plug);
            }
        }

        // Initial Gatherers.
        $addParams     = FALSE;
        $initGatherers = $beforeNode->getElementsByTagName('initial_gatherer');
        if ($initGatherers->length !== 0) {
            $addParams = TRUE;
        }

        $paramContent = '('.self::_bakeActionParams($method['params'], FALSE).')';
        // We have atleast one initial gatherer, so we need to expose the
        // $params array to the calling code.
        if ($addParams === TRUE) {
            $newParam          = array();
            $newParam['name']  = 'params';
            $newParam['value'] = 'array()';

            // Add the new param to the params list.
            $method['params'][] = $newParam;
        }

        // Method arguments.
        $methodContents .= '('.self::_bakeActionParams($method['params']).')';
        if ($parentMethod === TRUE) {
            // This is an inherited method, print call to the parent and exit.
            $methodContents .= self::_printParentMethodContents($method);
            return $methodContents;
        }

        // Start method scope, create and add available data to event basket.
        $methodContents .= "\n{\n";

        if (self::$addReportCode === TRUE) {
            $methodContents .= 'DebugExecutionPath::start("'.$system.'::'.$method['name'].'");'."\n";
        }

        $methodContents .= ChannelsDebugger::debugPrintStartChannelCode($method['name']);

        // If this action is fatal then add try catch block.
        if ($method['fatal'] === TRUE) {
            $methodContents .= "try {\n";
            $methodContents .= "DAL::beginTransaction();\n";
        }

        $methodContents .= "Channels::createEventBasket();\n";
        foreach ($method['params'] as $param) {
            if ($param['name'] === 'params') {
                // This is the params array, don't need to add it.
                continue;
            }

            $methodContents .= 'Channels::addToBasket(\''.$param['name'].'\', $'.$param['name'].");\n";
        }

        include_once 'Channels/Parsers/ChannelsInitialGathererParser.inc';
        foreach ($initGatherers as $gatherer) {
            $methodContents .= ChannelsInitialGathererParser::bake($gatherer);
        }

        $methodContents .= $beforeContents;
        $eventFn         = $system.'Actions::'.$method['name'];
        if (isset($method['action']) === TRUE) {
            $eventFn = $method['action'];
        }

        $eventCall       = $eventFn.$paramContent;
        $methodContents .= self::_bakeAction($eventCall, $blockerExists, $method);
        $methodContents .= $afterContents;
        // Remove the event basket for this channel.
        $methodContents .= "Channels::removeEventBasket();\n";
        // If debug mode is set we need to print out the code to end a channel.
        $methodContents .= ChannelsDebugger::debugPrintEndChannelCode($method['system'], $method['name']);

        if ($method['fatal'] === TRUE) {
            $methodContents .= "DAL::commit();\n";
        }

        if (self::$addReportCode === TRUE) {
            $methodContents .= 'DebugExecutionPath::end("'.$system.'::'.$method['name'].'");'."\n";
        }

        $methodContents .= "\nreturn \$retval;\n";

        // If action is fatal, close its try catch block.
        if ($method['fatal'] === TRUE) {
            $methodContents .= "\n} catch(Exception \$e) {\n";
            $methodContents .= "DAL::rollBack();\n";
            $methodContents .= "throw new ChannelException(\$e->getMessage());\n";
            $methodContents .= "}\n";
        }

        // End Method.
        $methodContents      .= "}\n\n";
        self::$_currentSystem = '';
        return $methodContents;

    }//end _bakeMethod()


    /**
     * Bakes parameter list of a function.
     *
     * @param array   $params           List of parameter's, their type,
     *                                  and default value.
     * @param boolean $addDefaultValues If TRUE any param with a "value" attribute
     *                                  will have a default value set for it.
     *
     * @return string
     */
    private static function _bakeActionParams(array $params, $addDefaultValues=TRUE)
    {
        $contents = array();

        foreach ($params as $param) {
            $content = '';

            if (isset($param['passed_by_ref']) === TRUE && $param['passed_by_ref'] === TRUE) {
                $content .= '&';
            }

            $content .= '$'.$param['name'];

            if (isset($param['value']) === TRUE && $addDefaultValues === TRUE) {
                $content .= '='.$param['value'];
            }

            $contents[] = $content;
        }

        $contents = implode(', ', $contents);

        return $contents;

    }//end _bakeActionParams()


    /**
     * Prints out the contents of an inherited method.
     *
     * @param array $method Details about the channel being written.
     *
     * @return string
     */
    private static function _printParentMethodContents(array $method)
    {
        // This method is inherited. As we are not using traditional
        // PHP inheritance in the baked-out files, we emulate this by
        // calling the parent method from our baked method.
        $methodContents  = "\n{\n";
        $methodContents .= "Channels::includeSystem('".$method['system']."');\n";

        if (self::$addReportCode === TRUE) {
            $methodContents .= 'DebugExecutionPath::end("'.$method['system'].'::'.$method['name'].'");'."\n";
        }

        $methodContents .= 'return '.$method['system'].'::'.$method['name'];
        $methodNames     = array();
        foreach ($method['params'] as $paramData) {
            $methodNames[] = '$'.$paramData['name'];
        }

        $methodContents .= '('.implode(', ', $methodNames).");\n";
        $methodContents .= "\n}\n";
        return $methodContents;

    }//end _printParentMethodContents()


    /**
     * Retrieves the actions from the XML document specified.
     *
     * Actions XML file contains list of channels/actions that are availale
     * in a System. Actions can be query or method.
     *
     * @param string  $systemName   System's name.
     * @param boolean $concreteOnly If TRUE, will only return actions defined
     *                              in this system, rather than parent actions.
     *
     * @return array
     * @throws ChannelException When there is no <actions> node present in the
     *                          actions file for a system.
     */
    private static function _getSystemActions($systemName, $concreteOnly=FALSE)
    {
        $actionList      = array();
        $systemPath      = Channels::getSystemsPath($systemName);
        $actionsPath     = $systemPath.'/'.$systemName.'.xml';
        $actionsFilePath = $systemPath.'/'.$systemName.'Actions.inc';

        if ($systemName === '') {
            return $actionList;
        }

        if (file_exists($actionsFilePath) === FALSE) {
            return FALSE;
        }

        Channels::requireSystemActions($systemName);
        $className = $systemName.'Actions';
        $ref       = new ReflectionClass($className);
        $methods   = $ref->getMethods();

        // Does this system have parent system?
        // Then start the recursion here.
        $parentActions = array();
        $parentSystem  = self::_getParentSystem($ref, TRUE);
        if ($parentSystem !== NULL) {
            $parentActions = self::_getSystemActions($parentSystem);
        }

        $actionList = $parentActions;

        // Is there any fatal actions?
        $fatalActionNames = array();
        if (file_exists($actionsPath) === TRUE) {
            $doc = new DomDocument;
            $doc->load($actionsPath);

            $fatalActions = $doc->getElementsByTagName('fatal-actions')->item(0);
            if ($fatalActions !== NULL) {
                $fActions = $fatalActions->getElementsByTagName('action');
                for ($i = 0; $i < $fActions->length; $i++) {
                    $fatalActionNames[] = $fActions->item($i)->getAttribute('name');
                }
            }
        }

        // Let's get the list of method actions from Reflection.
        foreach ($methods as $method) {
            if (($method->isConstructor() === FALSE)
                && ($method->isPublic() === TRUE)
            ) {
                $methodName          = $method->getName();
                $newAction           = array();
                $newAction['name']   = $methodName;
                $newAction['method'] = $methodName;
                $newAction['system'] = $systemName;

                $key = array_search($methodName, $fatalActionNames);
                if ($key === FALSE) {
                    $newAction['fatal'] = FALSE;
                } else {
                    $newAction['fatal'] = TRUE;
                    unset($fatalActionNames[$key]);
                }

                $newAction['params']            = self::_getActionParams($method);
                $actionList[$newAction['name']] = $newAction;
            }//end if
        }//end foreach

        if (empty($fatalActionNames) === FALSE) {
            $errorMsg = $systemName.' System defined '.implode(',', $fatalActionNames).', but the method does not exist.';
            throw new ChannelException($errorMsg);
        }

        return $actionList;

    }//end _getSystemActions()


    /**
     * Returns the system information from systems.xml file.
     *
     * @param string $systemName The name of the system to get the inheritance for.
     *
     * @return array
     */
    private static function _getSystemTypeData($systemName)
    {
        $ovenDir        = Channels::getOvenPath();
        $systemsXmlFile = $ovenDir.'/systems.xml';
        $doc            = new DomDocument();
        $doc->load($systemsXmlFile);
        $systems = $doc->getElementsByTagName('system');
        $type    = array();
        foreach ($systems as $system) {
            $sysName = $system->getAttribute('name');
            if ($sysName === $systemName) {
                if ($system->getAttribute('concrete') === 'true') {
                    $type['concrete'] = TRUE;
                } else {
                    $type['concrete'] = FALSE;
                }

                if ($system->getAttribute('abstract') === 'true') {
                    $type['abstract'] = TRUE;
                } else {
                    $type['abstract'] = FALSE;
                }

                if ($system->getAttribute('interface') === 'true') {
                    $type['interface'] = TRUE;
                } else {
                    $type['interface'] = FALSE;
                }

                $type['path'] = $system->getAttribute('path');
                if ($system->hasAttribute('exclusive') === TRUE) {
                    if ($system->getAttribute('exclusive') === 'true') {
                        $type['exclusive'] = TRUE;
                    } else {
                        $type['exclusive'] = FALSE;
                    }
                }

                if ($system->hasAttribute('implements') === TRUE) {
                    $type['implements'] = $system->getAttribute('implements');
                }

                if ($system->hasAttribute('implementor') === TRUE) {
                    $type['implementor'] = $system->getAttribute('implementor');
                }

                break;
            }//end if
        }//end foreach

        return $type;

    }//end _getSystemTypeData()


    /**
     * Returns list of systems that implements the given system.
     *
     * @param string $system Name of the system.
     *
     * @return array
     */
    private static function _getSystemImplementers($system)
    {
        // If this method was called for the specified system before then return
        // its cached value.
        if (isset(self::$_systemImplementersCache[$system]) === TRUE) {
            return self::$_systemImplementersCache[$system];
        }

        Channels::requireSystemActions($system);

        $imps = array();
        if ($system === 'MySource') {
            $imps[] = $system;
            self::$_systemImplementersCache[$system] = $imps;
            return $imps;
        }

        if (self::_isInterface($system) === FALSE) {
            // Not an interface so return current system.
            $imps[] = $system;
            self::$_systemImplementersCache[$system] = $imps;
            return $imps;
        }

        $typeData = self::_getSystemTypeData($system);
        if ($typeData['interface'] === TRUE) {
            if ($typeData['exclusive'] === TRUE) {
                $imps[] = $typeData['implementor'];
                $imps[] = $system;
                self::$_systemImplementersCache[$system] = $imps;
                return $imps;
            } else {
                $systems = Channels::getSystemList();
                foreach ($systems as $currSystem) {
                    if ($currSystem === $system) {
                        $imps[] = $currSystem;
                        continue;
                    }

                    include_once Channels::getSystemsPath($currSystem).'/'.$currSystem.'Actions.inc';
                    $ref = new ReflectionClass($currSystem.'Actions');
                    if ($ref->implementsInterface($system.'Actions') === TRUE) {
                        $imps[] = $currSystem;
                    }
                }
            }
        }//end if

        self::$_systemImplementersCache[$system] = $imps;
        return $imps;

    }//end _getSystemImplementers()


    /**
     * Returns the list of all parent systems names.
     *
     * This list includes an interface the system implements.
     *
     * @param ReflectionClass $ref ReflectionClass object of the current
     *                             Actions' class to get its parents.
     *
     * @return array
     * @throws Exception If a concrete system implements more than one interfaces.
     */
    private static function _getAllParentsSystems(ReflectionClass $ref)
    {
        $continue = TRUE;
        $parents  = array();
        do {
            $parentC = self::_getParentSystem($ref);
            if ($parentC === NULL) {
                $continue = FALSE;
            } else {
                $name      = $parentC->getName();
                $parents[] = substr($name, 0, -7);
            }

            $ref = $parentC;
        } while ($continue);

        return $parents;

    }//end _getAllParentsSystems()


    /**
     * Returns the parent system's name or ReflectionClass obj.
     *
     * @param ReflectionClass $ref        ReflectionClass object of the current
     *                                    Actions' class to get its parents.
     * @param boolean         $returnName If TRUE, it return the name of the parent
     *                                    system in string.
     *
     * @return array
     * @throws Exception If a concrete system implements more than one interfaces.
     */
    private static function _getParentSystem(ReflectionClass $ref, $returnName=FALSE)
    {
        $result  = NULL;
        $parentC = $ref->getParentClass();
        if ($parentC !== FALSE) {
            // Parent system founds.
            if ($returnName === TRUE) {
                $result = substr($parentC->getName(), 0, -7);
            } else {
                $result = $parentC;
            }
        } else {
            // Maybe it does not extend any other system, but implements Interfaces.
            $interfaces = $ref->getInterfaces();
            if (empty($interfaces) === FALSE) {
                if (count($interfaces) > 1) {
                    $errMsg = $ref->getName().' system is not allowed to implement more than one system.';
                    throw new Exception($errMsg);
                }

                $interfaceName = key($interfaces);
                $parentC       = $interfaces[$interfaceName];
                if ($returnName === TRUE) {
                    $result = substr($parentC->getName(), 0, -7);
                } else {
                    $result = $parentC;
                }
            }
        }//end if

        return $result;

    }//end _getParentSystem()


    /**
     * Deletes all of the generated XML files in the Oven Dir.
     *
     * @param array $systems An array of systems to delete data files for.
     *                       Empty array for all systems(remove all XML Files).
     *
     * @return void
     */
    private static function _removeBakedDataFiles(array $systems=array())
    {
        if (empty($systems) === TRUE) {
            $systems = Channels::getSystemList();
        }

        $baseDir = Channels::getOvenPath();

        $dir = new DirectoryIterator($baseDir);
        while ($dir->valid()) {
            if ($dir->isDot() === TRUE) {
                $dir->next();
                continue;
            }

            if ($dir->isDir() === FALSE) {
                $dir->next();
                continue;
            }

            // Make sure this is in the list of dirs to delete from.
            if (in_array($dir->getFileName(), $systems) === FALSE) {
                $dir->next();
                continue;
            }

            // This call eliminates things like the ".cvs" directory.
            if (self::_isValidSystemName($dir->getFileName()) === FALSE) {
                $dir->next();
                continue;
            }

            $fileName   = $baseDir.'/'.$dir->getFileName();
            $systemFile = new DirectoryIterator($fileName);

            while ($systemFile->valid()) {
                if ($systemFile->isDir() === TRUE) {
                    $systemFile->next();
                    continue;
                }

                $fileName      = $systemFile->getFileName();
                $dotPos        = strrpos($fileName, '.');
                $fileExtension = substr($fileName, $dotPos);

                if (strtolower($fileExtension) === '.xml') {
                    unlink($systemFile->getPath().'/'.$fileName);
                }

                $systemFile->getPath();
                $systemFile->next();
            }

            $dir->next();
        }//end while

    }//end _removeBakedDataFiles()


    /**
     * Bakes out the XML data for one system in the system list.
     *
     * It will add any of this system's plugs to the baked out XML files, but
     * will not bake any PHP code. If the system is disabled or not installed,
     * an Exception will be thrown.
     *
     * @param string $systemName The name of the system to bake data for.
     *
     * @return boolean
     * @throws ChannelException When a plug within the system doesn't validate,
     *                          when the the system is not enabled or doesn't
     *                          exist.
     */
    private static function _bakeSystemData($systemName)
    {
        if (self::_createSystemBakedDir($systemName) === FALSE) {
            // Problem creating the system dir (write permissions?).
            // Throw an Exception.
            throw new ChannelException($systemName.' directory not created.');
        }

        // Creates the XML files for the system's actions.
        self::_initActionsXml(array($systemName));

        $plugsFile = Channels::getSystemsPath($systemName).'/'.$systemName.'.xml';
        if (file_exists($plugsFile) === FALSE) {
            return TRUE;
        }

        $doc = new DomDocument();
        $doc->load($plugsFile);
        $plugsNode = $doc->getElementsByTagName('plugs')->item(0);
        if ($plugsNode === NULL) {
            return TRUE;
        }

        // If we have a list of plugs from other systems that were installed
        // before this system, we need to install them in our newly created
        // XML files for this system.
        $standbyFile = self::_getStandbyFile($systemName);
        $standbyNode = $standbyFile->getElementsByTagName('plugs')->item(0);

        foreach ($standbyNode->childNodes as $standbyPlug) {
            // Import the standby plug, and add it to the existing plug node.
            $imported = $doc->importNode($standbyPlug);
            $plugsNode->appendChild($imported);
        }

        foreach ($plugsNode->childNodes as $plug) {
            if ($plug->nodeType !== 1) {
                // Not an Element Node.
                continue;
            }

            // Make sure each plug is valid.
            $valid = FALSE;
            try {
                include_once 'Channels/Parsers/ChannelsPlugParser.inc';
                $valid = ChannelsPlugParser::validate($plug, $systemName);
            } catch (ChannelException $e) {
                // Problem encountered, re-throw the exception.
                throw $e;
            }

            $plugXml = '<plugs>'.$doc->saveXML($plug).'</plugs>';
            if ($valid === TRUE) {
                self::_addPlug($plugXml, $systemName);
            } else {
                // Plug validation failed, but no exception thrown. This is
                // most likely due to the plug being for a system that does not
                // exist, but is not required. This should then be added to the
                // appropriate data.
                $tempDoc = new DomDocument();
                $tempDoc->loadXML($plugXml);
                $plugsNode   = $tempDoc->getElementsByTagName('plugs')->item(0);
                $missingPlug = $plugsNode->childNodes->item(0);
                $actionNode  = $tempDoc->getElementsByTagName('action')->item(0);
                $event       = $actionNode->nodeValue;
                $eventParts  = explode('::', $event);
                $eventSystem = $eventParts[0];
                $methodNode  = NULL;

                // Set the system the plug came from.
                $plugType = $plug->tagName;
                if ($plugType === 'initial_gatherer') {
                    $methodNode = $missingPlug->getElementsByTagName('field')->item(0);
                } else {
                    $methodNode = $missingPlug->getElementsByTagName('method')->item(0);
                }

                $methodNode->setAttribute('system', $systemName);
                self::_addToStandbyFile($eventSystem, $missingPlug);
            }//end if
        }//end foreach

        return TRUE;

    }//end _bakeSystemData()


    /**
     * Retrieve the XML file that holds plugs that were for this system.
     *
     * @param string $systemName The system to find plugs for.
     *
     * @return DomDocument
     */
    private static function _getStandbyFile($systemName)
    {
        $ovenDir     = Channels::getOvenPath($systemName);
        $standbyFile = $ovenDir.'/nonbakedplugs.xml';
        $doc         = new DomDocument();
        if (file_exists($standbyFile) === TRUE) {
            $doc->load($standbyFile);
        } else {
            $doc->loadXML('<plugs/>');
        }

        return $doc;

    }//end _getStandbyFile()


    /**
     * Adds a plug's XML to the standby file for a system.
     *
     * @param string     $systemName The system to add the plug to.
     * @param DomElement $plug       The node of the plug to add.
     *
     * @return boolean
     */
    private static function _addToStandbyFile($systemName, DomElement $plug)
    {
        $ovenDir     = Channels::getOvenPath($systemName);
        $standbyFile = $ovenDir.'/nonbakedplugs.xml';
        $doc         = new DomDocument();
        if (file_exists($standbyFile) === TRUE) {
            $doc->load($standbyFile);
        } else {
            $doc->loadXML('<plugs/>');
        }

        $plugNode = $doc->getElementsByTagName('plugs')->item(0);
        $newNode  = $doc->importNode($plug, TRUE);

        foreach ($plugNode->childNodes as $current) {
            if ($doc->saveXML($current) === $doc->saveXML($newNode)) {
                // This plug already exists in the standby file.
                return TRUE;
            }
        }

        $plugNode->appendChild($newNode);
        if (file_exists($ovenDir) === FALSE) {
            mkdir($ovenDir);
        }

        $doc->save($standbyFile);
        return TRUE;

    }//end _addToStandbyFile()


    /**
     * Returns TRUE if the system is enabled in the system list definition.
     *
     * @param string $systemName The system to check.
     *
     * @return boolean
     */
    private static function _systemEnabled($systemName)
    {
        $doc = Channels::getSystemXml();

        $systemNodes = $doc->getElementsByTagName('system');

        for ($i = 0; $i < $systemNodes->length; $i++) {
            $systemNode  = $systemNodes->item($i);
            $currentName = $systemNode->getAttribute('name');
            if ($currentName === $systemName) {
                // System is matched, now check the value of the enabled
                // attribute.
                $enabledValue = $systemNode->getAttribute('enabled');
                if ($enabledValue === 'yes') {
                    return TRUE;
                } else {
                    return FALSE;
                }
            }
        }

        return FALSE;

    }//end _systemEnabled()


    /**
     * Takes the raw XML of a plug, reads its event data, and adds it.
     *
     * @param string $xmlSource The raw XML definition of the plug.
     * @param string $system    The system that the plug is defined in.
     *
     * @return void
     * @throws ChannelException When a plug fails to validate.
     */
    private static function _addPlug($xmlSource, $system)
    {
        $doc = new DomDocument();
        $doc->loadXML($xmlSource);
        $plugNode   = $doc->childNodes->item(0)->childNodes->item(0);
        $plugType   = $plugNode->tagName;
        $actionNode = $plugNode->getElementsByTagName('action')->item(0);
        $attrListen = $actionNode->getAttribute('listen');
        if ($attrListen === 'after') {
            $after = TRUE;
        } else {
            $after = FALSE;
        }

        if ($plugType === 'refiner') {
            $after = TRUE;
        }

        $action     = $actionNode->nodeValue;
        $methodNode = NULL;

        // Initial gatherer needs to use it's field node, as there isn't
        // a method node.
        if ($plugType === 'initial_gatherer') {
            $methodNode = $plugNode->getElementsByTagName('field')->item(0);
        } else {
            $methodNode = $plugNode->getElementsByTagName('method')->item(0);
        }

        $methodNode->setAttribute('system', $system);

        /*
            This section deals with determining which systems to add the plug
            to. This plug could specify an interface, which means that the
            plug needs to be added to any system that implements this interface.
            If the system is not an interface, then the system itself will
            be used, and the plug will be added to it only.
        */

        // Get the system of the current action.
        $eventInfo = explode('::', $action);

        // Get all the implementers for this action.
        // If the system is not an interface, then $imps will contain only
        // the action itself.
        $imps = self::_getSystemImplementers($eventInfo[0]);

        foreach ($imps as $implementer) {
            // Create the new action for each implementer.
            $action = $implementer.'::'.$eventInfo[1];
            // Check this is a valid plug.
            include_once 'Channels/Parsers/ChannelsPlugParser.inc';
            $validPlug = ChannelsPlugParser::validate($plugNode, $system);
            if ($validPlug !== TRUE) {
                $error = 'Plug Validation failed ('.$system.'):  '.$validPlug;
                throw new ChannelException($error);
            }

            // Extract any gatherers that the system has.
            $gatherers     = array();
            $gatherersList = $plugNode->getElementsByTagName('gatherer');
            foreach ($gatherersList as $gatherer) {
                $gatMethod = $gatherer->getElementsByTagName('method')->item(0);
                $gatMethod->setAttribute('system', $system);
                $gatherers[] = '<plugs>'.$doc->saveXML($gatherer).'</plugs>';
            }

            if (self::_xmlFileExists($action) === FALSE) {
                self::_createXmlFile($action);
            }

            self::_addPlugXml($action, $doc->saveXML(), $after);
            // Add the gatherers to the source.
            foreach ($gatherers as $gatherer) {
                // Always in the before event, so last argument is always false.
                self::_addPlugXml($action, $gatherer, FALSE);
            }
        }//end foreach

    }//end _addPlug()


    /**
     * Takes the XML source for a plug, and puts it into the channels system.
     *
     * The XML for the plug is integrated into the appropriate channel for the
     * event it is listening for, and its ordering is determined by the type
     * of plug as well as the plugs already present on the channel.
     * This will not modify the PHP code for the system. The system that the
     * plug is acting on will need to be rebaked to apply this plug to the
     * working system.
     *
     * @param string  $event     The event the plug will act on.
     * @param string  $xmlSource The source of the plug.
     * @param boolean $after     Does the plug act on the after event?.
     *
     * @return boolean
     * @throws ChannelException When the plug is trying to be inserted onto a
     *                          query channel but is not allowed.
     */
    private static function _addPlugXml($event, $xmlSource, $after=FALSE)
    {
        $fileName = self::getEventFilePath($event);
        $doc      = new DomDocument();
        $doc->load($fileName);
        $newPlug = new DomDocument();
        $newPlug->loadXML($xmlSource);
        $plugNode = $newPlug->childNodes->item(0)->childNodes->item(0);
        $plugType = $plugNode->tagName;
        // Need to determine the type of the channel, and if it is a query
        // channel, determine if this plug type is allowed.
        $channelNode = $doc->getElementsByTagName('channel')->item(0);
        $channelType = $channelNode->getAttribute('type');
        if ($channelType === 'query') {
            if ($after === TRUE) {
                $msg = 'Plugs not allowed in the after action of a query channel';
                throw new ChannelException($msg);
            }

            // In the before action, so we have to determine if it is valid for
            // a query channel.
            switch ($plugType) {
                case 'blocker':
                    $msg = '\''.$plugType.'\' not allowed in query channel.';
                    throw new ChannelException($msg);
                break;

                default:
                    // This is a valid plug type for a query channel.
                break;
            }
        }

        if ($plugType === 'initial_gatherer') {
            $method = $plugNode->getElementsByTagName('field');
        } else {
            $method = $plugNode->getElementsByTagName('method');
        }

        $method     = $method->item(0);
        $system     = $method->getAttribute('system');
        $methodName = $method->nodeValue;
        $params     = array();
        $paramsNode = $plugNode->getElementsByTagName('parameter');
        foreach ($paramsNode as $newParameter) {
            $params[] = $newParameter->nodeValue;
        }

        if ($after === TRUE) {
            $afterString = 'after';
        } else {
            $afterString = 'before';
        }

        $tagName   = $plugNode->tagName;
        $nodeValue = $method->nodeValue;

        $field = NULL;
        if ($plugType === 'initial_gatherer') {
            $field = $nodeValue;
        } else if ($plugType === 'gatherer') {
            $field = $plugNode->getElementsByTagName('field');
        }

        // Determine if the plug is already in the system, if it is, return FALSE.
        $exists = self::plugExists($event, $tagName, $system, $afterString, $nodeValue, $field);
        if ($exists === TRUE) {
            return FALSE;
        }

        if ($after === TRUE) {
            $targetNode = 'after';
        } else {
            $targetNode = 'before';
        }

        // This will return the appropriate parent for our new plug.
        $parent         = $doc->getElementsByTagName($targetNode)->item(0);
        $newPlugElement = $doc->importNode($plugNode, TRUE);
        include_once 'Parsers/ChannelsPlugParser.inc';
        $nodeAfter = ChannelsPlugParser::getPlugPosition(
            $plugNode,
            $parent,
            $params
        );

        if ($nodeAfter === NULL) {
            // No children, or last child.
            $parent->AppendChild($newPlugElement);
        } else {
            // Insert before the first element with a higher weighting.
            $parent->insertBefore($newPlugElement, $nodeAfter);
        }

        file_put_contents($fileName, $doc->saveXML());
        return TRUE;

    }//end _addPlugXml()


    /**
     * Creates XML files for the MySource Channel.
     *
     * The MySource channel has to be treated differently, as it doesn't have
     * a system directory. Instead, its' channel is generated dynamically.
     * This function prepares its' XML data to a point where systems can define
     * plugs that exist on one of the MySource Channels, and have them added
     * successfully.
     *
     * @return boolean
     */
    private static function _initMySourceXml()
    {
        if (self::_createSystemBakedDir('MySource') === FALSE) {
            return FALSE;
        }

        foreach (self::$_mySourceChannels as $channelName => $channelData) {
            if (self::_createXmlFile('MySource::'.$channelName) === FALSE) {
                return FALSE;
            }
        }

        return TRUE;

    }//end _initMySourceXml()


    /**
     * Creates XML files for each system supplied.
     *
     * This will create clean XML definitions for each action defined in each
     * system specified.
     *
     * @param array $systemList The systems to create files for.
     *
     * @return void
     */
    private static function _initActionsXml(array $systemList)
    {
        foreach ($systemList as $systemName) {
            // We pass in the concrete only flag, as if the parent method is
            // not implemented, we want to use the plugs defined there.
            $actions = self::_getSystemActions($systemName, TRUE);

            foreach ($actions as $action) {
                $event = $systemName.'::'.$action['name'];
                if (isset($action['method']) === TRUE) {
                    $actionType = 'method';
                } else {
                    $actionType = 'query';
                }

                if (self::_xmlFileExists($event) === FALSE) {
                    self::_createXmlFile($event, $actionType);
                }
            }
        }

    }//end _initActionsXml()


    /**
     * Checks that a baked XML file exists for the event provided.
     *
     * @param string $event The name of the event to check.
     *
     * @return boolean
     */
    private static function _xmlFileExists($event)
    {
        $eventFile = self::getEventFilePath($event);
        if (file_exists($eventFile) === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end _xmlFileExists()


    /**
     * Creates an XML file with basic plug structure for the event supplied.
     *
     * Basic plug structure consists of a channel tag with name and date_updated
     * attributes. Channels tag contains child tags before and after.
     *
     * @param string $event       The name of the event to create the file for.
     * @param string $channelType The type of channel (action|query).
     *
     * @return void
     */
    private static function _createXmlFile($event, $channelType='method')
    {
        $xmlStr = '<?xml version="1.0"?>
            <channel name="'.$event.'" type="'.$channelType.'">
            <before/>
            <after/>
            </channel>';
        // Write out the file.
        $fileName = self::getEventFilePath($event);
        self::_writeBakeFile($fileName, $xmlStr);

    }//end _createXmlFile()


    /**
     * Creates a baked file dir for the system specified.
     *
     * @param string $system Name of the system to find the directory for.
     *
     * @return boolean
     */
    private static function _createSystemBakedDir($system)
    {
        $dir = Channels::getOvenPath().'/'.$system;

        if (file_exists($dir) === TRUE) {
            return TRUE;
        }

        $dirCreateSuccess = mkdir($dir);
        return $dirCreateSuccess;

    }//end _createSystemBakedDir()


    /**
     * Bakes out the code to call the action.
     *
     * @param string $eventCall     The Action Call Signature.
     * @param string $blockerExists The source of the plug.
     * @param array  $method        Array containing method information.
     *
     * @return string
     */
    private static function _bakeAction(
        $eventCall,
        $blockerExists,
        array $method=array()
    ) {
        $actionContents  = "\n";
        $actionContents .= '/*'."\n";
        $actionContents .= '=================================================='."\n";
        if ($method['fatal'] === TRUE) {
            $actionContents .= 'Fatal Action.'."\n";
        } else {
            $actionContents .= 'Non Fatal Action.'."\n";
        }

        $actionContents .= ''.$eventCall."\n";
        $actionContents .= '=================================================='."\n";
        $actionContents .= '*/'."\n";

        if ($blockerExists === TRUE) {
            // We have a blocker, only run if retval is equal to the most recently
            // run blockers flow value.
            $actionContents .= 'if ($retval === $flowValues[$blockerLastRun]) {'."\n";
        }

        // Output the action call.
        $actionContents .= "try {\n";
        $actionContents .= '    $retval = '.$eventCall.";\n";
        $actionContents .= "} catch (ChannelActionException \$e) {\n";
        // Print the end action code for debugging.
        $actionContents .= ChannelsDebugger::debugPrintEndActionCode();
        $actionContents .= '    throw new ChannelException($e->getMessage()';
        $actionContents .= ");\n";
        $actionContents .= "}\n";

        if ($blockerExists === TRUE) {
            // Close the blocker "IF" statement.
            $actionContents .= "}\n";
            $actionContents .= "Channels::modifyBasket('retval', \$retval);\n";
        } else {
            $actionContents .= "Channels::addToBasket('retval', \$retval);\n";
        }

        return $actionContents;

    }//end _bakeAction()


    /**
     * Bakes out a warning for each baked out class.
     *
     * @return string
     */
    private static function _bakeCodeHeader()
    {
        $header  = "\n\n\t/**\n";
        $header .= "\t* This code was generated by the channels system.\n";
        $header .= "\t* Do not modify this code.\n";
        $header .= "\t*/\n\n";
        return $header;

    }//end _bakeCodeHeader()


    /**
     * Returns list of parameters for specified parent node.
     *
     * @param ReflectionMethod $method The ReflectionMethod obj to retrieve
     *                                 the signature information.
     *
     * @return array
     * @throws ChannelException When one of the params has no name or its' name
     *                          is reserved within channels.
     */
    private static function _getActionParams(ReflectionMethod $method)
    {
        $result     = array();
        $parameters = $method->getParameters();
        foreach ($parameters as $param) {
            $newParam = array();

            if ($param->getName() === 'params') {
                throw new ChannelException('params is a reserved var name.');
            }

            $newParam['name'] = $param->getName();
            if ($param->isDefaultValueAvailable() === TRUE) {
                $newParam['value'] = var_export($param->getDefaultValue(), 1);
                if ($param->isArray() === TRUE) {
                    $newParam['type']  = 'array';
                    $newParam['value'] = str_replace("\n", '', $newParam['value']);
                } else {
                    $class = $param->getClass();
                    if ($class !== NULL) {
                        $newParam['type'] = $class->getName();
                    }
                }
            }

            if ($param->isPassedByReference() === TRUE) {
                $newParam['passed_by_ref'] = TRUE;
            }

            $result[] = $newParam;
        }//end foreach

        return $result;

    }//end _getActionParams()


    /**
     * Writes out a file in a two-step process.
     *
     * Writes out the file to a temporary directory first, before copying it
     * over to its final destination. This ensures that the time the
     * destination file is non-readable is minimal.
     *
     * @param string $fileName The actual file name that the file will be
     *                         written to.
     * @param string $contents The content to write to the file.
     *
     * @return boolean
     */
    private static function _writeBakeFile($fileName, $contents)
    {
        $dirName = dirname($fileName);
        if (file_exists($dirName) === FALSE) {
            mkdir($dirName);
        }

        $baseName = basename($fileName);

        $res = file_put_contents($fileName, $contents);
        if ($res === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end _writeBakeFile()


    /**
     * Writes the system list cache file.
     *
     * @param DomDocument $doc The object containing the XML structure.
     *
     * @return void
     * @throws Exception Data directory does not exist.
     */
    private static function _writeSystemsList(DomDocument $doc)
    {
        $dataDir = Channels::getOvenPath();
        if (file_exists($dataDir) === FALSE) {
            throw new Exception('Data directory does not exist.');
        }

        $body = '<?php
        class Systems
        {
            public static $systems = array('."\n";

        $systems = $doc->getElementsByTagName('system');
        foreach ($systems as $system) {
            $systemName = $system->getAttribute('name');
            $body      .= '\''.$systemName.'\' => TRUE,'."\n";
        }

        $body .= ');
        }';
        $body .= '?>';
        file_put_contents($dataDir.'/systems.inc', $body);

    }//end _writeSystemsList()


    /**
     * Writes plugs.xml file to cache the list of all plugs in the system.
     *
     * @return void
     */
    private static function _writePlugsList()
    {
        $files = self::getEventList();
        include_once 'Channels/Parsers/ChannelsPlugParser.inc';
        $locs = ChannelsPlugParser::getLocTypes();

        $types = ChannelsPlugParser::getPlugTypes();

        $plugList = array();
        $doc      = new DOMDocument();

        $plugsDoc = new DOMDocument();
        $plugsDoc->loadXML('<plugs />');
        $plugsElem = $plugsDoc->getElementsByTagName('plugs')->item(0);
        foreach ($files as $systemDir => $eventList) {
            foreach ($eventList as $eventFile) {
                if (file_exists($eventFile) === FALSE
                    || basename($eventFile) === 'nonbakedplugs.xml'
                ) {
                    continue;
                }

                $doc->load($eventFile);
                foreach ($locs as $currentLoc) {
                    // Before or after.
                    $locPlugs = $doc->getElementsByTagName($currentLoc)->item(0);
                    foreach ($locPlugs->childNodes as $plug) {
                        if ($plug->nodeType !== XML_ELEMENT_NODE) {
                            continue;
                        }

                        $imported = $plugsDoc->importNode($plug, TRUE);
                        $plugsElem->appendChild($imported);
                    }//end foreach
                }//end foreach
            }//end foreach
        }//end foreach

        $ovenDir   = Channels::getOvenPath();
        $plugsFile = $ovenDir.'/plugs.xml';
        $plugsDoc->save($plugsFile);

    }//end _writePlugsList()


    /**
     * Saves the data in the DomDocument to the system list.
     *
     * @param DomDocument $doc The object containing the XML structure.
     *
     * @return boolean
     */
    private static function _writeSystemXml(DomDocument $doc)
    {
        // Set the data to be readable.
        $doc->formatOutput = TRUE;
        // Get the systems XML file, and output the data.
        $ovenDir    = Channels::getOvenPath();
        $systemFile = $ovenDir.'/systems.xml';
        file_put_contents($systemFile, $doc->saveXML());

        return TRUE;

    }//end _writeSystemXml()


    /**
     * Validates the given system name.
     *
     * @param string $systemName System name to validate.
     *
     * @return boolean
     */
    private static function _isValidSystemName($systemName)
    {
        if (($systemName === '') || (strpos($systemName, '.') !== FALSE)) {
            return FALSE;
        }

        return TRUE;

    }//end _isValidSystemName()


    /**
     * Checks if the given system is an interface.
     *
     * @param string $system Name of the system.
     *
     * @return boolean
     * @throws ChannelException When the file doesn't exist or there is no
     *                          'actions' node present in the actions file.
     */
    private static function _isInterface($system)
    {
        if ($system === '' || strpos($system, 'Widget') !== FALSE) {
            return FALSE;
        }

        $sysActionsPath = Channels::getSystemsPath($system).'/'.$system.'Actions.inc';
        if (file_exists($sysActionsPath) === FALSE) {
            $msg = 'Actions file not found for '.$system.' at '.$sysActionsPath;
            throw new ChannelException($msg);
        }

        include_once $sysActionsPath;
        $ref = new ReflectionClass($system.'Actions');
        if ($ref->isInterface() === TRUE) {
            return TRUE;
        }

        return FALSE;

    }//end _isInterface()


    /**
     * Validates the given method name.
     *
     * Validates by checking that actions.xml has this defined.
     *
     * @param string $actionStr        SystemName::method.
     * @param string $returnActionList Optionally if it is TRUE, then
     *                                 it returns the list of actions.
     *
     * @return boolean
     */
    public static function actionMethodExists($actionStr, $returnActionList=FALSE)
    {
        if (($actionStr === '') || $actionStr === NULL) {
            return FALSE;
        }

        list($systemName, $action) = explode('::', $actionStr);
        if (self::validateSystem($systemName) === FALSE) {
            return FALSE;
        }

        if ($systemName === 'MySource') {
            $actionInChannels = in_array(
                $action,
                array_keys(self::getMySourceChannels())
            );
            return $actionInChannels;
        }

        $sysActions = self::_getSystemActions($systemName);
        $result     = array_key_exists($action, $sysActions);

        if ($result === TRUE && $returnActionList === TRUE) {
            return $sysActions;
        }

        return $result;

    }//end actionMethodExists()


    /**
     * Creates a new empty XML file for the sytems to be stored in.
     *
     * @return void
     */
    public static function refreshSystemList()
    {
        $ovenDir     = Channels::getOvenPath();
        $systemFile  = $ovenDir.'/systems.xml';
        $initialXml  = '<systems>';
        $initialXml .= "</systems>\n";

        $xml = new SimpleXMLElement($initialXml);

        self::_writeBakeFile($systemFile, $xml->asXML());

    }//end refreshSystemList()


    /**
     * Enables a list of systems in the system.xml file.
     *
     * NB: This does NOT bake the new data files.
     *
     * @param array $systems Systems to enable.
     *
     * @return void
     */
    public static function enableSystems(array $systems)
    {
        $doc = Channels::getSystemXml();

        $systemNodes = $doc->getElementsByTagName('system');

        for ($i = 0; $i < $systemNodes->length; $i++) {
            $systemNode = $systemNodes->item($i);
            $systemName = $systemNode->getAttribute('name');
            if (in_array($systemName, $systems) === TRUE) {
                $systemNode->setAttribute('enabled', 'yes');
            }
        }

        self::_writeSystemXml($doc);

    }//end enableSystems()


    /**
     * Disables a list of systems in the system.xml file.
     *
     * NB: This does NOT bake the new data files.
     *
     * @param array $systems Systems to disable.
     *
     * @return void
     */
    public static function disableSystems(array $systems)
    {
        $doc = Channels::getSystemXml();

        $systemNodes = $doc->getElementsByTagName('system');

        for ($i = 0; $i < $systemNodes->length; $i++) {
            $systemNode = $systemNodes->item($i);
            $systemName = $systemNode->getAttribute('name');
            if (in_array($systemName, $systems) === TRUE) {
                $systemNode->setAttribute('enabled', 'no');
            }
        }

        self::_writeSystemXml($doc);

    }//end disableSystems()


    /**
     * Removes the systems supplied from the systems list.
     *
     * @param array $systems An array of systems remove from the system list.
     *
     * @return void
     */
    public static function removefromSystemsList(array $systems)
    {
        $doc        = Channels::getSystemXml();
        $systemNode = $doc->getElementsByTagName('systems')->item(0);

        // Check the system is unique.
        foreach ($systemNode->childNodes as $system) {
            if ($system->nodeType !== XML_ELEMENT_NODE) {
                continue;
            }

            $currentName = $system->getAttribute('name');
            if (in_array($currentName, $systems) === TRUE) {
                $systemNode->removeChild($system);
            }
        }

        self::_writeSystemXml($doc);

    }//end removefromSystemsList()


    /**
     * Retrieve the attributes of an XML Node.
     *
     * @param DomElement $element Element to retrieve attributes for.
     *
     * @return array
     */
    public static function getElementAttributes(DomElement $element)
    {
        $attributes = array();
        foreach ($element->attributes as $attribute) {
            $attributes[$attribute->name] = $attribute->value;
        }

        return $attributes;

    }//end getElementAttributes()


    /**
     * Removes a plug from the system.
     *
     * @param string $event  The event the plug lies on.
     * @param string $type   The type of plug (runner, switcher etc).
     * @param string $system The system the plug was defined in.
     * @param string $loc    The location of the plug ("before" or "after").
     * @param string $method The method the plug calls.
     * @param string $field  The field that initial gatherer uses.
     *
     * @return boolean
     */
    public static function removePlug(
        $event,
        $type,
        $system,
        $loc,
        $method=NULL,
        $field=NULL
    ) {
        $res = self::plugExists($event, $type, $system, $loc, $method, $field, TRUE);
        return $res;

    }//end removePlug()


    /**
     * Removes all the plugs defined in the channels from a certain system.
     *
     * Used when a System has it's source files deleted, and all plug's that
     * it had defined are no longer valid. This will remove them without having
     * to know what they were.
     *
     * @param string $system System to remove plugs for.
     * @param string $method Method to remove plugs for.
     *
     * @return void
     */
    public static function removeSystemPlugs($system, $method=NULL)
    {
        include_once dirname(dirname(__FILE__)).'/Libs/FileSystem/FileSystem.inc';
        $dirName  = Channels::getOvenPath();
        $xmlFiles = FileSystem::listDirectory($dirName, array('.xml'));

        foreach ($xmlFiles as $xmlPath) {
            $fileName = basename($xmlPath);
            if ($fileName === 'systems.xml') {
                continue;
            }

            $doc = new DOMDocument();
            $doc->load($xmlPath);
            $methodElements = $doc->getElementsByTagName('method');
            foreach ($methodElements as $methodNode) {
                if ($method !== NULL) {
                    if ($methodNode->nodeValue !== $method) {
                        continue;
                    }
                }

                if ($methodNode->getAttribute('system') === $system) {
                    $parent         = $methodNode->parentNode;
                    $parentOfParent = $parent->parentNode;
                    $parentOfParent->removeChild($parent);
                }
            }

            file_put_contents($xmlPath, $doc->saveXML());
        }//end foreach

    }//end removeSystemPlugs()


    /**
     * Determines if a plug exists and optionally removes it.
     *
     * This method is used to ensure that duplicate plugs are not inserted
     * into the channels system, as well as removal of plugs if they exist.
     * Note: One of the method or field argument must be provided. Field is
     * used to check initial gatherer types since they dont have method element.
     *
     * @param string  $event  The event the plug lies on.
     * @param string  $type   The type of plug (runner, switcher etc).
     * @param string  $system The system the plug was defined in.
     * @param string  $loc    The location of the plug ("before" or "after").
     * @param string  $method The method the plug calls.
     * @param string  $field  The field that initial gatherer use.
     * @param boolean $remove Should this plug be removed if found?.
     *
     * @return boolean
     */
    public static function plugExists(
        $event,
        $type,
        $system,
        $loc,
        $method=NULL,
        $field=NULL,
        $remove=FALSE
    ) {
        $fileName = self::getEventFilePath($event);

        if (file_exists($fileName) === FALSE) {
            return FALSE;
        }

        $doc = new DOMDocument();
        $doc->load($fileName);

        // Get all the method elements.
        $locElements = $doc->getElementsByTagName($loc);

        if (($method === NULL) && ($field === NULL)) {
            return FALSE;
        }

        if ($type === 'initial_gatherer') {
            $check         = 'field';
            $methodOrField = $field;
        } else {
            $check         = 'method';
            $methodOrField = $method;
        }

        foreach ($locElements as $currentLoc) {
            $typeElements = $currentLoc->getElementsByTagName($type);
            foreach ($typeElements as $currentPlug) {
                $methods       = $currentPlug->getElementsByTagName($check);
                $currentMethod = $methods->item(0);
                if ($currentMethod->nodeValue === $methodOrField) {
                    if ($currentMethod->getAttribute('system') === $system) {
                        // If this is a gatherer then check its field.
                        if ($field !== NULL && $type === 'gatherer') {
                            $fieldValue = $currentPlug->getElementsByTagName('field');
                            $fieldValue = $fieldValue->item(0);
                            if ($fieldValue !== NULL) {
                                if ($fieldValue->nodeValue !== $field) {
                                    continue;
                                }
                            }
                        }

                        if ($remove === TRUE) {
                            $currentLoc->removeChild($currentPlug);
                            // Write changes to file.
                            $fileName = self::getEventFilePath($event);
                            file_put_contents($fileName, $doc->saveXML());
                        }

                        return TRUE;
                    }//end if
                }//end if
            }//end foreach
        }//end foreach

        return FALSE;

    }//end plugExists()


    /**
     * Checks if the given system exists.
     *
     * @param string $systemName Name of the system to check.
     *
     * @return boolean
     */
    public static function systemExists($systemName)
    {
        if ($systemName === 'MySource') {
            return TRUE;
        }

        $res     = TRUE;
        $sysPath = Channels::getSystemsPath($systemName);
        if (empty($sysPath) === TRUE) {
            $res = FALSE;
        }

        return $res;

    }//end systemExists()


    /**
     * Returns the list of MySource Channels.
     *
     * @return array
     */
    public static function getMySourceChannels()
    {
        return self::$_mySourceChannels;

    }//end getMySourceChannels()


    /**
     * Validates the given System by checking that its required files exist.
     *
     * @param string $systemName System that this plug belongs to.
     *
     * @return boolean
     * @throws ChannelException The given system is not valid.
     */
    public static function validateSystem($systemName)
    {
        if (($systemName === '') || ($systemName === NULL)) {
            return FALSE;
        }

        if ($systemName === 'MySource') {
            return TRUE;
        }

        $systemsDir = Channels::getSystemsPath($systemName);

        if (file_exists($systemsDir.'/'.$systemName.'Actions.inc') === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end validateSystem()


    /**
     * Validates the given System's inheritance.
     *
     * @param string $systemName System name to validate.
     *
     * @return boolean
     * @throws Exception If inheritance inconsistency found.
     */
    public static function validateSystemInheritance($systemName)
    {
        if (($systemName === '') || ($systemName === NULL)) {
            return FALSE;
        }

        if ($systemName === 'MySource') {
            return TRUE;
        }

        $systemsDir = Channels::getSystemsPath($systemName);

        $actionFilePath = $systemsDir.'/'.$systemName.'Actions.inc';
        if (file_exists($systemsDir.'/'.$systemName.'Actions.inc') === FALSE) {
            $errorMsg = $systemName.'Actions.inc file does not exist.';
            throw new Exception($errorMsg);
        }

        include_once $actionFilePath;

        $systemFilePath = $systemsDir.'/'.$systemName.'System.inc';
        if (file_exists($systemsDir.'/'.$systemName.'System.inc') === FALSE) {
            $errorMsg = $systemName.'System.inc file does not exist.';
            throw new Exception($errorMsg);
        }

        include_once $systemFilePath;

        $actionRef = new ReflectionClass($systemName.'Actions');
        $systemRef = new ReflectionClass($systemName.'System');

        $widgetSystem = FALSE;
        if (strlen($systemName) > 6 && substr($systemName, -6) === 'Widget') {
            include_once $systemsDir.'/'.$systemName.'Type.inc';
            $typeRef       = new ReflectionClass($systemName.'Type');
            $typeParentRef = $typeRef->getParentClass();
            $widgetSystem  = TRUE;
        }

        $actionParentRef = $actionRef->getParentClass();
        $systemParentRef = $systemRef->getParentClass();

        if ($actionParentRef === FALSE && $systemParentRef === FALSE) {
            $errorMsg = 'Please make '.$systemName.'\'s System class extends from ';
            if ($widgetSystem === TRUE) {
                $errorMsg .= 'AbstractWidgetSystem.';
            } else {
                $errorMsg .= 'BaseSystem.';
            }

            throw new Exception($errorMsg);
        } else if ($actionParentRef === FALSE && $systemParentRef !== FALSE) {
            $systemParentName = $systemParentRef->getName();
            if ($systemParentRef->getName() !== 'BaseSystem') {
                if ($actionRef->implementsInterface(substr($systemParentName, 0, -6).'Actions') === FALSE) {
                    $errorMsg = $systemName.', Actions class does not have any parent class. Please make System class extends from BaseSystem';
                    throw new Exception($errorMsg);
                }
            }
        } else {
            $actionParentName = $actionParentRef->getName();
            $systemParentName = $systemParentRef->getName();
            if (substr($actionParentName, 0, -7) !== substr($systemParentName, 0, -6)) {
                $errorMsg = 'In '.$systemName.' system, Inheritance mismatches between Actions and System classes.';
                throw new Exception($errorMsg);
            }

            if ($widgetSystem === TRUE) {
                $typeParentName = $typeParentRef->getName();
                if (substr($systemParentName, 0, -6) !== substr($typeParentName, 0, -4)) {
                    $errorMsg = 'In '.$systemName.' widget system, Inheritance mismatches between System and Type classes.';
                    throw new Exception($errorMsg);
                }
            }
        }//end if

        return TRUE;

    }//end validateSystemInheritance()


    /**
     * Returns the event file path (xml).
     *
     * @param string $event The event string.
     *
     * @return string
     */
    public static function getEventFilePath($event)
    {
        $fileName  = Channels::getOvenPath().'/'.self::getEventSystem($event).'/';
        $fileName .= self::_getEventFileName($event);
        return $fileName;

    }//end getEventFilePath()


    /**
     * Creates a friendly file name from an event name.
     *
     * @param string $event The event to create the file name for.
     *
     * @return string
     */
    private static function _getEventFileName($event)
    {
        $retval = self::getEventAction($event).'.xml';
        return $retval;

    }//end _getEventFileName()


    /**
     * Returns the system of the given event.
     *
     * For example: AM::getAsset, returns AM.
     *
     * @param string $event The event string.
     *
     * @return string
     */
    public static function getEventSystem($event)
    {
        $res = self::_parseEventName($event);
        return $res;

    }//end getEventSystem()


    /**
     * Returns the action of the given event.
     *
     * For example: AM::getAsset, returns getAsset.
     *
     * @param string $event The event string.
     *
     * @return string
     */
    public static function getEventAction($event)
    {
        $res = self::_parseEventName($event, FALSE);
        return $res;

    }//end getEventAction()


    /**
     * Returns the action of the given event.
     *
     * By default it returns the system part from the event name.
     * For example: AM::getAsset, returns AM. If it's FALSE, it
     * returns getAsset.
     *
     * @param string $event        The event string.
     * @param string $returnSystem If FALSE it returns the action name instead.
     *
     * @return string
     * @throws ChannelException When the event string is empty.
     */
    private static function _parseEventName($event, $returnSystem=TRUE)
    {
        if (is_string($event) === FALSE) {
            throw new ChannelException('Event must be string.');
        }

        $parts = explode('::', $event);

        if ((empty($parts) === TRUE) || (count($parts) !== 2)) {
            throw new ChannelException('Event string "'.$event.'" is invalid.');
        }

        $targetIdx = 0;
        if ($returnSystem === FALSE) {
            $targetIdx = 1;
        }

        if ($parts[$targetIdx] === '') {
            $error = 'Empty string found for in "'.$event.'".';
            throw new ChannelException($error);
        }

        return $parts[$targetIdx];

    }//end _parseEventName()


    /**
     * Returns all events/channels that are currently baked.
     *
     * Returns the full file path or the event names only. Returned array is
     * indexed by system name.
     *
     * @param boolean $namesOnly If true, only the event names will be returned.
     *                           Else full file paths.
     *
     * @return array
     */
    public static function getEventList($namesOnly=FALSE)
    {
        $eventList = array();
        $dir       = new DirectoryIterator(Channels::getOvenPath());
        while ($dir->valid() === TRUE) {
            if (($dir->isDot() === FALSE) && ($dir->isDir() === TRUE)) {
                if (self::_isValidSystemName($dir->getFilename()) === TRUE) {
                    $path       = $dir->getPath().'/'.$dir->getFilename();
                    $eventFiles = self::_getEventFiles($path, $namesOnly);
                    // Add the events to the running array of events.
                    $eventList[$dir->getFilename()] = $eventFiles;
                }
            }

            $dir->next();
        }

        return $eventList;

    }//end getEventList()


    /**
     * Returns list of events for a given path.
     *
     * @param string  $path      Path of the system.
     * @param boolean $namesOnly If true, only the event names will be returned.
     *                           Else full file paths.
     *
     * @return array
     */
    private static function _getEventFiles($path, $namesOnly=FALSE)
    {
        $events = array();
        $dir    = new DirectoryIterator($path);
        while ($dir->valid() === TRUE) {
            $eventName = substr($dir->getFilename(), -3);

            if (($dir->isDot() === FALSE) && ($eventName === 'xml')) {
                // If this is an xml file then add it to the list.
                if ($namesOnly === TRUE) {
                    $events[] = $dir->getFilename();
                } else {
                    $events[] = $dir->getPath().'/'.$dir->getFilename();
                }
            }

            $dir->next();
        }

        return $events;

    }//end _getEventFiles()


    /**
     * Add indentation level to the passed string.
     *
     * @param string  $string String to add indentation in front.
     * @param integer $spaces Number of spaces to add.
     *
     * @return array
     */
    public static function indent($string, $spaces=4)
    {
        $spaces   = str_repeat(' ', $spaces);
        $newLines = array();
        $lines    = explode("\n", $string);
        foreach ($lines as $line) {
            $newLines[] = $spaces.$line;
        }

        $implodedLines = implode("\n", $newLines);
        return $implodedLines;

    }//end indent()


}//end class

?>
