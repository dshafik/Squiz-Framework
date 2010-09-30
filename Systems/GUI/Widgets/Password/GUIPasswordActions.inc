<?php
/**
 * Actions for the Password System.
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
 * Password Actions Class.
 */
class GUIPasswordActions
{


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        $ratings = array(
                    0 => _('Very weak'),
                    1 => _('Weak'),
                    2 => _('Better'),
                    3 => _('Medium'),
                    4 => _('Strong'),
                    5 => _('Strongest'),
                   );

        $matchStrs = array(
                      _('No Match'),
                      _('Matched'),
                     );

        $settings['_ratings']   = $ratings;
        $settings['_matchStrs'] = $matchStrs;

        echo '<div id="'.$id.'" class="'.$class.'">';
        echo '<div class="'.$class.'-strengthText">'.$ratings['0'].'</div>';
        echo '<div class="'.$class.'-strengthBarWrapper">
                <div class="'.$class.'-strengthBar"></div>
             </div>';

        $textBoxSettings = array(
                            'type' => 'password',
                            'size' => 28,
                           );

        $confirmTextSettings = array(
                                'size'  => 28,
                                'value' => _('Re enter password here...'),
                               );

        Channels::includeSystem('GUI');
        echo '<div class="'.$class.'-strengthMain">';
        echo GUI::createWidget($id.'-main', 'GUITextBox', $textBoxSettings);
        echo '</div>';

        // Confirm.
        echo '<div class="'.$class.'-confirmWrapper">';
        echo '<div class="'.$class.'-strengthConfirmText">';
        echo GUI::createWidget($id.'-confirmText', 'GUITextBox', $confirmTextSettings);
        echo '</div>';
        echo '<div class="'.$class.'-strengthConfirm">';
        echo GUI::createWidget($id.'-confirm', 'GUITextBox', $textBoxSettings);
        echo '</div>';

        // End confrim wrapper.
        echo '</div>';

        echo '<div class="'.$class.'-matchText">'.$matchStrs[0].'</div>';

        return $settings;

    }//end paint()


}//end class

?>
