<?php
/**
 * Actions for the GUI Column Browser Widget.
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
 * GUI Column Browser Widget Actions Class.
 */
class GUIColumnBrowserActions
{

    /**
     * List of items to print.
     *
     * An item can have nested children.
     * Example array structure: array(
     *                            0 => array(
     *                                  'id' => 'asset',
     *                                  'itemContent' => 'Asset',
     *                                  'children' => array(
     *                                                 0 => array(
     *                                                       'id' => 'test',
     *                                                       'itemContent' => 'Test',
     *                                                       'children' => array(..),
     *                                                     ),
     *                                                ),
     *                                 ),
     *                          ).
     *
     * @var array
     * @internal
     */
    protected static $items = array();

    /**
     * If TRUE then multiple selections will be allowed.
     *
     * @var boolean
     */
    protected static $enableMultiSelect = FALSE;

    /**
     * If specified then the lineage widget will be notified of the selection changes.
     *
     * @var string
     */
    protected static $lineageWidgetId = NULL;

    /**
     * List of columns.
     *
     * @var array
     */
    private static $_columns = array();


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

        self::$_columns = array();

        // Get the contents of columns and structure them by level.
        $items = Util::getArrayIndex($settings, 'items', self::$items);
        GUIColumnBrowser::paintColumns($settings, $items);

        echo '<div id="'.$id.'" class="'.$class.'">';
        echo '<div id="'.$id.'" class="'.$class.'-columnWrapper">';

        foreach (self::$_columns as $level => $colCards) {
            // Print the column div which will contain multiple "cards".
            echo '<div class="'.$class.'-column level-'.$level.'">';
            foreach ($colCards as $cardid => $cardContent) {
                echo '<div class="'.$class.'-column-card" parentid="'.$cardid.'">';
                echo $cardContent;
                echo '</div>';
            }

            echo '</div>';
        }

        echo '</div></div>';

        return $settings;

    }//end paint()


    /**
     * Adds the contents of columns in to columns array with a proper structure.
     *
     * @param array   $settings Widget settings.
     * @param array   $items    List of items to print for specifid level.
     * @param integer $level    The current level being printed.
     * @param string  $parentid The parent item id.
     *
     * @return void
     */
    public static function paintColumns(array $settings, array $items=array(), $level=0, $parentid='')
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];
        if (isset(self::$_columns[$level]) === FALSE) {
            self::$_columns[$level] = array();
        }

        if (isset(self::$_columns[$level][$parentid]) === FALSE) {
            self::$_columns[$level][$parentid] = '';
        }

        foreach ($items as $itemInfo) {
            ob_start();
                GUIColumnBrowser::paintItem($itemInfo, $settings);

                if (empty($itemInfo['children']) === FALSE) {
                    GUIColumnBrowser::paintColumns($settings, $itemInfo['children'], ($level + 1), $itemInfo['id']);
                }

                self::$_columns[$level][$parentid] .= ob_get_contents();
            ob_end_clean();
        }

    }//end paintColumns()


    /**
     * Prints a single column item.
     *
     * @param array $itemInfo Array containing id, itemContent (and children) info.
     * @param array $settings Widget settings.
     *
     * @return void
     */
    public static function paintItem(array $itemInfo, array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'].'-item';

        $itemid   = Util::getArrayIndex($itemInfo, 'id', '');
        $content  = Util::getArrayIndex($itemInfo, 'itemContent', '');
        $children = Util::getArrayIndex($itemInfo, 'children', array());

        echo '<div itemid="'.$itemid.'" class="'.$class.'">';
        echo '<div class="'.$class.'-content">';
        echo $content;
        echo '</div>';

        $childCount = count($children);
        if ($childCount > 0) {
            echo '<div class="'.$class.'-childCount"><span>'.$childCount.'</span></div>';
        }

        echo '</div>';

    }//end paintItem()


    /**
     * Returns the selection lineage.
     *
     * @param array $settings Widget settings.
     *
     * @return array
     */
    public static function getLineage(array $settings)
    {
        return array();

    }//end getLineage()


}//end class

?>
