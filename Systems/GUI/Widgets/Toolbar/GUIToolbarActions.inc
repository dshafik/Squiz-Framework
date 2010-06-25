<?php
/**
 * Actions for the Toolbar System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Toolbar
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * Toolbar Actions Class.
 *
 * @since 4.0.0
 */
class GUIToolbarActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getTemplateContent(array $settings=array())
    {
        $content = '<div class="Toolbar">
                        <div class="logo" />
                        <div class="leftList">
                            <widget type="GUI/Toolbar/List" id="ToolbarLeftList" />
                        </div>
                        <div class="rightList">
                            <widget type="GUI/Toolbar/List" id="ToolbarRightList" />
                        </div>
                    </div>';
        return $content;

    }//end getTemplateContent()


    /**
     * Prepare for baking.
     *
     * @param DOMNode $htmlTpl The HTML content of the widget.
     *
     * @since  4.0.0
     * @return DOMNode
     */
    public static function prepareBake(DOMNode $htmlTpl)
    {
        return $htmlTpl;

    }//end prepareBake()


}//end class

?>