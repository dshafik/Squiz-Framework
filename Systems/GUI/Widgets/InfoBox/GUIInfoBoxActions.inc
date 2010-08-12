<?php
/**
 * Actions for the GUI InfoBox System.
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
 * GUI InfoBox Actions Class.
 */
class GUIInfoBoxActions
{

    /**
     * The main title of the info box.
     *
     * @var string
     */
    protected static $title = '';

    /**
     * Thumbnail URL.
     *
     * @var string
     */
    protected static $thumbURL = '';

    /**
     * Content for the bottom section.
     *
     * @var string
     */
    protected static $content = '';


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];
        echo '<div id="'.$id.'" class="'.$class.'">';

        // Top section.
        echo '<div class="'.$class.'-top">';
        echo '<div class="'.$class.'-title">';
        echo '<div class="'.$class.'-title-icon"></div>';
        echo '<div class="'.$class.'-title-value">'.$settings['title'].'</div>';
        echo '</div>';
        echo '</div>';

        // Mid section.
        echo '<div class="'.$class.'-mid">';
        echo '<div class="'.$class.'-thumbContainer">';
        echo '<img src="'.$settings['thumbURL'].'" />';
        echo '</div>';
        echo '</div>';

        // Bottom section.
        echo '<div class="'.$class.'-bottom">';
        echo $settings['content'];
        echo '</div>';

        echo '</div>';

    }//end paint()


}//end class

?>
