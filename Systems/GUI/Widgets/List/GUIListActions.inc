<?php
/**
 * Actions for the GUI List System.
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
 * GUI List Actions Class.
 */
class GUIListActions
{


    /**
     * Returns this widgets content.
     *
     * @param array $settings The list of settings for this widget.
     *
     * @return void
     */
    public static function paint(array $settings=array())
    {
        echo '<div class="%class%-left" id="%id%-left">';
        echo '<div class="%class%-right" id="%id%-right">';
        echo '<ul class="%class%" id="%id%">';

        // Print the list items.
        GUIList::paintItems(
            $settings['dataProvider'],
            (bool) $settings['sortable'],
            $settings['widget']['id'],
            $settings['widget']['type']
        );

        echo '</ul>';
        echo '</div>';
        echo '</div>';

    }//end paint()


    /**
     * Paint the items of the list.
     *
     * @param array   $items    An array of items to print.
     * @param integer $sortable Should drag-handles be printed.
     * @param string  $id       The ID used by the list.
     * @param string  $class    The class used by the list.
     *
     * @return void
     */
    public static function paintItems(array $items, $sortable, $id, $class)
    {
        $sortable = (int) $sortable;
        $title    = _('Click and drag to change order');

        foreach ($items as $item) {
            echo '<li>';
            if ($sortable === 1) {
                echo '<span class="'.$class.'-dragHandle" title="'.$title.'"></span>';
            }

            echo $item.'</li>';
        }

    }//end paintItems()


}//end class

?>