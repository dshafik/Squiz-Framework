<?php
/**
 * Actions for the GUI Container Switcher Widget System.
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
 * GUI Container Switcher Widget Actions Class.
 */
class GUIContainerSwitcherActions extends GUIContentSwitcherActions
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
        $id        = $settings['widget']['id'];
        $className = $settings['widget']['type'];

        $initItem = Util::getArrayIndex($settings, 'initialItem', NULL);

        echo '<div id="'.$id.'" class="'.$className.'">';

        if (isset($settings['items']) === FALSE || count($settings['items']) === 0) {
            echo '</div>';
            return;
        }

        $total      = count($settings['items']);
        $count      = 1;
        $extraClass = ' positionFirst';

        foreach ($settings['items'] as $item) {
            $itemSystem = Util::getArrayIndex($item, 'system', '');

            if ($initItem === NULL) {
                $initItem = $item['id'];
            }

            $selected = '';
            if ($item['id'] === $initItem) {
                $selected = ' selected';
            }

            echo '<div id="'.$id.'-'.$item['id'].'" class="'.$className.'-modeButton'.$extraClass.$selected.'" modeid="'.$item['id'].'" system="'.$itemSystem.'">';
            echo $item['label'];
            echo '</div>';

            if (($total - 1) === $count) {
                $extraClass = ' positionLast';
            } else {
                $extraClass = '';
            }

            $count++;
        }//end foreach

        echo '</div>';

    }//end printButtonsContent()


}//end class

?>