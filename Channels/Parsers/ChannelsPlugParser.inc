<?php
/**
 * Main class for the Parsing of Plugs into string.
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

require_once 'Channels/Parsers/ChannelsBlockerParser.inc';
require_once 'Channels/Parsers/ChannelsGathererParser.inc';
require_once 'Channels/Parsers/ChannelsInitialGathererParser.inc';
require_once 'Channels/Parsers/ChannelsModifierParser.inc';
require_once 'Channels/Parsers/ChannelsRefinerParser.inc';
require_once 'Channels/Parsers/ChannelsRunnerParser.inc';
require_once 'Channels/Parsers/ChannelsSwitcherParser.inc';
require_once 'Libs/XML/XML.inc';
require_once 'Channels/Debug/ChannelsDebugger.inc';
require_once 'Channels/ChannelsBaker.inc';

/**
 * ChannelsPlugParser.
 *
 * Base query parser. Contains common parsing functions like field parser etc.
 *
 */
class ChannelsPlugParser
{

    /**
     * Cache for the required elements list.
     *
     * @var   arary
     */
    private static $_requiredElementsCache = array();


    /**
     * Constructs an baked code for plug from DomElement.
     *
     * @param DOMElement $xmlNode DomElement that represents a plug.
     *
     * @return string
     */
    public static function bake(DOMElement $xmlNode)
    {
        $plugCode  = '';
        $fatalNode = $xmlNode->getElementsByTagName('fatal');
        if ($fatalNode->length !== 0) {
            $fatalNode  = $fatalNode->item(0);
            $fatalValue = strtolower($fatalNode->nodeValue);
            if ($fatalValue === 'yes') {
                $isFatal = TRUE;
            } else {
                $isFatal = FALSE;
            }
        } else {
            $isFatal = FALSE;
        }

        $plugSystem       = self::getPlugSystem($xmlNode);
        $validationSystem = $plugSystem;
        $plugSystem       = self::findPlugImplementor($plugSystem);

        $plugCode .= "\n";

        $plugCode .= '/*'."\n";
        $plugCode .= '___________________________________________________'."\n";
        $plugCode .= 'System '.$plugSystem.' '.ucwords($xmlNode->tagName)." plug.\n";
        $plugCode .= '___________________________________________________'."\n";
        $plugCode .= '*/'."\n";

        if (Channels::$channelsMode === 'testing') {
            $plugCode .= call_user_func(
                array(
                 self::getPlugHandler($xmlNode->tagName),
                 'getTestingPlugIfCode',
                ),
                $plugSystem,
                $xmlNode
            );
        }

        if (ChannelsBaker::$addReportCode === TRUE) {
            $plugCode .= 'DebugExecutionPath::setChannelType(\'';
            $plugCode .= $xmlNode->tagName.'\');'."\n";
        }

        $plugCode .= "include_once '".Channels::getOvenPath($plugSystem);
        $plugCode .= '/'.$plugSystem.".inc';\n";

        $plugCode .= "try {\n";
        $plugCode .= ChannelsDebugger::debugPrintStartPlugCode(
            $plugSystem,
            $xmlNode
        );

        // Validate.
        call_user_func(
            array(
             self::getPlugHandler($xmlNode->tagName),
             'validate',
            ),
            $xmlNode,
            $validationSystem
        );

        // Bake.
        $plugCode .= call_user_func(
            array(
             self::getPlugHandler($xmlNode->tagName),
             'bake',
            ),
            $xmlNode
        );

        $plugCode .= "\n} catch (ChannelPlugException \$e) {\n";
        // If it's not fatal, we have to catch and ignore any exceptions.
        if ($isFatal === FALSE) {
            $plugCode .= "Channels::HandleError(\$e);\n";
        } else {
            $plugCode .= "// Fatal Plug. Throw a new exception.\n";
            $plugCode .= ChannelsDebugger::debugPrintEndPlugCode();

            $plugCode .= 'throw new ChannelException($e->getMessage()';
            $plugCode .= ");\n";
        }

        $plugCode .= "}\n";

        if (Channels::$channelsMode === 'testing') {
            $plugCode .= "}\n";
            if ($xmlNode->tagName === 'blocker') {
                $plugCode .= " else { \$retval = FALSE;}\n";
            }
        }

        $plugCode .= ChannelsDebugger::debugPrintEndPlugCode();

        $plugCode = ChannelsBaker::indent($plugCode, 4);
        return $plugCode;

    }//end bake()


