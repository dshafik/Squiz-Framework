<?php
/**
 * Parser class for initial gather plug.
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
 * ChannelsInitialGathererParser.
 *
 * Initial Gatherer plug.
 *
 * @since 4.0.0
 */
class ChannelsInitialGathererParser extends ChannelsPlugParser
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
     */
    public static function bake(DOMElement $xmlNode)
    {
        $contents     = '';
        $plugInfo     = self::getPlugInfo($xmlNode);
        $fieldName    = $plugInfo['field'];
        $defaultValue = $plugInfo['default'];
        $plugSystem   = explode('::', $plugInfo['event']);
        $plugSystem   = $plugSystem[0];

        $contents .= '/*'."\n";
        $contents .= '___________________________________________________'."\n";
        $contents .= 'System  '.$plugSystem.' initial_gatherer plug.'."\n";
        $contents .= '___________________________________________________'."\n";
        $contents .= '*/'."\n";

        // TODO: Need to work out what system it is, this can be different.
        $plugSystem = 'Testing';

        if (Channels::$channelsMode === 'testing') {
            $contents .= 'if (Channels::runPlug(\''.$plugSystem.'::';
            $contents .= $xmlNode->tagName.'::'.$plugInfo['event'];
            $contents .= '\') === TRUE) {'."\n";
        }

        // Code to set the initial value.
        $contents .= '$'.$fieldName.' = ';
        $contents .= '(isset($params[\''.$fieldName.'\']) === TRUE) ';
        $contents .= '? $params[\''.$fieldName.'\'] : '.$defaultValue.';'."\n";
        // Add the new data to the event basket.
        include_once 'Channels/Debug/ChannelsDebugger.inc';
        $contents .= 'Channels::addToBasket(\''.$fieldName.'\', $'.$fieldName;
        $contents .= ");\n";

        if (Channels::$channelsMode === 'testing') {
            $contents .= "}\n";
        }

        include_once 'Channels/ChannelsBaker.inc';
        $contents = ChannelsBaker::indent($contents);
        return $contents;

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

        $defaultEl       = $plugElement->getElementsByTagName('default');
        $info['default'] = $defaultEl->item(0)->nodeValue;

        $fieldVal       = $plugElement->getElementsByTagName('field');
        $info['field']  = $fieldVal->item(0)->nodeValue;
        $info['system'] = $fieldVal->item(0)->getAttribute('system');
        $info['loc']    = 'before';

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
                 'default'  => TRUE,
                 'field'    => TRUE,
                 'fatal'    => TRUE,
                 'action'   => TRUE,
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
        $weight = 0;
        return $weight;

    }//end getPlugWeight()


}//end class

?>