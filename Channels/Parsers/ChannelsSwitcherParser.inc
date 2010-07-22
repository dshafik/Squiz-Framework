<?php
/**
 * Parser class for switcher plug.
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
 * ChannelsSwitcherParser.
 *
 * Switcher plug.
 *
 * @since 4.0.0
 */
class ChannelsSwitcherParser extends ChannelsPlugParser
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

        // Check switcher_event if this is a switcher.
        $switcherElement = $plug->getElementsByTagName('switch_event');
        $nodeValue       = $switcherElement->item(0)->nodeValue;
        if (self::actionMethodExists($nodeValue) === FALSE) {
            $msg = 'Switcher Action "'.$nodeValue.'" not found.';
            throw new Exception($msg);
        }

        $actionTag = self::validateActionTags($plug, $system);

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
     * @throws Exception If switch action is not found in
     *                   switch system.
     */
    public static function bake(DOMElement $xmlNode)
    {
        include_once 'Channels/ChannelsBaker.inc';
        $plugContent      = '';
        $methodCall       = self::getPlugMethodSignature($xmlNode);
        $methodNode       = $xmlNode->getElementsByTagName('method')->item(0);
        $methodAttributes = ChannelsBaker::getElementAttributes($methodNode);
        $switchMethod     = $xmlNode->getElementsByTagName('switch_event');
        $switchMethod     = $switchMethod->item(0)->nodeValue;
        $switchSplit      = strpos($switchMethod, '::');
        $switchSystem     = substr($switchMethod, 0, $switchSplit);
        $switchAction     = substr($switchMethod, ($switchSplit + 2));

        // Include the switch methods' system action file, so we can
        // Use reflection on its method.
        Channels::requireSystemActions($switchSystem);

        // Get the parameters for the switch method, and write out
        // out a string from them.
        $actionClass = $switchSystem.'Actions';
        // Retrieve the actions from the switcher's system and in turn retrieve
        // the parameters to pass to the switch method.
        $switchSystemActions = ChannelsBaker::actionMethodExists(
            $switchSystem.'::'.$switchAction,
            TRUE
        );

        if ($switchSystemActions === FALSE) {
            $msg = $switchAction.' not found in "'.$switchSystem.'"';
            throw new Exception($msg);
        }

        $switchActionParams = $switchSystemActions[$switchAction]['params'];
        $switchString       = '';
        if (empty($switchActionParams) === FALSE) {
            $switchParams = array();
            foreach ($switchActionParams as $paramData) {
                $switchParams[] = $paramData['name'];
            }

            $switchString = '$'.implode(', $', $switchParams);
        }

        // Output an "IF" condition that will represent the switch
        // plug in PHP code.
        $negatedAttributeContents = Util::getArrayIndex(
            $methodAttributes,
            'negated',
            'false'
        );

        if (isset($negatedAttributeContents[0]) === FALSE
            || strtolower($negatedAttributeContents) === 'true'
        ) {
            $comparisonValue = 'FALSE';
        } else {
            $comparisonValue = 'TRUE';
        }

        $plugContent .= 'if ('.$methodCall.' === '.$comparisonValue.') {'."\n";

        // Method arguments might not be set, if not then set them to NULL.
        if (empty($switchParams) === FALSE) {
            foreach ($switchParams as $param) {
                $plugContent .= 'if (isset($'.$param.') === FALSE) {'."\n\t";
                $plugContent .= '$'.$param.' = NULL;';
                $plugContent .= "}\n";
            }
        }

        if (ChannelsBaker::$addReportCode === TRUE) {
            $plugContent .= 'DebugExecutionPath::end();'."\n";
        }

        $plugContent .= 'Channels::includeSystem(\''.$switchSystem."');\n";
        $plugContent .= 'return '.$switchMethod.'('.$switchString.");\n";
        $plugContent .= '}';

        return $plugContent;

    }//end bake()


    /**
     * Returns plug infomation.
     *
     * @param string $plugSystem Name of the system the plug belong to.
     * @param object $plugXml    DomElement that represents a plug.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTestingPlugIfCode($plugSystem, $plugXml)
    {
        $plugInfo = self::getPlugInfo($plugXml);
        $plugCode = 'if (Channels::runPlug(\''.$plugSystem.'::'.$plugInfo['method'].'::'.$plugInfo['switch_event'].'::'.$plugInfo['event'].'\') === TRUE) {'."\n";
        return $plugCode;

    }//end getTestingPlugIfCode()


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

        $switchEvent = $plugElement->getElementsByTagName('switch_event');
        $info['switch_event'] = $switchEvent->item(0)->nodeValue;
        $info['loc']          = 'before';

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
                 'required'     => TRUE,
                 'method'       => TRUE,
                 'switch_event' => TRUE,
                 'action'       => TRUE,
                 'fatal'        => FALSE,
                 'gatherer'     => FALSE,
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
        $weight = 1;
        return $weight;

    }//end getPlugWeight()


}//end class

?>