    /**
     * Returns plug infomation.
     *
     * @param string $plugSystem Name of the system the plug belong to.
     * @param object $plugXml    DomElement that represents a plug.
     *
     * @return string
     */
    public static function getTestingPlugIfCode($plugSystem, $plugXml)
    {
        $plugInfo = self::getPlugInfo($plugXml);
        $plugCode = 'if (Channels::runPlug(\''.$plugSystem.'::'.$plugInfo['method'].'::'.$plugInfo['event'].'\') === TRUE) {'."\n";
        return $plugCode;

    }//end getTestingPlugIfCode()


    /**
     * Returns plug infomation.
     *
     * @param DOMElement $plugElement DomElement that represents a plug.
     * @param boolean    $nonBaked    If TRUE, non-baked plugs are included.
     *
     * @return string
     */
    public static function getPlugInfo(DOMElement $plugElement, $nonBaked=FALSE)
    {
        $info         = array();
        $info['type'] = $plugElement->tagName;
        $plugInfo     = call_user_func(
            array(
             self::getPlugHandler($info['type']),
             'xmlToArray',
            ),
            $plugElement
        );

        return $plugInfo;

    }//end getPlugInfo()


    /**
     * Returns the name of Plug Handler for callback function.
     *
     * @param string $type Name of the plug type.
     *
     * @return string
     */
    public static function getPlugHandler($type)
    {
        $handler = '';
        switch ($type) {
            case 'switcher':
                $handler = 'ChannelsSwitcherParser';
            break;

            case 'modifier':
                $handler = 'ChannelsModifierParser';
            break;

            case 'runner':
                $handler = 'ChannelsRunnerParser';
            break;

            case 'initial_gatherer':
                $handler = 'ChannelsInitialGathererParser';
            break;

            case 'gatherer':
                $handler = 'ChannelsGathererParser';
            break;

            case 'blocker':
                $handler = 'ChannelsBlockerParser';
            break;

            case 'refiner':
                $handler = 'ChannelsRefinerParser';
            break;

            default:
                // Should throw an exception here.
            break;
        }//end switch

        return $handler;

    }//end getPlugHandler()


    /**
     * Converts the passed plug XML into array form..
     *
     * @param DOMElement $plugElement DomElement that represents a plug.
     *
     * @return array
     */
    protected static function xmlToArray(DOMElement $plugElement)
    {
        $info         = array();
        $info['type'] = $plugElement->tagName;

        $required         = $plugElement->getElementsByTagName('required');
        $info['required'] = $required->item(0)->nodeValue;

        $fatal = $plugElement->getElementsByTagName('fatal');
        if ($fatal->length !== 0) {
            $info['fatal'] = $fatal->item(0)->nodeValue;
        } else {
            $info['fatal'] = 'no';
        }

        $actions       = $plugElement->getElementsByTagName('action');
        $info['event'] = $actions->item(0)->nodeValue;

        return $info;

    }//end xmlToArray()


    /**
     * Returns list of required plug XML elements.
     *
     * Required plug xml elements are checked during baking.
     *
     * @param string $plugType If this is not NULl then only this plug type
     *                         elements are returned.
     *
     * @return boolean
     */
    public static function getRequiredElements($plugType=NULL)
    {
        $elms = NULL;
        if ($plugType === NULL) {
            return array();
        } else if (isset(self::$_requiredElementsCache[$plugType]) === TRUE) {
            return self::$_requiredElementsCache[$plugType];
        }

        $elms = call_user_func(
            array(
             self::getPlugHandler($plugType),
             'getPlugElements',
            )
        );

        self::$_requiredElementsCache[$plugType] = $elms;

        return $elms;

    }//end getRequiredElements()


