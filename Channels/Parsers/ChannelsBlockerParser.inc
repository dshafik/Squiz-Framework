<?php
/**
 * Parser class for blocker plug.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Channels
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Channels/Parsers/ChannelsPlugParser.inc';
require_once 'Channels/ChannelsBaker.inc';

/**
 * ChannelsBlockerParser.
 *
 * Blocker plug.
 *
 * @since 4.0.0
 */
class ChannelsBlockerParser extends ChannelsPlugParser
{


    /**
     * Returns TRUE is the given plug DOM is valid.
     *
     * @param DOMElement $plug   DomElement that represents a plug.
     * @param string     $system Name of the system.
     *
     * @since  4.0.0
     * @return boolean
     * @throws Exception If the plug is not valid.
     */
    public static function validate(DOMElement $plug, $system)
    {
        $requiredTagValue = self::validateRequired($plug, $system);

        $isValid = self::validateEventSystem($plug, $system, $requiredTagValue);
        $isValid = self::validateMethodParameter($plug, $system);

        $actionTag = self::validateActionTags($plug, $system);

        if (self::actionMethodExists($actionTag->nodeValue) === FALSE) {
            $msg = 'Action "'.$actionTag->nodeValue.'" not found.';
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
     * @since  4.0.0
     * @return string
     */
    public static function bake(DOMElement $xmlNode)
    {
        $codedFlowValue = self::getCodedFlowValue($xmlNode);
        $methodCall     = self::getPlugMethodSignature($xmlNode);
        $plugContent    = 'if ($retval === $flowValues[$blockerLastRun]) {';
        $plugContent   .= "\n".'   $retval = '.$methodCall.';'."\n";
        $plugContent   .= '   $blockerLastRun++;'."\n";
        $plugContent   .= '   $flowValues[$blockerLastRun] = '.$codedFlowValue;
        $plugContent   .= ";\n}\n";
        $plugContent   .= 'Channels::addToBasket(\'retval\', $retval);'."\n";

        // Add the plug's return value to debug values.
        include_once 'Channels/Debug/ChannelsDebugger.inc';
        $plugContent .= ChannelsDebugger::debugAddPlugReturnValue('retval');

        return $plugContent;

    }//end bake()


    /**
     * Gets the flow through to action value for the blocker in PHP form.
     *
     * @param DOMElement $plugElement DomElement that represents blocker plug.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getCodedFlowValue(DOMElement $plugElement)
    {
        $codedFlowValue = 'FALSE';
        $flowValNode    = $plugElement->getElementsByTagName('flowVal')->item(0);
        if ($flowValNode !== NULL) {
            $flowValAttributes = ChannelsBaker::getElementAttributes($flowValNode);
            $flowType = Util::getArrayIndex($flowValAttributes, 'type', FALSE);
            if ($flowType !== FALSE) {
                $flowType = strtolower($flowType);
                if (in_array($flowType, array('boolean', 'string')) === TRUE) {
                    $flowVal = $flowValNode->nodeValue;
                    if ($flowType === 'string') {
                        $codedFlowValue = str_replace('\\', '\\\\', $flowVal);
                        $codedFlowValue = '\''.str_replace("'", "\'", $codedFlowValue).'\'';
                    } else {
                        $codedFlowValue = $flowVal;
                    }
                }
            }
        }

        return $codedFlowValue;

    }//end getCodedFlowValue()


    /**
     * Converts the passed plug XML into array form..
     *
     * @param DOMElement $plugElement DomElement that represents a plug.
     *
     * @since  4.0.0
     * @return array
     */
    protected static function xmlToArray(DOMElement $plugElement)
    {
        $info = parent::xmlToArray($plugElement);

        $methodEl        = $plugElement->getElementsByTagName('method');
        $info['system']  = $methodEl->item(0)->getAttribute('system');
        $info['method']  = $methodEl->item(0)->nodeValue;
        $info['loc']     = 'before';
        $info['flowVal'] = self::getCodedFlowValue($plugElement);

        return $info;

    }//end xmlToArray()


    /**
     * Returns list of required plug XML elements.
     *
     * Required plug xml elements are checked during baking.
     *
     * @since  4.0.0
     * @return array
     */
    protected static function getPlugElements()
    {
        $elms = array(
                 'required' => TRUE,
                 'method'   => TRUE,
                 'action'   => TRUE,
                 'flowVal'  => FALSE,
                );

        return $elms;

    }//end getPlugElements()


    /**
     * Returns an appropriate weight for this plug.
     *
     * @param string $plugName The name/type of the plug to check.
     *
     * @since  4.0.0
     * @return integer
     */
    public static function getPlugWeight($plugName)
    {
        $weight = 6;
        return $weight;

    }//end getPlugWeight()


}//end class

?>