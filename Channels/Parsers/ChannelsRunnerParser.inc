<?php
/**
 * Parser class for runner plug.
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

require_once 'Channels/Parsers/ChannelsPlugParser.inc';

/**
 * ChannelsRunnerParser.
 *
 * Runner plug.
 *
 */
class ChannelsRunnerParser extends ChannelsPlugParser
{


    /**
     * Returns TRUE is the given plug DOM is valid.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @return boolean
     * @throws Exception If the plug is not valid.
     */
    public static function validate(DOMElement $plug, $system)
    {
        $requiredTagValue = self::validateRequired($plug, $system);
        $fatalValue       = self::validateFatal($plug, $system);
        $isValid = self::validateEventSystem($plug, $system, $requiredTagValue);
        $isValid = self::validateMethodParameter($plug, $system);
        $actionTag        = self::validateActionTags($plug, $system);

        if (self::actionMethodExists($actionTag->nodeValue) === FALSE) {
            $msg = 'Action "'.$actionTag->nodeValue.'" not found.';
            throw new Exception($msg);
        }

        $listen = $actionTag->getAttribute('listen');
        if ($listen === '') {
            $msg = 'Runner action tag is missing "listen" attribute.';
            throw new Exception($msg);
        }

        $listenTypes = self::getLocTypes();
        if (in_array($listen, $listenTypes) === FALSE) {
            $msg = 'Invalid "listen" attribute value. Found: '.$listen;
            throw new Exception($msg);
        }

        // Main channel actions cannot be fatal.
        include_once 'Channels/ChannelsBaker.inc';
        $actionInfo = ChannelsBaker::getEventSystem($actionTag->nodeValue);
        if (($fatalValue === 'yes') && ($actionInfo === 'MySource')) {
            $msg = 'Main channel plugs cannot be fatal.';
            throw new Exception($msg);
        }

        $result = self::validateGatherers($plug, $system);
        return $result;

    }//end validate()


    /**
     * Constructs an baked code for plug from DomElement.
     *
     * @param DOMElement $xmlNode DomElement that represents a single query.
     *
     * @return string
     */
    public static function bake(DOMElement $xmlNode)
    {
        $plugContent = '';
        $methodCall  = self::getPlugMethodSignature($xmlNode);
        $plugContent = $methodCall.';';
        return $plugContent;

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
        $plugCode = 'if (Channels::runPlug(\''.$plugSystem.'::'.$plugInfo['loc'].'::'.$plugInfo['method'].'::'.$plugInfo['event'].'\') === TRUE) {'."\n";
        return $plugCode;

    }//end getTestingPlugIfCode()


    /**
     * Converts the passed plug XML into array form..
     *
     * @param DOMElement $plugElement DomElement that represents a plug.
     *
     * @return array
     */
    protected static function xmlToArray(DOMElement $plugElement)
    {
        $info = parent::xmlToArray($plugElement);

        $methodEl = $plugElement->getElementsByTagName('method');

        $info['system'] = $methodEl->item(0)->getAttribute('system');
        $info['method'] = $methodEl->item(0)->nodeValue;

        $actionTag   = $plugElement->getElementsByTagName('action')->item(0);
        $info['loc'] = $actionTag->getAttribute('listen');

        return $info;

    }//end xmlToArray()


    /**
     * Returns list of required plug XML elements.
     *
     * Required plug xml elements are checked during baking.
     *
     * @return array
     */
    protected static function getPlugElements()
    {
        $elms = array(
                 'required' => TRUE,
                 'method'   => TRUE,
                 'action'   => TRUE,
                 'fatal'    => FALSE,
                 'gatherer' => FALSE,
                );

        return $elms;

    }//end getPlugElements()


    /**
     * Returns an appropriate weight for this plug.
     *
     * @param string $plugName The name/type of the plug to check.
     *
     * @return integer
     */
    public static function getPlugWeight($plugName)
    {
        $weight = 5;
        return $weight;

    }//end getPlugWeight()


}//end class

?>