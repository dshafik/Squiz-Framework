<?php
/**
 * Actions for the GUI InfoBox System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage GUIInfoBox
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI InfoBox Actions Class.
 *
 * @since 4.0.0
 */
class GUIInfoBoxActions
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
        $content = '<div id="%id%" class="%class%">';

        // Top section.
        $content .= '<div class="%class%-top">';
        $content .= '<div class="%class%-title">';
        $content .= '<div class="%class%-title-icon"></div>';
        $content .= '<div class="%class%-title-value">'.$settings['title'].'</div>';
        $content .= '</div>';
        $content .= '</div>';

        // Mid section.
        $content .= '<div class="%class%-mid">';
        $content .= '<div class="%class%-thumbContainer">';
        $content .= '<img src="'.$settings['thumbURL'].'" />';
        $content .= '</div>';
        $content .= '</div>';

        // Bottom section.
        $content .= '<div class="%class%-bottom">';
        $content .= $settings['content'];
        $content .= '</div>';

        $content .= '</div>';

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
