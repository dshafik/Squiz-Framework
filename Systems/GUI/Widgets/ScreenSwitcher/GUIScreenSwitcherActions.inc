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
 */
class GUIScreenSwitcherActions extends GUIContentSwitcherActions
{


    /**
     * Prints the contents of the switcher buttons.
     *
     * @param array $settings The settings of the widget.
     *
     * @return void
     */
    public static function printButtonsContent(array $settings)
    {
        Channels::includeSystem('GUI');

        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<div id="'.$id.'" class="'.$class.'">';
        echo '<div class="'.$class.'-left">';
        echo '</div>';

        echo '<div class="'.$class.'-right">';
        echo '</div>';

        $screens = array();
        if (isset($settings['items']) === TRUE) {
            $screens = $settings['items'];
        }

        // Populate the initial item with NULL if not set.
        $settings['initialItem'] = Util::getArrayIndex($settings, 'initialItem');

        foreach ($screens as $screen) {
            $screenid     = $id.'-'.$screen['id'];
            $extraClasses = '';
            if (strtolower(Util::getArrayIndex($screen, 'disabled')) === 'true') {
                $extraClasses .= ' disabled';
            } else if ($screen['id'] === $settings['initialItem']) {
                $extraClasses .= ' selected';
            } else {
                $extraClasses .= ' inactive';
            }

            echo '<div id="'.$screenid.'" class="'.$class.'-button'.$extraClasses.'">';

            // Work out the name of the template and web icon we expect from the
            // system we are referencing.
            $screenBaseName = preg_replace('/[^A-Za-z0-9]/', ' ', $screen['id']);
            $screenBaseName = ucwords($screenBaseName);
            $screenBaseName = str_replace(' ', '', $screenBaseName);

            $path  = '/__web/'.GUI::getSystemWebPath($screen['system']).'/';
            $path .= $screenBaseName.'.png';

            // Screen icon.
            echo '<div class="'.$class.'-icon"';
            echo ' style="background-image: url('.$path.')"';
            echo '>';
            echo '</div>';

            // Name of screen.
            echo '<div class="'.$class.'-text">';
            echo $screen['label'];
            echo '</div>';

            // Selected arrow - shown only when active.
            echo '<div class="'.$class.'-arrow">';
            echo '</div>';

            // Divider line on right hand side of block.
            echo '<div class="'.$class.'-divider">';
            echo '</div>';

            // No access icon - shown only when disabled.
            echo '<div class="'.$class.'-no-entry">';
            echo '</div>';

            echo '</div>';

            $firstScreen = FALSE;
        }//end foreach

        echo '</div>';

    }//end printButtonsContent()


}//end class

?>
