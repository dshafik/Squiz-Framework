<?php
/**
 * Actions for the Toolbar System.
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
 * Toolbar Actions Class.
 */
class GUIToolbarActions
{

    /**
     * URL of the toolbar logo.
     *
     * @var string
     */
    protected static $logo;

    /**
     * Content of the toolbar
     *
     * @var string
     */
    protected static $content;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings)
    {
        if (isset($settings['logo']) === FALSE
            || $settings['logo'] === ''
        ) {
            $settings['logo'] = '/__web/Systems/GUI/Widgets/Toolbar/Web/logo.png';
        }

        $className = $settings['widget']['type'];
        echo '<div class="'.$className.'">
                <div class="'.$className.'-logo" style="background: url(\''.$settings['logo'].'\') no-repeat 0 0;"></div>
                    <div class="'.$className.'-content">';

        if (isset($settings['content']) === TRUE) {
            echo $settings['content'];
        }

        echo '</div>
            </div>'."\n";

    }//end paint()


}//end class

?>