<?php
/**
 * Actions for the GUI Lineage Widget.
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
 * GUI Lineage Widget Actions Class.
 */
class GUILineageActions
{

    /**
     * The lineage.
     *
     * @var array
     */
    protected static $lineage = array();


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

        $lineage = Util::getArrayIndex($settings, 'lineage', array());

        echo '<div id="'.$id.'" class="'.$class.'">';
        echo '<ul>';
        foreach ($lineage as $itemid => $item) {
            self::printItem($itemid, $item, $settings);
        }

        echo '</ul>';
        echo '</div>';

        // Set the item template.
        $settings['_templates'] = array(
                                   'itemTemplate' => '<li itemid="%itemid%"><a>%title%</a></li>',
                                  );

        return $settings;

    }//end paint()


    /**
     * Prints a single lineage item.
     *
     * @param string $itemid   The ID of the item to print.
     * @param array  $item     The lineage item to print.
     * @param array  $settings Widget settings.
     *
     * @return void
     */
     public static function printItem($itemid, array $item, array $settings)
     {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        echo '<li itemid="'.$itemid.'">';
        echo '<a>'.$item['title'].'</a>';
        echo '</li>';

     }//end printItem()


}//end class

?>
