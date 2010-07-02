<?php
/**
 * Actions for the GUI Screen Switcher Widget System.
 *
 * @version    4.0.0
 * @package    MySource4
 * @subpackage Button
 * @author     Squiz Pty Ltd <mysource4@squiz.net>
 * @copyright  2006-2007 Squiz Pty Ltd (ABN 77 084 670 600)
 * @license    http://matrix.squiz.net/licence Squiz.Net Open Source Licence
 */

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Screen Switcher Widget Actions Class.
 *
 * @since 4.0.0
 */
class GUIScreenSwitcherActions
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
        $content  = '<div id="%id%" class="%class%">';

        $content .= '<div class="%class%-left">';
        $content .= '</div>';

        $firstScreen = TRUE;
        foreach ($settings['screens'] as $screen) {

            if (strtolower($screen['disabled']) === 'true') {
                $content .= '<div class="%class%-button disabled">';
            } else {
                // Enable the first screen by default.
                if ($firstScreen === TRUE) {
                    $content .= '<div class="%class%-button selected">';
                    $firstScreen === FALSE;
                } else {
                    $content .= '<div class="%class%-button inactive">';
                }//end if first screen
            }//end if screen disabled

            // Work out the name of the template and web icon we expect from the
            // system we are referencing.
            $screenBaseName  = ucwords($screen['id']).'Screen';

            $path  = '/__web/'.GUI::getSystemWebPath($screen['system']).'/';
            $path .= $screenBaseName.'.png';

            // Screen icon.
            $content .= '<div class="%class%-icon"';
            $content .= ' style="background-image: url('.$path.')"';
            $content .= '>';
            $content .= '</div>';

            // Name of screen.
            $content .= '<div class="%class%-text">';
            $content .= $screen['label'];
            $content .= '</div>';

            // Selected arrow - shown only when active.
            $content .= '<div class="%class%-arrow">';
            $content .= '</div>';

            // Divider line on right hand side of block.
            $content .= '<div class="%class%-divider">';
            $content .= '</div>';

            // No access icon - shown only when disabled.
            $content .= '<div class="%class%-no-entry">';
            $content .= '</div>';

            $content .= '</div>';

            $firstScreen = FALSE;
        }//end foreach screen

        $content .= '<div class="%class%-right">';
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
