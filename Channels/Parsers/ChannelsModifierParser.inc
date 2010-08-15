<?php
/**
 * Parser class for modifier plug.
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
 * ChannelsModifierParser.
 *
 * Modifier plug.
 */
class ChannelsModifierParser extends ChannelsPlugParser
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

        // Make sure params are not queries.
        $plugParams = self::getPlugMethodParameters($xmlNode);
        foreach ($plugParams as $param) {
            $plugContent .= "\n".'if (is_array($'.$param.') === TRUE) {'."\n\t";
            $plugContent .= 'if (isset($'.$param.'[\'DAL_QUERY\']) === TRUE) {';
            $plugContent .= "\n\t\t".'$'.$param.' = DAL::executeQueryArray($'.$param.'[\'DAL_QUERY\']);';
            $plugContent .= "\n\t\t".'Channels::modifyBasket(\''.$param.'\', $'.$param.');';
            $plugContent .= "\n\t}\n}";
        }

        $parameters = $xmlNode->getElementsByTagName('modify-parameters');

        if ($parameters->length === 1) {
            $methodCall = self::getPlugMethodSignature($xmlNode);
            $params     = self::getPlugMethodParameters($xmlNode);

            $childNodes = $parameters->item(0)->getElementsByTagName('parameter');
            if ($childNodes->length === 1) {
                $plugContent .= "\n".'$'.$childNodes->item(0)->nodeValue.' = '.$methodCall.";\n";
                $plugContent .= 'Channels::modifyBasket(\''.$childNodes->item(0)->nodeValue.'\', $'.$childNodes->item(0)->nodeValue.');'."\n";
            } else if ($childNodes->length > 1) {
                $plugContent .= "\n\$_modifierReturnValue = ".$methodCall.";\n";
                foreach ($childNodes as $param) {
                    $plugContent .= "\nif (array_key_exists('".$param->nodeValue.'\', $_modifierReturnValue) === TRUE) {'."\n\t";
                    $plugContent .= 'Channels::modifyBasket(\''.$param->nodeValue.'\', $_modifierReturnValue[\''.$param->nodeValue.'\']);'."\n";
                    $plugContent .= "\t\$".$param->nodeValue.' = $_modifierReturnValue[\''.$param->nodeValue.'\'];';
                    $plugContent .= "\n}\n";
                }
            }//end if
        }//end if

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
        $weight = 3;
        return $weight;

    }//end getPlugWeight()


}//end class

?>