    /**
     * Returns an array of the valid plug types for the channels system.
     *
     * @return array
     */
    public static function getPlugTypes()
    {
        $types = array(
                  'runner',
                  'blocker',
                  'gatherer',
                  'modifier',
                  'refiner',
                  'switcher',
                  'initial_gatherer',
                 );

        return $types;

    }//end getPlugTypes()


    /**
     * Returns plug location types.
     *
     * @return array
     */
    public static function getLocTypes()
    {
        $types = array(
                  'before',
                  'after',
                 );
        return $types;

    }//end getLocTypes()


    /**
     * Returns list of plug types.
     *
     * @param string $location Location type (i.e. before/after).
     *
     * Returned array is index by listen type (beofre/after).
     *
     * @return array
     */
    public static function getPlugTypeLocs($location=NULL)
    {
        $types = array();
        if ($location !== NULL) {
            if (isset(self::$_typeLocs[$location]) === TRUE) {
                $types = self::$_typeLocs[$location];
            }
        } else {
            $types = self::$_typeLocs;
        }

        return $types;

    }//end getPlugTypeLocs()


    /**
     * Based on weightings, finds the appropriate position for the new plug.
     *
     * Should be called from addPlugXml() only, otherwise conflicts with
     * the DomDocuments could arise.
     *
     * @param DomElement $plugNode   The new plug to be inserted.
     * @param DomElement $parent     The parent node that the $plugNode will be
     *                               inserted underneath.
     * @param array      $parameters An array of names of parameters that the
     *                               plug method requires.
     *
     * @return DomElement
     */
    public static function getPlugPosition(
        DomElement $plugNode,
        DomElement $parent,
        array $parameters
    ) {
        $plugWeight  = self::getPlugWeight($plugNode->tagName);
        $nodeAfter   = NULL;
        $sameWeights = array();

        foreach ($parent->childNodes as $currentPlug) {
            $currentWeight = self::getPlugWeight($currentPlug->tagName);
            // Will need to sort if weights are equal.
            if ($currentWeight === $plugWeight) {
                $sameWeights[] = $currentPlug;
            }

            if ($currentWeight > $plugWeight) {
                $nodeAfter = $currentPlug;
                break;
            }
        }

        /*
            Sort out the plugs with the same weightings. They could be the same
            type of plug, or a switcher and a modifier.
        */

        if (count($sameWeights) > 0) {
            if ($plugWeight === 1) {
                $tagName = $plugNode->tagName;
                if ($tagName === 'switcher') {
                    // We need to determine if there is a modifier that should
                    // be placed before this switcher.
                    foreach ($sameWeights as $same) {
                        if ($same->tagName === 'modifier') {
                            $params = $same->getElementsByTagName('parameter');
                            foreach ($params as $newParam) {
                                $paramName = $newParam->nodeValue;
                                $paramUsed = in_array($paramName, $parameters);
                                if ($paramUsed === TRUE) {
                                    // The modifier changes the switchers
                                    // variable, so it needs to go first.
                                    $nodeAfter = $currentPlug->nextSibling;
                                }
                            }
                        }
                    }
                } else {
                    foreach ($sameWeights as $same) {
                        if ($same->tagName === 'switcher') {
                            $params = $same->getElementsByTagName('parameter');
                            foreach ($params as $newParam) {
                                $paramName = $newParam->nodeValue;
                                $paramUsed = in_array($paramName, $parameters);
                                if ($paramUsed === TRUE) {
                                    // The switcher doesn't use the modifiers
                                    // data, so it goes first.
                                    $nodeAfter = $same;
                                }
                            }
                        }
                    }
                }//end if
            }//end if
        }//end if

        return $nodeAfter;

    }//end getPlugPosition()


