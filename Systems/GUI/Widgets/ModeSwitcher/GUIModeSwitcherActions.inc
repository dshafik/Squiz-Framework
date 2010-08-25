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
     * Number of static buttons in mode switcher.
     *
     * @var integer
     */
    protected static $numberOfStaticButton = 1;


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
        echo '  <div class="'.$class.'-overflow GUI-alignLeft">';
        echo '      <div class="SwitcherList">';

        self::_paintStaticButtons($settings);

        $total    = count($settings['items']);
        $count    = 1;
        $items    = Util::getArrayIndex($settings, 'items', array());
        $initItem = Util::getArrayIndex($settings, 'initialItem', NULL);
        $selected = '';

        $staticButtons = Util::getArrayIndex(
            $settings,
            'numberOfStaticButton',
            self::$numberOfStaticButton
        );

        // Start 2nd list.
        echo '<div class="SwitcherList-overflow">
                <ul class="SwitcherList-mode modeList-2" style="left: -14px; width: 1000px;">';

        for ($i = $staticButtons; $i < $total; $i++) {
            $mode       = $items[$i];
            $modeSystem = Util::getArrayIndex($mode, 'system', '');

            if ($initItem === $mode['id']) {
                $selected = ' selected';
            } else {
                $selected = '';
            }

            echo '<li id="'.$id.'-'.$mode['id'].'" class="'.$class.'-modeButton'.$selected.'">';
            echo '<a href="javascript:" modeid="'.$mode['id'].'" system="'.$modeSystem.'">'.$mode['label'].'</a></li>';

            $posClass = '';
        }

        // End 2nd list.
        echo '</ul></div>';

        echo '</div></div>
            <div class="'.$class.'-expander GUI-alignLeft"><span></span></div>
        </div>';

    }//end printButtonsContent()


    /**
     * Prints the static buttons.
     *
     * @param array $settings List of settings for this widget.
     *
     * @return void
     */
    private static function _paintStaticButtons(array $settings)
    {
        echo '<ul class="SwitcherList-mode modeList-1">';
        $staticButtons = Util::getArrayIndex(
            $settings,
            'numberOfStaticButton',
            self::$numberOfStaticButton
        );

        $items    = Util::getArrayIndex($settings, 'items', array());
        $posClass = 'positionFirst';
        $selected = '';
        $initItem = Util::getArrayIndex($settings, 'initialItem', NULL);
        foreach ($items as $item) {
            if ($initItem === NULL || $initItem === $item['id']) {
                $selected = ' selected';
            } else {
                $selected = '';
            }

            $modeSystem = Util::getArrayIndex($item, 'system', '');

            echo '<li class="'.$posClass.$selected.'">';
            echo '<a href="javascript:" modeid="'.$item['id'].'" system="'.$modeSystem.'">';
            echo $item['label'].'</a></li>';

            $posClass = '';

            if (--$staticButtons <= 0) {
                break;
            }
        }//end foreach

        echo '</ul>';

    }//end _paintStaticButtons()


}//end class

?>