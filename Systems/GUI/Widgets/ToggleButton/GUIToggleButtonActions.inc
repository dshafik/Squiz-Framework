<?php
/**
 * Actions for the GUI Toggle Button System.
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

require_once 'Systems/BaseSystem.inc';

/**
 * GUI Toggle Button Actions Class.
 */
class GUIToggleButtonActions
{

    /**
     * The default value of the toggle.
     *
     * @var boolean
     */
    protected static $value = FALSE;


    /**
     * Prints this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<div class="'.$class.'" id="'.$id.'">';

        $selected = Util::getArrayIndex($settings, 'value', self::$value);
        if ($selected === TRUE) {
            $selected = TRUE;
        } else {
            $selected = FALSE;
        }

        // Defaults etc.
        $optionY = Util::getArrayIndex($settings, 'yes', _('Yes'));
        $optionN = Util::getArrayIndex($settings, 'no', _('No'));

        $classY = $class.' option yes';
        if (strlen($optionY) === 2) {
            $classY .= ' smallYes';
        }

        if ($selected === TRUE) {
            $classY .= ' selected';
        }

        $classN = $class.' option no';
        if ((strlen($optionN) === 2)) {
            $classN .= ' smallNo';
        }

        if ($selected === FALSE) {
            $classN .= ' selected';
        }

        $classToggle = $class.' toggle ';
        if ($selected === TRUE) {
            $classToggle .= 'toggleY';
        } else {
            $classToggle .= 'toggleN';
        }

        // Toggle Icon.
        echo '<div class="'.$classToggle.'">&nbsp;</div>';

        // Left container.
        echo '<div class="'.$class.' left">&nbsp;</div>';

        // Options.
        echo '<div class="'.$class.' middle">';
        echo '<div class="'.$classY.'">'.$optionY.'</div>';
        echo '<div class="'.$classN.'">'.$optionN.'</div>';
        echo '</div>';

        // Right container.
        echo '<div class="'.$class.' right">&nbsp;</div>';
        echo '</div>';

        return $settings;

    }//end paint()


}//end class

?>