    /**
     * Returns an appropriate weight for the plug name passed in.
     *
     * This value is used to determine plug precedence in a channel.
     *
     * @param string $plugName The name/type of the plug to check.
     *
     * @return integer
     */
    public static function getPlugWeight($plugName)
    {
        $plugWeight = call_user_func(
            array(
             self::getPlugHandler($plugName),
             'getPlugWeight',
            ),
            $plugName
        );
        return $plugWeight;

    }//end getPlugWeight()


    /**
     * Helper function to retrieve the system a plug was defined in.
     *
     * @param DomElement $xmlNode XML Node of the plug to retrieve the system
     *                            for.
     *
     * @return string
     */
    protected static function getPlugSystem(DomElement $xmlNode)
    {
        $methodNode = $xmlNode->getElementsByTagName('method')->item(0);
        $plugSystem = $methodNode->getAttribute('system');

        return $plugSystem;

    }//end getPlugSystem()


    /**
     * Returns the right name of the system to plug in.
     *
     * @param string $plugSystem The name of the system specified in XML.
     *
     * @return string
     * @throws Exception If plug exists when system isnt an exclusive interface.
     */
    protected static function findPlugImplementor($plugSystem)
    {
        $systemDoc = Channels::getSystemXml();
        $systems   = $systemDoc->getElementsByTagName('system');
        $system    = NULL;
        foreach ($systems as $system) {
            if ($system->getAttribute('name') === $plugSystem) {
                break;
            }
        }

        $sysType = array();
        $types   = array(
                    'concrete',
                    'interface',
                    'abstract',
                   );
        foreach ($types as $type) {
            if ($system->getAttribute($type) === 'true') {
                $sysType['is'.ucwords($type)] = TRUE;
            } else {
                $sysType['is'.ucwords($type)] = FALSE;
            }
        }

        if ($sysType['isConcrete'] === TRUE) {
            $plugSystem = $system->getAttribute('implements');
        } else if ($sysType['isInterface'] === TRUE
            || $sysType['isAbstract'] === TRUE
        ) {
            if ($system->getAttribute('exclusive') === 'true') {
                $sysType['isExclusive'] = TRUE;
            } else {
                $sysType['isExclusive'] = FALSE;
            }

            if ($sysType['isExclusive'] === FALSE) {
                if ($sysType['isAbstract'] === TRUE) {
                    $plugSystem = $system->getAttribute('implements');
                } else {
                    $errMsg  = $plugSystem;
                    $errMsg .= ' is not an exclusive Interface system.';
                    $errMsg .= ' You can not create plug from this system.';
                    throw new Exception($errMsg);
                }
            }
        }//end if

        return $plugSystem;

    }//end findPlugImplementor()


    /**
     * Helper function to retrieve the method call that a plug requires.
     *
     * @param DomElement $xmlNode XML Node of the plug to retrieve the method
     *                            call for.
     *
     * @return string
     */
    protected static function getPlugMethodSignature(DomElement $xmlNode)
    {
        $methodNode = $xmlNode->getElementsByTagName('method')->item(0);

        $plugSystem = self::getPlugSystem($xmlNode);
        $plugSystem = self::findPlugImplementor($plugSystem);

        $methodCall = $plugSystem.'::'.$methodNode->nodeValue;

        $methodParams = '(';

        $params = self::getPlugMethodParameters($xmlNode);

        if (empty($params) === FALSE) {
            $methodParams .= '$'.implode(', $', $params);
        }

        $methodParams .= ')';
        $methodCall   .= $methodParams;

        return $methodCall;

    }//end getPlugMethodSignature()


