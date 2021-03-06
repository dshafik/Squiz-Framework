<?php
/**
 * Actions for the HelpToolbarButton System.
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
 * @subpackage Help
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * SaveButton Actions Class.
 */
class HelpToolbarButtonActions
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

        echo '<a title="'._('Click to view the help articles').'" id="'.$id.'" class="'.$class.' GUIToolbar-button"><div class="'.$class.'-icon"></div></a>';

        // If this is the first login for this user show the "General" articles on page load.
        Channels::includeSystem('Session');
        $helpDisplayed = Session::getSessionData('Help', 'helpDisplayed');
        if ($helpDisplayed !== TRUE) {
            $firstLogin = Session::getSessionData('User', 'firstLogin');
            if ($firstLogin === TRUE) {
                $settings['_openOnLoad'] = 'general.html';
            }
        }

        Session::setSessionData('Help', 'helpDisplayed', TRUE);

        return $settings;

    }//end paint()


}//end class

?>
