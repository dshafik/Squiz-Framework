<?php
/**
 * Actions for the GUIBox System.
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
 * Box Actions Class.
 */
class GUIBoxActions
{

    /**
     * Title for the header.
     *
     * @var string
     */
    protected static $title = '';

    /**
     * The body content for the box container.
     *
     * @var string
     */
    protected static $content = '';

    /**
     * Extra header content.
     *
     * @var string
     */
    protected static $headerContent = '';

    /**
     * The button style.
     *
     * @var string
     */
    protected static $style = '';

    /**
     * If FALSE then the right border wont be shown.
     *
     * @var boolean
     */
    protected static $borderRight = TRUE;

    /**
     * If FALSE then the left border wont be shown.
     *
     * @var boolean
     */
    protected static $borderLeft = TRUE;


    /**
     * Prints widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings)
    {
        // Check border settings.
        $borderClass = '';

        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        include_once 'Libs/Util/Util.inc';
        $borderLeft  = Util::getArrayIndex($settings, 'borderLeft', 'true');
        $borderRight = Util::getArrayIndex($settings, 'borderRight', 'true');

        if ($borderLeft === 'false') {
            $borderClass = ' noLeftBorder';
        }

        if ($borderRight === 'false') {
            $borderClass .= ' noRightBorder';
        }

        echo '<div id="'.$id.'" class="'.$class.$borderClass.'">';
        echo '<div class="'.$class.'-header">';

        if (isset($settings['title']) === TRUE) {
            // Simple title text/widget.
            if (is_array($settings['title']) === FALSE) {
                if (empty($settings['title']) === FALSE) {
                    echo '<div class="'.$class.'-title">'.$settings['title'].'</div>';
                }
            } else if (isset($settings['title']['dataProvider']) === TRUE) {
                // Dynamic title.
                self::_getContentFromDataProvider($settings['title']['dataProvider']);
            }
        }

        if (isset($settings['headerContent']) === TRUE) {
            echo '<div class="'.$class.'-headerContent">';
            echo $settings['headerContent'];
            echo '</div>';
        }

        echo '</div><!-- end box header -->';
        echo '<div class="'.$class.'-content">';

        if (isset($settings['content']) === TRUE) {
            echo $settings['content'];
        }

        echo '</div><!--end box content-->';
        echo '</div>';

        return $settings;

    }//end paint()


    private static function _getContentFromDataProvider($dataProvider)
    {
        echo 'DATA PROVIDER';

    }//end _getContentFromDataProvider()


}//end class

?>