    /**
     * Returns parameters of a plug given its xmlNode.
     *
     * @param DomElement $xmlNode The plug node.
     *
     * @return array
     * @throws ChannelPlugException If parameters tag includes more parameters
     *                              than actual function signature.
     */
    protected static function getPlugMethodParameters(DomElement $xmlNode)
    {
        $methodNode = $xmlNode->getElementsByTagName('method')->item(0);
        $methodName = $methodNode->nodeValue;

        $childParams = array();

        // Check whether parameters tag exists in XML definition. If so,
        // get the list of parameters.
        $parameters = $xmlNode->getElementsByTagName('parameters')->item(0);

        $parametersExists = FALSE;
        if ($parameters !== NULL) {
            $parametersExists = TRUE;
            $childNodes       = $parameters->getElementsByTagName('parameter');
            foreach ($childNodes as $childNode) {
                $childParams[] = $childNode->nodeValue;
            }
        }

        $plugSystem = self::getPlugSystem($xmlNode);
        $implementorPlugSystem = self::findPlugImplementor($plugSystem);
        if ($plugSystem === $implementorPlugSystem) {
            $plugSystem = $implementorPlugSystem;
        }

        $ref        = new ReflectionClass($plugSystem.'Actions');
        $method     = $ref->getMethod($methodName);
        $parameters = $method->getParameters();
        $params     = array();
        foreach ($parameters as $parameter) {
            $params[] = str_replace('$', '', $parameter->getName());
        }

        if ($parametersExists === FALSE) {
            return $params;
        } else {
            $paramsCount      = count($params);
            $childParamsCount = count($childParams);
            if ($paramsCount === $childParamsCount
                || $paramsCount > $childParamsCount
            ) {
                return $childParams;
            } else if ($paramsCount < $childParamsCount) {
                if ($xmlNode->tagName === 'modifier'
                    || $xmlNode->tagName === 'blocker'
                ) {
                    $errMsg  = '<parameters> tag in '.$methodName.' in ';
                    $errMsg .= $plugSystem.' system includes more parameters ';
                    $errMsg .= 'than actual function signatures.';
                    throw new ChannelPlugException($errMsg);
                } else {
                    return $childParams;
                }
            }
        }//end if

        return $params;

    }//end getPlugMethodParameters()


