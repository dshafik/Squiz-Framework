<?php
/**
 * Parser class for gatherer plug.
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
 * ChannelsGathererParser.
 *
 * Gatherer plug.
 *
 */
class ChannelsGathererParser extends ChannelsPlugParser
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
        $isValid          = self::validateMethodParameter($plug, $system);
        $actionTag        = self::validateActionTags($plug, $system);

        $fieldTags     = $plug->getElementsByTagName('field');
        $reservedWords = self::getReservedWords();
        $fieldValue    = $fieldTags->item(0)->nodeValue;
        if (in_array($fieldValue, $reservedWords) === TRUE) {
            $msg  = 'Gatherer field cannot use the reserved word "';
            $msg .= $msg.$fieldValue.'"';
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
        $plugContent  = '';
        $methodCall   = self::getPlugMethodSignature($xmlNode);
        $field        = $xmlNode->getElementsByTagName('field')->item(0);
        $fieldName    = $field->nodeValue;
        $plugContent .= '$'.$fieldName.' = '.$methodCall.";\n";
        $plugContent .= 'Channels::addToBasket(\''.$fieldName.'\', $'.$fieldName.", TRUE);\n";
        // Add the plug's return value to debug values.
        include_once 'Channels/Debug/ChannelsDebugger.inc';
        $plugContent .= ChannelsDebugger::debugAddPlugReturnValue($fieldName);

        return $plugContent;

    }//end bake()


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
        $fieldVal       = $plugElement->getElementsByTagName('field');
        $info['field']  = $fieldVal->item(0)->nodeValue;
        $info['loc']    = 'before';
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
                 'field'    => TRUE,
                 'fatal'    => FALSE,
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
        $weight = 2;
        return $weight;

    }//end getPlugWeight()


}//end class

?>