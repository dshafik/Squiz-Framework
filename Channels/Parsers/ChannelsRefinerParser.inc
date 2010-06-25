<?php
/**
 * Parser class for refiner plug.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Channels
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Channels/Parsers/ChannelsPlugParser.inc';

/**
 * ChannelsRefinerParser.
 *
 * Refiner plug.
 *
 * @since 4.0.0
 */
class ChannelsRefinerParser extends ChannelsPlugParser
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
     * @since  4.0.0
     * @return string
     */
    public static function bake(DOMElement $xmlNode)
    {
        $plugContent = '';

        // Make sure params are not queries.
        $plugParams = self::getPlugMethodParameters($xmlNode);
        foreach ($plugParams as $param) {
            $plugContent .= 'if (is_array($'.$param.') === TRUE) {'."\n\t";
            $plugContent .= 'if (isset($'.$param.'[\'DAL_QUERY\']) === TRUE) {';
            $plugContent .= "\n\t".'$'.$param.' = DAL::executeQueryArray($';
            $plugContent .= $param.'[\'DAL_QUERY\']);';
            $plugContent .= "\t\t}\n}";
        }

        $methodCall   = self::getPlugMethodSignature($xmlNode);
        $plugContent .= '$retval = '.$methodCall.";\n";
        $plugContent .= "Channels::modifyBasket('retval', \$retval);\n";
        // Add the plug's return value to debug values.
        include_once 'Channels/Debug/ChannelsDebugger.inc';
        $plugContent .= ChannelsDebugger::debugAddPlugReturnValue('retval');

        return $plugContent;

    }//end bake()


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

        $methodEl = $plugElement->getElementsByTagName('method');

        $info['system'] = $methodEl->item(0)->getAttribute('system');
        $info['method'] = $methodEl->item(0)->nodeValue;
        $info['loc']    = 'after';

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
                 'fatal'    => TRUE,
                 'gatherer' => FALSE,
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
        $weight = 4;
        return $weight;

    }//end getPlugWeight()


}//end class

?>