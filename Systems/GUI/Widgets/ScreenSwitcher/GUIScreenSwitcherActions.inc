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

require_once 'Systems/GUI/Widgets/ContentSwitcher/GUIContentSwitcherActions.inc';

/**
 * GUI Screen Switcher Widget Actions Class.
 *
 * @since 4.0.0
 */
class GUIScreenSwitcherActions extends GUIContentSwitcherActions
{


    /**
     * Returns the contents of the switcher buttons.
     *
     * @param array $settings The settings of the widget.
     *
     * @since  4.0.0
     * @return string
     */
    public static function getButtonsContent(array $settings=array())
    {
        Channels::includeSystem('GUI');

        $content  = '<div id="%id%" class="%class%">';
        $content .= '<div class="%class%-left">';
        $content .= '</div>';

        $content .= '<div class="%class%-right">';
        $content .= '</div>';

        $screens = array();
        if (isset($settings['items']) === TRUE) {
            $screens = $settings['items'];
        } else if (isset($settings['screens']) === TRUE) {
            $screens = $settings['screens'];
        }

        foreach ($screens as $screen) {
            $screenid = $settings['id'].'-'.$screen['id'];
            if (strtolower($screen['disabled']) === 'true') {
                $content .= '<div id="'.$screenid.'" class="%class%-button disabled">';
            } else {
                $content .= '<div id="'.$screenid.'" class="%class%-button inactive">';
            }//end if screen disabled

            // Work out the name of the template and web icon we expect from the
            // system we are referencing.
            $screenBaseName = preg_replace('/[^A-Za-z0-9]/', ' ', $screen['id']);
            $screenBaseName = ucwords($screenBaseName);
            $screenBaseName = str_replace(' ', '', $screenBaseName);

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
        }//end foreach

        $content .= '</div>';

        return $content;

    }//end getButtonsContent()


}//end class

?>