    /**
     * Returns TRUE if the given plug XML is valid.
     *
     * In case of invalid plug definition, it throws an exception.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    public static function validate(DOMElement $plug, $system)
    {
        $plugTypes   = self::getPlugTypes();
        $mainChannel = 'MySource';

        if (in_array($plug->tagName, $plugTypes) === FALSE) {
            // Check that this is a valid plug type.
            $msg = 'Invalid plug type found "'.$plug->tagName.'"';
            throw new Exception($msg);
        }

        $requiredElements = self::getRequiredElements($plug->tagName);

        foreach ($requiredElements as $element => $required) {
            if ($required !== TRUE) {
                continue;
            }

            $tags = $plug->getElementsByTagName($element);
            if ($tags->length === 0) {
                $msg = 'Element "'.$element.'" not found for '.$plug->tagName;
                throw new Exception($msg);
            }
        }

        // Check required and fatal tags.
        $requiredTags     = $plug->getElementsByTagName('required');
        $requiredTagValue = $requiredTags->item(0)->nodeValue;
        if ($requiredTagValue !== 'no' && $requiredTagValue !== 'yes') {
            $msg = 'Invalid value for "required" tag. Found:'.$requiredTagValue;
            throw new Exception($msg);
        }

        $isValid = call_user_func(
            array(
             self::getPlugHandler($plug->tagName),
             'validate',
            ),
            $plug,
            $system
        );

        return $isValid;

    }//end validate()


    /**
     * Returns TRUE if the required tag is valid.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    protected static function validateRequired(DOMElement $plug, $system)
    {
        // Check required and fatal tags.
        $requiredTags     = $plug->getElementsByTagName('required');
        $requiredTagValue = $requiredTags->item(0)->nodeValue;
        if ($requiredTagValue !== 'no' && $requiredTagValue !== 'yes') {
            $msg = 'Invalid value for "required" tag. Found:'.$requiredTagValue;
            throw new Exception($msg);
        }

        return $requiredTagValue;

    }//end validateRequired()


    /**
     * Returns TRUE if the given plug XML has a valid fatal tag and value.
     *
     * This function is called by each plug parser classes. Also note that
     * blocker does not call this.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    protected static function validateFatal(DOMElement $plug, $system)
    {
        $fatalValue = NULL;
        $fatalTags  = $plug->getElementsByTagName('fatal');
        if ($fatalTags->length > 0) {
            $fatalValue = $fatalTags->item(0)->nodeValue;
            if ($fatalValue !== 'no' && $fatalValue !== 'yes') {
                $msg = 'Invalid value for "fatal" tag. Found: '.$fatalValue;
                throw new Exception($msg);
            }
        } else {
            $fatalValue = 'no';
        }

        return $fatalValue;

    }//end validateFatal()


    /**
     * Returns TRUE if the given plug XML has a valid fatal tag and value.
     *
     * This function is called by each plug parser classes. Also note that
     * blocker does not call this.
     *
     * @param DOMElement $plug             DomElement that represents a plug.
     * @param string     $system           Name of the system.
     * @param string     $requiredTagValue Value of required tag.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    protected static function validateEventSystem(
        DOMElement $plug,
        $system,
        $requiredTagValue
    ) {
        $event       = $plug->getElementsByTagName('action')->item(0);
        $eventSystem = ChannelsBaker::getEventSystem($event->nodeValue);
        if ($eventSystem !== 'MySource') {
            if (ChannelsBaker::systemExists($eventSystem) === FALSE) {
                if ($requiredTagValue === 'yes') {
                    $msg  = 'System '.$eventSystem.' is required by ';
                    $msg .= $system.', but not found in Systems dir.';
                    throw new Exception($msg);
                } else {
                    // Event system does not exists and not required, skip.
                    return FALSE;
                }
            }
        }

        if (ChannelsBaker::validateSystem($system) === FALSE) {
            $msg = 'Plugs file for "'.$system.'" not found!';
            throw new Exception($msg);
        }

        return TRUE;

    }//end validateEventSystem()


    /**
     * Returns TRUE if the given plug XML has a valid fatal tag and value.
     *
     * This function is called by each plug parser classes. Also note that
     * blocker does not call this.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    protected static function validateMethodParameter(DOMElement $plug, $system)
    {
        $methodTags = $plug->getElementsByTagName('method');
        $methodName = $methodTags->item(0)->nodeValue;
        if (self::plugMethodExists($methodName, $system) === FALSE) {
            $msg  = 'Invalid method for plug "'.$plug->tagName;
            $msg .= '". Method "'.$methodName;
            $msg .= '" not found in class "'.$system.'Plugs".';
            throw new Exception($msg);
        }

        $paramsNode = $plug->getElementsByTagName('parameters');
        if ($paramsNode->length === 0) {
            if ($plug->tagName !== 'modifier' && $plug->tagName !== 'blocker') {
                $msg  = 'Missing \'parameters\' tag for ';
                $msg .= $plug->tagName.' plug in '.$system.' system';
                throw new Exception($msg);
            }
        }

        return TRUE;

    }//end validateMethodParameter()


    /**
     * Returns TRUE if the given plug XML has a valid fatal tag and value.
     *
     * This function is called by each plug parser classes. Also note that
     * blocker does not call this.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    protected static function validateActionTags(DOMElement $plug, $system)
    {
        $listen = NULL;

        $actionTags = $plug->getElementsByTagName('action');
        $actionTag  = $actionTags->item(0);
        $actionInfo = ChannelsBaker::getEventSystem($actionTag->nodeValue);

        // Check for the listen attribute.
        $listen = $actionTag->getAttribute('listen');

        if ($actionInfo === 'MySource') {
            $channelName = ChannelsBaker::getEventAction($actionTag->nodeValue);

            if (empty($listen) === TRUE && $plug->tagName !== 'refiner') {
                $after = FALSE;
            } else if ($listen === 'before') {
                $after = FALSE;
            } else {
                $after = TRUE;
            }

            if (self::isValidMySourcePlug($channelName, $plug->tagName, $after) === FALSE) {
                $msg = 'Plugs of type \''.$plug->tagName.'\' not allowed ';
                if ($after === TRUE) {
                    $location = 'after';
                } else {
                    $location = 'before';
                }

                $msg .= $location.' channel action ('.$actionTag->nodeValue.').';
                throw new Exception($msg);
            }
        }//end if

        return $actionTag;

    }//end validateActionTags()


    /**
     * Returns TRUE if the given plug XML has a valid fatal tag and value.
     *
     * This function is called by each plug parser classes. Also note that
     * blocker does not call this.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the given plug XML is not valid.
     */
    protected static function validateGatherers(DOMElement $plug, $system)
    {
        // Validate gatherers if there are any.
        $gatherers = $plug->getElementsByTagName('gatherers');
        $gatherers = $gatherers->item(0);

        if (($gatherers !== NULL) && ($gatherers->hasChildNodes() === TRUE)) {
            $listen     = NULL;
            $actionTags = $plug->getElementsByTagName('action');
            $actionTag  = $actionTags->item(0);
            $listen     = $actionTag->getAttribute('listen');

            // Gatherers cannot be in a before action plug.
            if (($listen !== NULL) && ($listen === 'before')) {
                $msg  = 'Gatherers cannot be used in plugs';
                $msg .= ' that are before action. (Found in '.$system.')';
                throw new Exception($msg);
            }

            foreach ($gatherers->childNodes as $gatherer) {
                if ($gatherer->nodeType !== XML_ELEMENT_NODE) {
                    continue;
                } else {
                    $msg = self::validate($gatherer, $system);
                    if ($msg !== TRUE) {
                        throw new Exception('Gatherer: '.$msg);
                    }
                }
            }
        }//end if

        return TRUE;

    }//end validateGatherers()


