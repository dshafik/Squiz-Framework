<?php
/**
 * Actions for the GUIScreenSettings System.
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
 * @subpackage Cron
 * @author     Squiz Pty Ltd <products@squiz.net>
 * @copyright  2010 Squiz Pty Ltd (ACN 084 670 600)
 * @license    http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt GPLv2
 */

require_once 'Systems/BaseSystem.inc';

/**
 * ScreenSettings Actions Class.
 */
class GUIScreenSettingsActions
{

    /**
     * Title of the box.
     *
     * @var string
     */
    protected static $title;

    /**
     * The sub title of the box.
     *
     * @var string
     */
    protected static $subTitle = '';

    /**
     * List of sections.
     *
     * @var array
     */
    protected static $sections;


    /**
     * Prints widgets content.
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
        $title    = Util::getArrayIndex($settings, 'title', '');
        $subTitle = Util::getArrayIndex($settings, 'subTitle', '');

        echo '<div class="'.$class.'-top">';
        echo '<div class="'.$class.'-title-value">'.$title.'</div>';
        echo '<div class="'.$class.'-title-subTitle">'.$subTitle.'</div>';
        echo '</div>';

        // Mid section.
        echo '<div class="'.$class.'-mid">';

        echo self::_printSectionsContent($settings);

        // End mid section.
        echo '</div>';
        echo '</div>';

    }//end paint()


    /**
     * Prints the content for the sections.
     *
     * @param array $settings The settings for the widget.
     *
     * @return void
     */
    private static function _printSectionsContent(array $settings)
    {
        $sections = Util::getArrayIndex($settings, 'sections', array());
        $class    = $settings['widget']['type'];

        foreach ($sections as $section) {
            // Title of the section.
            echo '<div class="'.$class.'-section-title">';
            echo $section['label'];
            echo '</div>';

            // Items for this section.
            $items = Util::getArrayIndex($section, 'items', array());
            GUIScreenSettings::printItems($items);
        }

    }//end _printSectionsContent()


    /**
     * Prints the items of a section.
     *
     * @param array $items List of items for a section.
     *
     * @since  4.0.0
     * @return void
     */
    public static function printItems(array $items)
    {
        foreach ($items as $item) {
            GUIScreenSettings::printItem($item);
        }

    }//end printItems()


    /**
     * Print a single item.
     *
     * @param string $itemContent Content of the item.
     *
     * @since  4.0.0
     * @return void
     */
    public static function printItem($itemContent)
    {
        $class = 'GUIScreenSettings';
        echo '<div class="'.$class.'-item">';
        echo $itemContent;
        echo '</div>';

    }//end printItem()


}//end class

?>