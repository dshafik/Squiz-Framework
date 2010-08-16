<?php
/**
 * Actions for the GUI Mode Switcher Widget System.
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
 * GUI Mode Switcher Widget Actions Class.
 */
class GUIModeSwitcherActions extends GUIContentSwitcherActions
{


    /**
     * Prints the contents of the switcher buttons.
     *
     * @param array $settings The settings of the widget.
     *
     * @return mixed
     */
    public static function printButtonsContent(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<div id="'.$id.'" class="Switcher GUIContentSwitcher '.$class.' GUI-alignLeft">';
        echo '<div class="'.$class.'-overflow GUI-alignLeft" style="width: 315px;">';

        $items = Util::getArrayIndex($settings, 'items', array());
        if (count($items) === 0) {
            echo '</div></div>';
            return;
        }

        echo '<ul>';

        $total      = count($settings['items']);
        $count      = 1;
        $extraClass = ' positionFirst';
        $initItem   = Util::getArrayIndex($settings, 'initialItem', NULL);

        foreach ($settings['items'] as $mode) {
            $modeSystem = Util::getArrayIndex($mode, 'system', '');

            if ($initItem === NULL) {
                $initItem = $mode['id'];
            }

            $selected = '';
            if ($mode['id'] === $initItem) {
                $selected = ' selected';
            }

            echo '<li id="'.$id.'-'.$mode['id'].'" class="'.$class.'-modeButton'.$extraClass.$selected.'">';
            echo '<a href="#" modeid="'.$mode['id'].'" system="'.$modeSystem.'">'.$mode['label'].'</a></li>';

            $extraClass = '';
            $count++;
        }

        echo '</ul>';
        echo '</div>
            <div class="'.$class.'-expander GUI-alignLeft"><span></span></div>
        </div>';

    }//end printButtonsContent()


}//end class

?>