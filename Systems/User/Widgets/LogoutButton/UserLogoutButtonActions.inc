<?php
/**
 * Actions for the UserLogoutButton System.
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
 * @subpackage User
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * LogoutButton Actions Class.
 */
class UserLogoutButtonActions
{


    /**
     * Prints this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return array
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<div id="'.$id.'" class="'.$class.'" title="'._('Logout').'"';
        echo ' onclick="GUI.getWidget(\''.$id.'-intervention\').show();"></div>';

        Channels::includeSystem('GUI');

        $ivContents     = '<p>'._('Are you sure you want to logout?').'</p>';
        $buttonSettings = array(
                           'value'  => _('Logout'),
                           'click'  => 'window.location=window.location.href.toString() + \'?sq_logout=true\'',
                           'colour' => 'Dark',
                          );
        $ivContents    .= GUI::createWidget($id.'-ivbuttonYes', 'GUIButton', $buttonSettings);
        $buttonSettings = array(
                           'value'  => _('Cancel'),
                           'click'  => 'GUI.getWidget(\''.$id.'-intervention\').hide();',
                           'colour' => 'Dark',
                          );
        $ivContents    .= GUI::createWidget($id.'-ivbuttonNo', 'GUIButton', $buttonSettings);

        Channels::includeSystem('GUI');
        $ivSettings = array(
                       'targetElementid' => $id,
                       'elementContents' => $ivContents,
                      );
        echo GUI::createWidget($id.'-intervention', 'GUIIntervention', $ivSettings);

        return $settings;

    }//end paint()


}//end class

?>
