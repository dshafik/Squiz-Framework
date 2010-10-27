<?php
/**
 * Actions for the GUISave System.
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
 * SaveButton Actions Class.
 */
class GUISaveButtonActions
{


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

        echo '<div id="'.$id.'" class="'.$class.' GUI-alignRight">';
        echo '<button accesskey="s" id="'.$id.'-save" disabled="true" class="GUIButton buttonSave GUI-alignleft" onclick="GUI.save();"><span class="GUIButtonLabel">'._('Save').'</span></button>';
        echo '<button id="'.$id.'-revert" disabled="true" class="GUIButton buttonSaveRevert GUI-alignleft" onclick="GUI.revert();"><span class="GUIButtonLabel">'._('Revert').'</span><span class="GUIButtonIcon"></span></button>';
        echo '</div>';

        return $settings;

    }//end paint()


}//end class

?>