    /**
     * Validates the given method name.
     *
     * Validated by checking that SystemPlugs class contains the given method.
     *
     * @param string $methodName Method name to check.
     * @param string $systemName System that this plug belongs to.
     *
     * @return boolean
     */
    protected static function plugMethodExists($methodName, $systemName)
    {
        if (($methodName === '') || ($methodName === NULL)) {
            return FALSE;
        }

        if (($systemName === '') || ($systemName === NULL)) {
            return FALSE;
        }

        if (ChannelsBaker::validateSystem($systemName) === FALSE) {
            return FALSE;
        }

        $systemsDir = Channels::getSystemsPath($systemName);

        include_once $systemsDir.'/'.$systemName.'Actions.inc';

        $classMethods = get_class_methods($systemName.'Actions');

        if (in_array($methodName, $classMethods) === FALSE) {
            return FALSE;
        }

        return TRUE;

    }//end plugMethodExists()


    /**
     * Validates the given method name.
     *
     * Validates by checking that actions.xml has this defined.
     *
     * @param string $actionStr SystemName::method.
     *
     * @return boolean
     */
    protected static function actionMethodExists($actionStr)
    {
        $exists = ChannelsBaker::actionMethodExists($actionStr);
        return $exists;

    }//end actionMethodExists()


    /**
     * Validates specified plug for the specified MySource channel.
     *
     * @param string  $channel Name of the MySource Channel.
     * @param string  $plug    Name of the plug.
     * @param boolean $isAfter If TRUE checks to see if plug is
     *                         valid after channel action. Otherwise
     *                         check if valid before.
     *
     * @return boolean
     */
    protected static function isValidMySourcePlug(
        $channel,
        $plug,
        $isAfter=FALSE
    ) {
        $mySourceChannels = ChannelsBaker::getMySourceChannels();
        if (in_array($channel, array_keys($mySourceChannels)) === TRUE) {
            $channelInfo = $mySourceChannels[$channel];

            if ($isAfter === TRUE) {
                $location = 'after';
            } else {
                $location = 'before';
            }

            $isValid = in_array($plug, $channelInfo['allowedPlugs'][$location]);
            return $isValid;
        }

        return FALSE;

    }//end isValidMySourcePlug()


    /**
     * Returns list of reserved words.
     *
     * Reserved words are used as varaible names by the channel system.
     * During the baking process gatherer field names will be checked to make
     * sure that they are not one of these reserved words.
     *
     * @return array
     */
    protected static function getReservedWords()
    {
        return array(
                'retval',
                'params',
               );

    }//end getReservedWords()


}//end class

?>
