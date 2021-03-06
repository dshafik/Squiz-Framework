<?php
/**
 * ChannelsDebugger class.
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

require_once 'Channels/Channels.inc';

/**
 * ChannelsDebugger.
 */
class ChannelsDebugger
{


    /**
     * Print the code to signal the start of an action in the current execution.
     *
     * @param string $eventCall The call that this action was called with.
     * @param array  $method    The details of the method.
     *
     * @return string
     */
    public static function debugPrintStartActionCode($eventCall, array $method)
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "\nif (Channels::\$installing === FALSE) {\n";
            $ret .= "\$debugFunctionParams = array('params' => array());\n";
            $ret .= "\$debugFunctionParams['channel'] = '".$eventCall."';\n";
            $ret .= '$debugFunctionParams[\'system\'] = \''.$method['system']."';\n";
            $ret .= '$debugFunctionParams[\'type\'] = \''.$method['type']."';\n";
            foreach ($method['params'] as $paramName) {
                $ret .= '$debugFunctionParams[\'params\'][\'';
                $ret .= $paramName['name'].'\'] = $'.$paramName['name'].";\n";
            }

            $ret .= "Channels::includeSystem('Report');\n";
            $ret .= "Report::debugStartAction(\$debugFunctionParams);\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintStartActionCode()


    /**
     * Print the code to signify that an action has finished in execution.
     *
     * @return string
     */
    public static function debugPrintEndActionCode()
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "\nif (Channels::\$installing === FALSE) {\n";
            $ret .= "Channels::includeSystem('Report');\n";
            $ret .= "if (isset(\$retval) === FALSE) {\n\$retval='';\n}\n";
            $ret .= "Report::debugEndAction(\$retval);\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintEndActionCode()


    /**
     * Print the code to signify that a channel has started in this execution.
     *
     * @param string $methodName The name if the channel being called.
     *
     * @return string
     */
    public static function debugPrintStartChannelCode($methodName)
    {
        $ret = '';
        // Bake out debug code if this is in debug mode.
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $displayName = self::$_currentSystem;
            $ret        .= "\nif (Channels::\$installing === FALSE) {\n";
            $ret        .= "Channels::includeSystem('Report');\n";
            // Bake in the start of execution if the channel is MySource::init.
            if ((self::$_currentSystem === 'MySource') && ($methodName === 'init')) {
                $ret .= "Report::debugStartExecution();\n";
            }

            $ret .= 'Channels::includeSystem(\''.$displayName."');\n";
            $ret .= "\$debugReportArgs = func_get_args();\n";
            $ret .= "\$method = new ReflectionMethod('";
            $ret .= $displayName."', '".$methodName."');\n";
            $ret .= 'Report::debugStartChannel(\''.$displayName.'\', \'';
            $ret .= $methodName."', \$debugReportArgs, \$method);\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintStartChannelCode()


    /**
     * Print the code to signify that a channel has finished in this execution.
     *
     * @param string $system      The system that the channel belongs to.
     * @param string $channelName The name of the channel that is being closed.
     *
     * @return string
     */
    public static function debugPrintEndChannelCode($system, $channelName)
    {
        $ret = '';
        // If debug mode, notify the report system that we have finished the
        // channel.
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "\nif (Channels::\$installing === FALSE) {\n";
            $ret .= 'Report::debugEndChannel(\''.$system.'\', \'';
            $ret .= $channelName."', \$retval);\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintEndChannelCode()


    /**
     * Prints the code that signifies a plug has started in this execution.
     *
     * @param string     $plugSystem The system that the plug belongs to.
     * @param DomElement $xmlNode    The node that represents the plug's XML.
     *
     * @return string
     */
    public static function debugPrintStartPlugCode($plugSystem, DomElement $xmlNode)
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret       .= "if (Channels::\$installing === FALSE) {\n";
            $ret       .= "Channels::includeSystem('Report');\n";
            $paramsNode = $xmlNode->getElementsByTagName('parameter');
            $ret       .= "\$paramValues = array();\n";
            foreach ($paramsNode as $param) {
                $varName = $param->nodeValue;
                $ret    .= '$paramValues[\''.$varName.'\'] = $'.$varName.";\n";
            }

            // Add the plug's data to pass to the debugStartChannel() method.
            $ret .= "\$plugData = array(\n";
            $ret .= '\'plugType\' => \''.$xmlNode->tagName."',\n";
            $ret .= '\'fatal\'    => \''.$fatalValue."',\n";
            $ret .= '\'system\'   => \''.$plugSystem."',\n";
            // Extract the plug's method node.
            $methodNode = $xmlNode->getElementsByTagName('method')->item(0);

            $ret .= '\'plugMethod\' => \''.$methodNode->nodeValue."',\n";
            $ret .= '\'params\'   => $paramValues'.",\n";
            $ret .= ");\n";
            $ret .= "Report::debugStartPlug(\$plugData);\n";
            $ret .= "}\n";
        }//end if

        return $ret;

    }//end debugPrintStartPlugCode()


    /**
     * Prints code to add the return value of a plug to the current plug.
     *
     * @param string $paramName The name of the parameter that should be stored
     *                          as the return value of the plug.
     *
     * @return string
     */
    public static function debugAddPlugReturnValue($paramName)
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "if (Channels::\$installing === FALSE) {\n";
            $ret .= "Channels::includeSystem('Report');\n";
            $ret .= 'Report::debugAddPlugReturnValue($'.$paramName.");\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugAddPlugReturnValue()


    /**
     * Prints the code to end a plug in the current execution.
     *
     * @return string
     */
    public static function debugPrintEndPlugCode()
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "if (Channels::\$installing === FALSE) {\n";
            $ret .= "Channels::includeSystem('Report');\n";
            $ret .= "Report::debugEndPlug();\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintEndPlugCode()


    /**
     * Print the code to start a query in debug execution.
     *
     * @return string
     */
    public static function debugPrintStartQueryCode()
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "if (Channels::\$installing === FALSE) {\n";
            $ret .= "Channels::includeSystem('Report');\n";
            $ret .= "Report::debugStartQuery();\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintStartQueryCode()


    /**
     * Print the code to close a query in debug execution.
     *
     * @return string
     */
    public static function debugPrintEndQueryCode()
    {
        $ret = '';
        if (Channels::$channelsMode === 'debug'
            && self::$_currentSystem !== 'Report'
        ) {
            $ret .= "if (Channels::\$installing === FALSE) {\n";
            $ret .= "Channels::includeSystem('Report');\n";
            $ret .= "Report::debugEndQuery(\$retval);\n";
            $ret .= "}\n";
        }

        return $ret;

    }//end debugPrintEndQueryCode()


}//end class

?>