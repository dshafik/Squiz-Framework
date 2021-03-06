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
     * If specified then the item children will be retrieved from specified action.
     *
     * The value must be an array with 'system' and 'method' names.
     *
     * Note that the array returned must be in following format:
     * array(
     *  0 => array(
     *        'id'          => 'itemid',
     *        'title'       => 'title of the item for lineage widgets',
     *        'itemContent' => 'The item content',
     *        'childCount'  => 123,
     *        'isButton'    => FALSE, // Optional, if TRUE then item will appear as a button.
     *       ),
     * ).
     *
     * @var array
     */
    protected static $dynamicDataProvider = NULL;

    /**
     * If TRUE then data can be loaded dynamically using the dynamicDataProvider.
     *
     * Note that dynamicDataProvider setting must be specified if this setting is set
     * to TRUE.
     *
     * @var boolean
     */
    protected static $dynamicLoading = FALSE;

    /**
     * Initial item id.
     *
     * @var string
     */
    protected static $initialItemid = NULL;

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
     * @throws ChannelException If there are invalid settings.
     */
    public static function paint(array $settings)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];

        if (Util::getArrayIndex($settings, 'dynamicLoading', self::$dynamicLoading) === TRUE
            && Util::getArrayIndex($settings, 'dynamicDataProvider', NULL) === NULL
        ) {
            throw new ChannelException(_('dynamicLoading is set to TRUE but dynamicDataProvider setting is not set'));
        }

        self::$_columns = array();

        // Get the contents of columns and structure them by level.
        $items = Util::getArrayIndex($settings, 'items', self::$items);
        GUIColumnBrowser::paintColumns($settings, $items);

        echo '<div id="'.$id.'" class="'.$class.'">';
        echo '<div id="'.$id.'" class="'.$class.'-columnWrapper">';

        $firstCardClass = ' visible';

        foreach (self::$_columns as $level => $colCards) {
            // Print the column div which will contain multiple "cards".
            echo '<div class="'.$class.'-column level-'.$level.'">';
            foreach ($colCards as $cardid => $cardInfo) {
                echo '<div class="'.$class.'-column-card'.$firstCardClass.'" parentid="'.$cardid.'" parentTitle="'.$cardInfo['title'].'">';
                echo $cardInfo['content'];
                echo '</div>';

                $firstCardClass = '';
            }

            echo '</div>';
        }

        echo '</div></div>';

        return $settings;

    }//end paint()


    /**
     * Adds the contents of columns in to columns array with a proper structure.
     *
     * @param array   $settings    Widget settings.
     * @param array   $items       List of items to print for specifid level.
     * @param integer $level       The current level being printed.
     * @param string  $parentid    The parent item id.
     * @param string  $parentTitle Title of the parent element, used by Lineage widgets.
     *
     * @return void
     */
    public static function paintColumns(array $settings, array $items=array(), $level=0, $parentid='', $parentTitle='')
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'];
        if (isset(self::$_columns[$level]) === FALSE) {
            self::$_columns[$level] = array();
        }

        if (isset(self::$_columns[$level][$parentid]) === FALSE) {
            self::$_columns[$level][$parentid] = array(
                                                  'title'   => $parentTitle,
                                                  'content' => '',
                                                 );
        }

        foreach ($items as $itemInfo) {
            ob_start();
            GUIColumnBrowser::paintItem($itemInfo, $settings);

            if (empty($itemInfo['children']) === FALSE) {
                // Paint children in a new column.
                GUIColumnBrowser::paintColumns($settings, $itemInfo['children'], ($level + 1), $itemInfo['id'], $itemInfo['title']);
            }

            self::$_columns[$level][$parentid]['content'] .= ob_get_contents();
            ob_end_clean();
        }

    }//end paintColumns()


    /**
     * Prints a single column item.
     *
     * @param array   $itemInfo Array containing id, itemContent (and children) info.
     * @param array   $settings Widget settings.
     * @param boolean $selected If TRUE then item will be marked as selected.
     *
     * @return void
     */
    public static function paintItem(array $itemInfo, array $settings, $selected=FALSE)
    {
        $id    = $settings['widget']['id'];
        $class = $settings['widget']['type'].'-item';

        $itemid     = Util::getArrayIndex($itemInfo, 'id', '');
        $content    = Util::getArrayIndex($itemInfo, 'itemContent', '');
        $children   = Util::getArrayIndex($itemInfo, 'children', array());
        $childCount = Util::getArrayIndex($itemInfo, 'childCount', NULL);
        $isButton   = Util::getArrayIndex($itemInfo, 'isButton', FALSE);
        $title      = Util::getArrayIndex($itemInfo, 'title', $itemid);

        if ($childCount === NULL && empty($children) === TRUE) {
            $childCount = count($children);
        }

        $buttonClass = '';
        if ($isButton === TRUE) {
            $buttonClass .= ' '.$class.'-button';
        }

        $selectedClass = '';
        if ($selected === TRUE) {
            $selectedClass = ' selected lastClicked';
        }

        $content = trim($content);
        if (empty($content) === TRUE) {
            $content = '&nbsp;';
        }

        echo '<div itemid="'.$itemid.'" itemtitle="'.$title.'" class="'.$class.$buttonClass.$selectedClass.'" childcount="'.$childCount.'">';
        echo '<div class="'.$class.'-content'.$buttonClass.'">';
        echo $content;
        echo '</div>';

        if ($childCount > 0 && $isButton === FALSE) {
            echo '<div class="'.$class.'-childCount"><span>'.$childCount.'</span></div>';
        }

        echo '</div>';

    }//end paintItem()


    /**
     * Returns the contents of the items requested.
     *
     * @param array  $settings Settings of the widget.
     * @param string $parentid The id of the parent.
     *
     * @return string
     * @throws ChannelException If the channel setting is invalid.
     *
     * @api            read
     * @api-permission public
     */
    public static function getChildrenContents(array $settings, $parentid)
    {
        $actionInfo = Util::getArrayIndex($settings, 'dynamicDataProvider', NULL);
        if ($actionInfo === NULL) {
            return NULL;
        }

        // Get system and method.
        $system = Util::getArrayIndex($actionInfo, 'system', NULL);
        $method = Util::getArrayIndex($actionInfo, 'method', NULL);
        if ($system === NULL || $method === NULL) {
            return NULL;
        }

        // Check if channel is callable.
        Channels::includeSystem('API');
        $api = API::getAPI($system, $method);
        if ($api === FALSE) {
            throw new ChannelException(_('Specified channel is invalid'));
        }

        $items = API::executeRequest($system, $method, array($parentid), $api['options']);

        if (empty($items) === TRUE) {
            return '&nbsp;';
        }

        ob_start();
        foreach ($items as $item) {
            GUIColumnBrowser::paintItem($item, $settings);
        }

        $contents = ob_get_contents();
        ob_end_clean();

        return $contents;

    }//end getChildrenContents()


    /**
     * Returns the contents of specifed lineage.
     *
     * @param array $settings Settings of the widget.
     * @param array $lineage  Lienage to expand to.
     *
     * @return string
     * @throws ChannelException If the channel setting is invalid.
     *
     * @api            read
     * @api-permission public
     */
    public static function reloadContents(array $settings, array $lineage=array())
    {
        $actionInfo = Util::getArrayIndex($settings, 'dynamicDataProvider', NULL);
        if ($actionInfo === NULL) {
            return NULL;
        }

        // Get system and method.
        $system = Util::getArrayIndex($actionInfo, 'system', NULL);
        $method = Util::getArrayIndex($actionInfo, 'method', NULL);
        if ($system === NULL || $method === NULL) {
            return NULL;
        }

        // Check if channel is callable.
        Channels::includeSystem('API');
        $api = API::getAPI($system, $method);
        if ($api === FALSE) {
            throw new ChannelException(_('Specified channel is invalid'));
        }

        $contents       = '';
        $level          = 0;
        $childSelection = NULL;
        $total          = count($lineage);

        foreach ($lineage as $parentid) {
            $items = API::executeRequest($system, $method, array($parentid), $api['options']);

            if (empty($items) === TRUE) {
                continue;
            }

            if ($level < $total) {
                $childSelection = $lineage[($level + 1)];
            }

            // Wrappers.
            $contents .= '<div class="GUIColumnBrowser-column level-'.($level++).' visible';
            if ($level === ($total - 1)) {
                $contents .= ' active';
            }

            $contents .= '">';

            $contents .= '<div class="GUIColumnBrowser-column-card visible" parentid="'.$parentid.'">';

            ob_start();
            foreach ($items as $item) {
                $selected = FALSE;
                if ((string) $childSelection === (string) $item['id']) {
                    $selected = TRUE;
                }

                GUIColumnBrowser::paintItem($item, $settings, $selected);
            }

            $contents .= ob_get_contents();
            ob_end_clean();

            $contents .= '</div></div>';
        }//end foreach

        return $contents;

    }//end reloadContents()


}//end class

?>
