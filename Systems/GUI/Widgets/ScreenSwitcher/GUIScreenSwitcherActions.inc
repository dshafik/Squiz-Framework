<?php
/**
 * Actions for the GUI Screen Switcher Widget System.
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
 * @subpackage GUI
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/GUI/Widgets/ContentSwitcher/GUIContentSwitcherActions.inc';

/**
 * GUI Screen Switcher Widget Actions Class.
 *
 */
class GUIScreenSwitcherActions extends GUIContentSwitcherActions
{


    /**
     * Returns the contents of the switcher buttons.
     *
     * @param array $settings The settings of the widget.
     *
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
